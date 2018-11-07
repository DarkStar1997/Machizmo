#include <NeuralNetworkSettings.hpp>
#include <iostream>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>
#include <MouseSettings.hpp>

Mouse mouse;

void function(double &x, double &y)
{
    x = x * (mouse.max_x / 190.0) - mouse.max_x * 0.0;
    y = y * (mouse.max_y / 107.0) - mouse.max_y * 1.4;
}

int main(int argc, char** argv)
{
    mouse.setScalingFunction(function);
    net_type net;
    dlib::deserialize("mmod_human_face_detector.dat") >> net;  

    int cur_x = mouse.max_x/2, cur_y = mouse.max_y/2, old_x = 0, old_y = 0;

    mouse.moveTo(cur_x, cur_y);

    cv::VideoCapture cap("/dev/video0");
    cv::Mat tmp;
    while(true)
    {
        cap >> tmp;
        cv::flip(tmp, tmp, 1);
        cv::resize(tmp, tmp, cv::Size(300, 300));
        dlib::matrix<dlib::rgb_pixel> img;
        dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(tmp));
        auto dets = net(img);
        if(dets.size() > 0)
        {
            for (int d = 0; d < dets.size(); d++)
            {
                std::string txt = "Face ";
                std::cout<<dets[d].rect.tl_corner() << ' ' << dets[d].rect.br_corner() << '\n';
                cv::rectangle(tmp, cv::Point(dets[d].rect.tl_corner()(0), dets[d].rect.tl_corner()(1)), cv::Point(dets[d].rect.br_corner()(0), dets[d].rect.br_corner()(1)), cv::Scalar(0, 0, 255));
                cv::putText(tmp, txt + std::to_string(d), cv::Point(dets[d].rect.tl_corner()(0), dets[d].rect.tl_corner()(1)), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 0, 0), 2.0);
            }
            int x = (dets[0].rect.tl_corner()(0) + dets[0].rect.br_corner()(0)) / 2;
            int y = (dets[0].rect.tl_corner()(1) + dets[0].rect.br_corner()(1)) / 2;
            mouse.moveTo(x, y, true);
        }
        cv::imshow("Frame", tmp);
        cv::waitKey(1);
    }
}
