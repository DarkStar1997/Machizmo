#include <iostream>
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <chrono>
#include <string>

Display* dpy=XOpenDisplay(0);
int scr=XDefaultScreen(dpy);
Window root_window=XRootWindow(dpy, scr);
int max_y=DisplayHeight(dpy, scr);
int max_x=DisplayWidth(dpy, scr);

using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------

template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = relu<affine<con5<45,SUBNET>>>;

using net_type = loss_mmod<con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

// ----------------------------------------------------------------------------------------


int main(int argc, char** argv) try
{
    net_type net;
    deserialize("mmod_human_face_detector.dat") >> net;  

    int cur_x = max_x/2, cur_y = max_y/2, old_x = 0, old_y = 0;

    XWarpPointer(dpy, 0L, root_window, 0, 0, 0, 0, cur_x, cur_y);
    XFlush(dpy);
    usleep(50);

    cv::VideoCapture cap("/dev/video1");
    cv::Mat tmp;
    while(true)
    {
        cap >> tmp;
        cv::flip(tmp, tmp, 1);
        cv::resize(tmp, tmp, cv::Size(300, 300));
        matrix<rgb_pixel> img;
        assign_image(img, cv_image<dlib::bgr_pixel>(tmp));
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
            XWarpPointer(dpy, 0L, root_window, 0, 0, 0, 0, x*(max_x/190.0) - max_x * 0, y*(max_y/107.0)-max_y*1.4);
            XFlush(dpy);
            usleep(50);
        }
        cv::imshow("Frame", tmp);
        cv::waitKey(1);
    }
}
catch(std::exception& e)
{
    cout << e.what() << endl;
}


