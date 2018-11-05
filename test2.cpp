#include <iostream>
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>

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

    image_window win;
    cv::VideoCapture cap("/dev/video0");
    cv::Mat tmp;
    while(true)
    {
    	cap >> tmp;
//		matrix<rgb_pixel> img;
//		 load_image(img, argv[i]);

		cv_image<dlib::bgr_pixel> img1(tmp);
		matrix<rgb_pixel> img;
		assign_image(img, img1);
	    // Upsampling the image will allow us to detect smaller faces but will cause the
	    // program to use more RAM and run longer.
//	    while(img.size() < 1800*1800)
//	        pyramid_up(img);

	    // Note that you can process a bunch of images in a std::vector at once and it runs
	    // much faster, since this will form mini-batches of images and therefore get
	    // better parallelism out of your GPU hardware.  However, all the images must be
	    // the same size.  To avoid this requirement on images being the same size we
	    // process them individually in this example.
	    auto dets = net(img);
	    win.clear_overlay();
	    win.set_image(img);
	    for (auto&& d : dets)
	        win.add_overlay(d);
		cv::waitKey(1);
//		cout << "Hit enter to process the next image." << endl;
//		cin.get();
	}
}
catch(std::exception& e)
{
    cout << e.what() << endl;
}


