#ifndef LIGHTCNN_CROP
#define LIGHTCNN_CROP

#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>  
#include "opencv2/opencv.hpp"  

class lightCNN {
public:

	void light_crop(float face5Pts_x[5], float face5Pts_y[5], cv::Mat img, cv::Mat img_crop);

private:

};

#endif //LIGHTCNN_CROP