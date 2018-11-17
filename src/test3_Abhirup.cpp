#include <Utilities.hpp>
Mouse mouse;
Utils util;
Video vid;
void function(double &x, double &y)
{
    x = x * (mouse.max_x / 190.0) - mouse.max_x * 0.0;
    y = y * (mouse.max_y / 107.0) - mouse.max_y * 1.4;
}
void grab_scroll()
{
    int x,y, t = util.SCROLL_MODE? 1:0;
    if(t)
    {
        std::tie(x,y) = mouse.currPos();    
        if(y > mouse.prev_y)
            mouse.scrollDown();
        else if (y < mouse.prev_y)
            mouse.scrollUp();
    }
}
int main(int argc, char** argv)
{
    util.SCROLL_MODE = false; util.CLICK_MODE = false;
    mouse.setScalingFunction(function);
    net_type net;
    dlib::deserialize("mmod_human_face_detector.dat") >> net;  
    dlib::shape_predictor predictor;
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;
    int cur_x = mouse.max_x/2, cur_y = mouse.max_y/2, old_x = 0, old_y = 0;

    mouse.moveTo(cur_x, cur_y);

    cv::VideoCapture cap("/dev/video0");
    cv::Mat tmp;
    while(true)
    {
        cap >> tmp;
        cv::flip(tmp, tmp, 1);
        cv::resize(tmp, tmp, cv::Size(300, 300));
        // dlib::matrix<dlib::rgb_pixel> img;
        // dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(tmp));
        dlib::cv_image<dlib::bgr_pixel> img(tmp);
        std::vector<dlib::rectangle> faces = detector(img);
        if(faces.size() > 0)
        {
            dlib::full_object_detection shape = predictor(img, faces[0]);
            util.scrollAux(shape); // sets scroll mode accordingly
            // std::cout << "my mode " << (util.SCROLL_MODE? "scroll ON":"scroll OFF") << "\n";
            // if(util.SCROLL_MODE)
            // {
            //     vid.add_mouth_overlay(tmp, shape);
            // }
            vid.add_scroll_overlay(tmp, shape, (util.SCROLL_MODE? 1 : 0));
            grab_scroll(); //PENDING: this doesnt work yet because Pointer movement from Face is not complete
            //MAR calculation is okay.
        }
        else if(faces.size() == 0)
        {
            // auto dets = net(img);
            // for (int d = 0; d < dets.size(); d++)
            // {
            //     std::string txt = "Face ";
            //     std::cout << dets.size() << "\n";
            //     std::cout << dets[d].rect.tl_corner() << ' ' << dets[d].rect.br_corner() << '\n';
            //     cv::rectangle(tmp, cv::Point(dets[d].rect.tl_corner()(0), dets[d].rect.tl_corner()(1)), cv::Point(dets[d].rect.br_corner()(0), dets[d].rect.br_corner()(1)), cv::Scalar(0, 0, 255));
            //     cv::putText(tmp, txt + std::to_string(d), cv::Point(dets[d].rect.tl_corner()(0), dets[d].rect.tl_corner()(1)), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 0, 0), 2.0);
            // }

            // ISSUE: for non-CNN faces, ".rect" is not available 
            // int x = (faces[0].rect.tl_corner()(0) + faces[0].rect.br_corner()(0)) / 2;
            // int y = (faces[0].rect.tl_corner()(1) + faces[0].rect.br_corner()(1)) / 2;
        }
        cv::imshow("Frame", tmp);
        cv::waitKey(1);
    }
}
