#include <NeuralNetworkSettings.hpp>
#include <iostream>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>
#include <MouseSettings.hpp>
#include <opencv2/core.hpp>
#include <MyTimer.h>
#include <sstream>

#define cvPoint()
struct Utils
{
	cv::Scalar 
		WHITE_COLOR = cv::Scalar(255, 255, 255),
		YELLOW_COLOR = cv::Scalar(0, 255, 255),
		RED_COLOR = cv::Scalar(0, 0, 255),
		GREEN_COLOR = cv::Scalar(0, 255, 0),
		BLUE_COLOR = cv::Scalar(255, 0, 0),
		BLACK_COLOR = cv::Scalar(0, 0, 0),
		BROWN_COLOR = cv::Scalar(42, 42, 165),
		MAROON_COLOR = cv::Scalar(0, 0, 128),
		ORANGE_COLOR = cv::Scalar(0, 165, 255);
	std::pair<int,int>
		MOUTH = std::make_pair(48,68),
		LEYE = std::make_pair(42,48),
		REYE = std::make_pair(36,42),
		LBROW = std::make_pair(22,27),
		RBROW = std::make_pair(17,22),
		NOSE = std::make_pair(27,35),
		JAW = std::make_pair(0,17);

	bool SCROLL_MODE = false, CLICK_MODE = false;
	
	void setParams(bool x, bool y)
	{
		SCROLL_MODE = x; CLICK_MODE = y;
	}
	double dist(std::pair<double, double> p1, std::pair<double, double> p2)
	{
		return sqrt((p1.first-p2.first)*(p1.first-p2.first) + (p1.second-p2.second)*(p1.second-p2.second));
	}
	
	double EAR(std::vector<std::vector<int>> &points, dlib::full_object_detection &shape)
	{
		double x1 = dist({shape.part(points[1][0]).x(), shape.part(points[1][0]).y()}, {shape.part(points[1][1]).x(), shape.part(points[1][1]).y()});
		double x2 = dist({shape.part(points[2][0]).x(), shape.part(points[2][0]).y()}, {shape.part(points[2][1]).x(), shape.part(points[2][1]).y()});
		double x3 = dist({shape.part(points[0][0]).x(), shape.part(points[0][0]).y()}, {shape.part(points[3][0]).x(), shape.part(points[3][0]).y()});
		return (x1 + x2)/(2.0 * x3);
	}

	double EAR_twist(std::vector<std::vector<int>> &points, dlib::full_object_detection &shape)
	{
		double x1 = dist({shape.part(points[1][0]).x(), shape.part(points[1][0]).y()}, {shape.part(points[1][1]).x(), shape.part(points[1][1]).y()});
		double x2 = dist({shape.part(points[2][0]).x(), shape.part(points[2][0]).y()}, {shape.part(points[2][1]).x(), shape.part(points[2][1]).y()});
		return x1 + x2;
	}

	double MAR(dlib::full_object_detection &shape)
	{
		int offset = MOUTH.first;
		std::vector<int> idx = {13, 19, 14, 18, 15, 17, 12, 16};
		for(int &i: idx) i+=offset;
		double
			x1 = dist({shape.part(idx[0]).x(), shape.part(idx[0]).y()}, {shape.part(idx[1]).x(), shape.part(idx[1]).y()}),
			x2 = dist({shape.part(idx[2]).x(), shape.part(idx[2]).y()}, {shape.part(idx[3]).x(), shape.part(idx[3]).y()}),
			x3 = dist({shape.part(idx[4]).x(), shape.part(idx[4]).y()}, {shape.part(idx[5]).x(), shape.part(idx[5]).y()}),
			x4 = dist({shape.part(idx[6]).x(), shape.part(idx[6]).y()}, {shape.part(idx[7]).x(), shape.part(idx[7]).y()});
		double mar = (x1+x2+x3)/(2.0*x4);
		// std::cout << mar << "\n";
		// if(mar > 0.25)
		// 	std::cout << "SCROLL ON\n";
		return mar;
	}

	void scrollAux(dlib::full_object_detection &shape)
	{
		double mar = MAR(shape);
		// std::cout << mar << "\n";
		if(mar > 0.27)
		{
			SCROLL_MODE = true;
			CLICK_MODE = false;
			// std::cout << "scroll ON\n";
		}	
		else
		{
			SCROLL_MODE = false;
			// std::cout << "scroll OFF\n";
		}
	}
};

struct Video
{
	Utils util;
    int ear_count = 3;
    std::pair<double, double> ear_val = {0, 0};
    double ear_thresh = 0.30;//0.27
	std::pair<int, int> blink_count = {0, 0};
	int halt_blink_for = 10, halt_blink = 0;
	bool blinked = false;
	int tilt_thresh = 20;
	double tilt_val = 0;

	std::vector<int> points = {31, 32, 33, 34, 35};
	std::vector<std::vector<int>> right_eye = {{36}, {37, 41}, {38, 40}, {39}};
	std::vector<std::vector<int>> left_eye = {{42}, {43, 47}, {44, 46}, {45}};

	void add_mouth_overlay(cv::Mat &tmp, dlib::full_object_detection &shape)
	{
		int len = util.MOUTH.second - util.MOUTH.first;
		// int offset = util.MOUTH.first;
		// std::vector<int> idxvec = {13, 19, 14, 18, 15, 17, 12, 16};
		for(int i=0; i<len-1; i++)
		{
			int j = i+util.MOUTH.first;
			cv::line(tmp, 
					cv::Point(shape.part(j+1).x(), shape.part(j+1).y()), 
					cv::Point(shape.part(j).x(), shape.part(j).y()), 
						util.GREEN_COLOR, 2, CV_AA);
		}
	}
	
	void add_scroll_overlay(cv::Mat &tmp, dlib::full_object_detection &shape, int t = 1)
	{
		std::string msg = "scroll " + std::string(t==1? "ON" : "OFF");
		cv::putText(tmp, msg, cv::Point(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, util.RED_COLOR, 1, CV_AA);
	} 
};