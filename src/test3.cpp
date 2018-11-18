#include <Utilities.hpp>

Mouse mouse;

void function(double &x, double &y)
{
    x = x * (mouse.max_x / 190.0) - mouse.max_x * 3.8;
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
			//std::cout << temp_tilt << '\n';
            std::pair<double, double> temp_ear = {util.EAR(vid.right_eye, shape), util.EAR(vid.left_eye, shape)};
            
            for(const auto& i : vid.points)
            {
                x += shape.part(i).x();
				y += shape.part(i).y();
            }
            
            x /= vid.points.size(); y /= vid.points.size();
			x = mouse.max_x / 2 - x;
            
            util.scrollAux(shape);
            if(cur_count == vid.ear_count)
            {
                vid.ear_val.first /= vid.ear_count;
                vid.ear_val.second /= vid.ear_count;
                vid.tilt_val /= vid.ear_count;

                std::cout << vid.tilt_val << '\n' << old_y;
                //std::cout << y << " " << old_y << '\n';
                if(util.SCROLL_MODE)
                {
                    if(y * (mouse.max_y / 107.0) - mouse.max_y * 1.4 < mouse.max_y / 2)
                        mouse.scrollUp();
                    else if(y * (mouse.max_y / 107.0) - mouse.max_y * 1.4 > mouse.max_y / 2)
                        mouse.scrollDown();
                }

                else if(vid.ear_val.first < vid.ear_thresh || vid.ear_val.second < vid.ear_thresh)
				{
					if(fabs(vid.tilt_val) > vid.tilt_thresh && vid.halt_blink == 0)
					{
						if((temp_ear.first < vid.ear_thresh) && temp_ear.second < vid.ear_thresh)
						{
							if(vid.tilt_val > 0)
							{
								std::cout << "Right Click\n";
								vid.blink_count.first++;
								vid.blinked = true;
								vid.halt_blink = 1;
                                mouse.rightClick();cv::flip(temp, temp, 1);
        
							}
							else
							{
								std::cout << "Left Click\n";
								vid.blink_count.second++;
								vid.blinked = true;
								vid.halt_blink = 1;
                                mouse.leftClick();
							}
						}
					}
				}
				cur_count = 0;
				vid.ear_val = {0, 0};
				vid.tilt_val = 0;
            }
            vid.ear_val.first += temp_ear.first;
			vid.ear_val.second += temp_ear.second;
			vid.tilt_val += temp_tilt;

            // for(const auto& i : vid.points)
            // {
            //     x += shape.part(i).x();
			// 	y += shape.part(i).y();
            // }
            
            // x /= vid.points.size(); y /= vid.points.size();
			// x = mouse.max_x / 2 - x;

			for(const auto& i: vid.points)
				cv::circle(temp, cv::Point(shape.part(i).x(), shape.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);

			for(const auto& i: vid.right_eye)
				for(const auto& j : i)
					cv::circle(temp, cv::Point(shape.part(j).x(), shape.part(j).y()), 2, cv::Scalar(0, 0, 255), -1);

			for(const auto& i: vid.left_eye)
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

				if(abs(dx*mouse.max_x/190.0) >= 5 || abs(dy*mouse.max_y/107.0) >= 5)
				{
                    mouse.moveTo(x, y, true);
				}
				old_x = x; old_y = y;
			}
			cur_count++;
			if(vid.blinked)
			{
				vid.halt_blink += 1;
				if(vid.halt_blink == vid.halt_blink_for)
				{
					vid.halt_blink = 0; vid.blinked = false;
				}
			}
            vid.add_mouth_overlay(temp, shape);
        }
        auto end = now();
        auto diff = span(start, end);
        std::stringstream text;
        text << "FPS: "<<std::setprecision(3)<<std::fixed<<(1/diff);
        cv::putText(temp, text.str(), cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

        text = std::stringstream("");
		text << "Right Blinks: " << vid.blink_count.first;
		cv::putText(temp, text.str(), cv::Point(150, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

        text = std::stringstream("");
		text << "Left Blinks: " << vid.blink_count.second;
		cv::putText(temp, text.str(), cv::Point(150, 35), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 256), 2.0);

        cv::imshow("Demo", temp);
		cv::waitKey(1);
    }
}