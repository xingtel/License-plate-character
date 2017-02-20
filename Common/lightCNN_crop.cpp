

#include "lightCNN_crop.h"
cv::Mat getRotationMatrix2DM(cv::Point2f center, double angle, double scale)
{
	angle *= CV_PI / 180;
	double alpha = cos(angle)*scale;
	double beta = sin(angle)*scale;

	cv::Mat M(2, 3, CV_64F);
	double* m = M.ptr<double>();

	m[0] = alpha;
	m[1] = beta;
	m[2] = (1 - alpha)*center.x - beta*center.y;
	m[3] = -beta;
	m[4] = alpha;
	m[5] = beta*center.x + (1 - alpha)*center.y;

	return M;
}
void lightCNN::light_crop(float face5Pts_x[5], float face5Pts_y[5], cv::Mat img, cv::Mat img_crop) {

	float ang_tan = (face5Pts_y[0] - face5Pts_y[1]) / (face5Pts_x[0] - face5Pts_x[1]);
	float angle = atan(ang_tan)*180.0 / CV_PI;

	int imgh = img.rows;
	int imgw = img.cols;

	float eye_center_x = (face5Pts_x[0] + face5Pts_x[1]) / 2;
	float eye_center_y = (face5Pts_y[0] + face5Pts_y[1]) / 2;
	
	float mouth_center_x = (face5Pts_x[3] + face5Pts_x[4]) / 2;
	float mouth_center_y = (face5Pts_y[3] + face5Pts_y[4]) / 2;
	
	float eye_mouth_distance = mouth_center_y - eye_center_y;
	//float eye_mouth_distance = sqrt(pow((mouth_center_y - eye_center_y), 2) + pow((mouth_center_x - eye_center_x), 2));
	int ec_mc_y = 48;
	float resize_scale = ec_mc_y / eye_mouth_distance;
	//std::cout << resize_scale << std::endl;
	cv::Mat rot_mat = getRotationMatrix2DM({ eye_center_x, eye_center_y }, angle, resize_scale);

	int ec_y = 40;
	double ex = 128 * 0.5f - eye_center_x;
	double ey = 40.0f - eye_center_y;

	rot_mat.at<double>(0, 2) += ex;
	rot_mat.at<double>(1, 2) += ey;

	//	cv::Mat warped(cv::Size(128, 128), img.type());
	cv::warpAffine(img, img_crop, rot_mat, img_crop.size());
}