/*******************************************************************
 *  Copyright(c) 2016
 *  All rights reserved.
 *
 *  Name: Age and Gender estimation
 *  Lib: OpenCV2, Eigen3, caffe
 *  Date: 2016-2-16
 *  Author: Yang
 ******************************************************************/

#include <iostream>

#include "./FaceDetectionModule/face_detection.hpp"
#include "GenderEstimationModule\gender_estimation.hpp"
#include "./AgeEstimationModule/age_estimation.hpp"
#include "main_inter.h"
#include "ex.hpp"
int main_ageGender(char * szname) {
    
    cv::Mat image = cv::imread(szname);
	cv::Mat gray = cv::imread(szname,0);
	FaceDetector fd;
	Landmarker lder;
	fd.LoadXML("../deeplandmark/haarcascade_frontalface_alt2.xml");
	std::vector<cv::Rect> bboxes;
	fd.DetectFace(gray, bboxes);
	std::vector<BoundingBox> face_area;
	if (bboxes.size() > 0)
	{
	
		BoundingBox face1(bboxes[0].x, bboxes[0].y, bboxes[0].width, bboxes[0].height, 1.0);
		face_area.push_back(face1);

		std::vector<Gender> genders(gender_estimation(image, face_area));
		std::vector<Age> ages(age_estimation(image, face_area));

		auto i = 0;
		for (const auto& roi : face_area)
		{
			cv::Rect head(extend_face_to_whole_head(roi, image.rows, image.cols).transformToCVRect());

			cv::rectangle(image, head, cv::Scalar(255, 255, 255));
			cv::rectangle(image, bboxes[0], cv::Scalar(255, 0, 255));

			cv::putText(image, gender_listss[genders[i]], cv::Point(head.x, head.y + 10), 0, 0.7, cv::Scalar(0, 0, 255), 2);

			cv::putText(image, age_list[ages[i++]], cv::Point(head.x, head.y + head.height), 0, 0.7, cv::Scalar(255, 0, 0), 2);
		}

	}
   
    
//    auto print_genders = [](Gender a)
//    {
//        if (a == Gender::Man)
//        {
//            std::cout << "Man" << std::endl;
//        }
//        else if (a == Gender::Woman)
//        {
//            std::cout << "Woman" << std::endl;
//        }
//        else
//        {
//            std::cout << "Error!\n" << "Nan value is detected" << std::endl;
//        }
//    };
//    
//    auto print_ages = [](Age a)
//    {
//        switch (a)
//        {
//            case Age::R0_2: std::cout << "Age From 0 to 2" << std::endl;
//                break;
//            case Age::R4_6: std::cout << "Age From 4 to 6" << std::endl;
//                break;
//            case Age::R8_13: std::cout << "Age From 8 to 13" << std::endl;
//                break;
//            case Age::R15_20: std::cout << "Age From 15 to 20" << std::endl;
//                break;
//            case Age::R25_32: std::cout << "Age From 25 to 32" << std::endl;
//                break;
//            case Age::R38_43: std::cout << "Age From 38 to 43" << std::endl;
//                break;
//            case Age::R48_53: std::cout << "Age From 48 to 53" << std::endl;
//                break;
//            case Age::R60_: std::cout << "Age over 60" << std::endl;
//                break;
//            default: std::cout << "Error occurs!" << std::endl;
//        }
//    };
//    
//    std::for_each(genders.begin(), genders.end(), print_genders);
//    std::for_each(ages.begin(), ages.end(), print_ages);
    
  
    return 0;
}
