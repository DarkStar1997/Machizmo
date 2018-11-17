#include <Utilities.hpp>

Mouse mouse;

void function(double &x, double &y)
{
    x = x * (mouse.max_x / 190.0) - mouse.max_x * 0.0;
    y = y * (mouse.max_y / 107.0) - mouse.max_y * 1.4;
}

int main()
{
    Utils util;
    Video vid;
    mouse.setScalingFunction(function);
    cv::VideoCapture cap("/dev/video0");
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor predictor;
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;
    
    int cur_x = mouse.max_x / 2, cur_y = mouse.max_y / 2, old_x = 0, old_y = 0, cur_count = 0;
    mouse.moveTo(cur_x, cur_y);

    cv::Mat temp;
    while(true)
    {
        cap >> temp;
        cv::resize(temp, temp, cv::Size(300, 300));
        auto start = now();
        dlib::cv_image<dlib::bgr_pixel> cimg(temp);
		std::vector<dlib::rectangle> faces = detector(cimg);
		int x_thresh, y_thresh;
        if(faces.size() > 0)
        {
            double x = 0, y = 0;
            dlib::full_object_detection shape = predictor(cimg, faces[0]);  // Getting face landmarks
            
            double temp_tilt = shape.part(0).y() - shape.part(16).y();   //Getting tilt
			std::cout << temp_tilt << '\n';
            std::pair<double, double> temp_ear = {util.EAR(vid.right_eye, shape), util.EAR(vid.left_eye, shape)};
            if(cur_count == vid.ear_count)
            {

            }
        }
    }
}