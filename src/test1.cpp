#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <math.h>
#include <chrono>
#include <sstream>

Display* dpy=XOpenDisplay(0);
int scr=XDefaultScreen(dpy);
Window root_window=XRootWindow(dpy, scr);
int max_y=DisplayHeight(dpy, scr);
int max_x=DisplayWidth(dpy, scr);

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

int main()
{
	cv::VideoCapture cap;
	cap.open("/dev/video1");
	if(!cap.isOpened())
	{
		std::cout<<"Unable to open camera\n";
		return 0;
	}
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor predictor;
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;

	int cur_x = max_x/2, cur_y = max_y/2, old_x = 0, old_y = 0;

	XWarpPointer(dpy, 0L, root_window, 0, 0, 0, 0, cur_x, cur_y);
	XFlush(dpy);
	usleep(50);

	//int ear_count = 5, cur_count = 0; double ear_val = 0;
	//double ear_thresh = 15;

	//first value of each pair is for right eye and second for left eye

	int ear_count = 3, cur_count = 0; //5
	std::pair<double, double> ear_val = {0, 0}; double ear_thresh = 0.30;//0.27
	std::pair<int, int> blink_count = {0, 0};
	int halt_blink_for = 10, halt_blink = 0;
	bool blinked = false;

	int tilt_thresh = 20;
	double tilt_val = 0;

	std::vector<int> points = {31, 32, 33, 34, 35};

	std::vector<std::vector<int>> right_eye = {{36}, {37, 41}, {38, 40}, {39}};
	std::vector<std::vector<int>> left_eye = {{42}, {43, 47}, {44, 46}, {45}};

	cv::Mat temp;
	while(1)
	{
		cap >> temp;
		cv::resize(temp, temp, cv::Size(300, 300));
		//cv::cvtColor(temp, temp, CV_BGR2GRAY);
		auto start = std::chrono::high_resolution_clock::now();
		dlib::cv_image<dlib::bgr_pixel> cimg(temp);
		std::vector<dlib::rectangle> faces = detector(cimg);
		int x_thresh, y_thresh;
		if(faces.size() > 0)
		{
			double x = 0, y = 0;
			dlib::full_object_detection shape = predictor(cimg, faces[0]);

			double temp_tilt = shape.part(0).y() - shape.part(16).y();
			std::cout << temp_tilt << '\n';

			std::pair<double, double> temp_ear = {EAR(right_eye, shape), EAR(left_eye, shape)};
			if(cur_count == ear_count)
			{
				ear_val.first /= ear_count;
				ear_val.second /= ear_count;
				tilt_val /= ear_count;

				std::cout << tilt_val <<'\n';

				if(ear_val.first < ear_thresh || ear_val.second < ear_thresh)
				{
					if(fabs(tilt_val) > tilt_thresh && halt_blink == 0)
					{
						if((temp_ear.first < ear_thresh) && temp_ear.second < ear_thresh)
						{
							if(tilt_val > 0)
							{
								std::cout << "Right Click\n";
								blink_count.first++;
								blinked = true;
								halt_blink = 1;
//								XTestFakeButtonEvent(dpy, 3, True, 0);
//								XFlush(dpy);
//								XTestFakeButtonEvent(dpy, 3, False, 0);
//								XFlush(dpy);
//								usleep(50);
							}
							else
							{
								std::cout << "Left Click\n";
								blink_count.second++;
								blinked = true;
								halt_blink = 1;
//								XTestFakeButtonEvent(dpy, 2, True, 0);
//								XFlush(dpy);
//								XTestFakeButtonEvent(dpy, 2, False, 0);
//								XFlush(dpy);
//								usleep(50);
							}
						}
					}
				}
				cur_count = 0;
				ear_val = {0, 0};
				tilt_val = 0;
			}
			ear_val.first += temp_ear.first;
			ear_val.second += temp_ear.second;
			tilt_val += temp_tilt;

			for(const auto& i : points)
			{
				x += shape.part(i).x();
				y += shape.part(i).y();
			}
			x /= points.size(); y /= points.size();
			x = max_x / 2 - x;

			for(const auto& i: points)
				cv::circle(temp, cv::Point(shape.part(i).x(), shape.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);

			for(const auto& i: right_eye)
				for(const auto& j : i)
					cv::circle(temp, cv::Point(shape.part(j).x(), shape.part(j).y()), 2, cv::Scalar(0, 0, 255), -1);

			for(const auto& i: left_eye)
				for(const auto& j : i)
					cv::circle(temp, cv::Point(shape.part(j).x(), shape.part(j).y()), 2, cv::Scalar(0, 0, 255), -1);
			if(old_x == 0 && old_y == 0)
			{
				old_x = x; old_y = y;
				continue;
			}
			else
			{
				int dx = x - old_x, dy = y - old_y;

				if(abs(dx*max_x/190.0) >= 5 || abs(dy*max_y/107.0) >= 5)
				{
					XWarpPointer(dpy, 0L, root_window, 0, 0, 0, 0, x*(max_x/190.0) - max_x*3.8, y*(max_y/107.0)-max_y*1.4);
					XFlush(dpy);
					usleep(50);
				}
				old_x = x; old_y = y;
			}
			cur_count++;
			if(blinked)
			{
				halt_blink += 1;
				if(halt_blink == halt_blink_for)
				{
					halt_blink = 0; blinked = false;
				}
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		std::stringstream text;
		text << "FPS: "<<std::setprecision(3)<<std::fixed<<(1/diff.count());
		//std::cout<<"FPS: "<<1/diff.count()<<'\n';
		cv::putText(temp, text.str(), cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

		text = std::stringstream("");
		text << "Right Blinks: " << blink_count.first;
		cv::putText(temp, text.str(), cv::Point(150, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

		text = std::stringstream("");
		text << "Left Blinks: " << blink_count.second;
		cv::putText(temp, text.str(), cv::Point(150, 35), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

		cv::imshow("Demo", temp);
		cv::waitKey(1);
	}
}
