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
};

struct Video
{
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
};