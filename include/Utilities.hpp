#include <NeuralNetworkSettings.hpp>
#include <iostream>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>
#include <MouseSettings.hpp>
#include <opencv2/core.hpp>

auto WHITE_COLOR = cv::Scalar(255, 255, 255),
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

bool SCROLL_MODE, CLICK_MODE;