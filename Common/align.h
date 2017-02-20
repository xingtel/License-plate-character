#pragma once

#include <vector>
/*
Align a(128, 128, 0.05);
Mat faceImageAlignCrop;
double timeA = getTickCount();
a.Alignment(faceAlign.gray, faceAlign.shape_current, faceImageAlignCrop);
timeA = (getTickCount() - timeA) / getTickFrequency();

imshow("faceImageAlignCrop", faceImageAlignCrop);
cout << "a.Alignment   " << timeA << endl;
*/

#include <string>

class Point
{
public:
	Point(int ix,int iy)
	{
		x = ix;
		y = iy;
	}
	int x;
	int y;
};
class Align
{
public:
	Align(){};
	//只实现了linear插值，当原人脸区域尺寸和截取尺寸差别不大时用双线性插值就可以。
	//align之后人脸图像的高度crop_height，宽度crop_width
	//padding是四周扩展的参数
	Align(int crop_height, int crop_width, float padding = 0.f, std::string type = "linear")
		:crop_height_(crop_height), crop_width_(crop_width)
		, padding_(padding), type_(type)
	{
		float std_points2[10] = {
			89.3095, 72.9025,
			169.3095, 72.9025,
			127.8949, 127.0441,
			96.8796, 184.8907,
			159.1065, 184.7601,
		};
		std_points = new float[10];
		for (int i = 0; i < 10; ++i) {
			std_points[i] = std_points2[i];
		}
		points_num_ = 5;
		for (int i = 0; i < points_num_; ++i) {
			std_points[i * 2] *= crop_height_ * 0.00390625;// / 256.0;
			std_points[i * 2 + 1] *= crop_width_ * 0.00390625;
		}
	}
	~Align(){ delete std_points; }
	//五点校准，feat_points十个数：
	//左眼中心x y，右眼中心x y，鼻子尖x y，左嘴角x y，右嘴角x y
	void Alignment(unsigned char * src_img, int srw, int srh,
		const float* const feat_points,
		unsigned char * dst_img);
	
	float Sampling(const unsigned char* const feat_map,
		int c, int H, int W, int C, float x, float y, float scale);

	int offset(int c, int h, int w)
	{
		return ((c * crop_height()) + h) *crop_width() + w;
	}

	//Alignment校准
	//输入原图像，和68点shape，返回截取后的脸dst_img
	//void Alignment(unsigned char * src_img, int srw, int srh,
	//	const std::vector<Vec2f> shape,
	//	unsigned char *  dst_img);
	
	//左上角 右上角 左下角 右下角
	std::vector<Point> getCornersPoints()
	{
		std::vector<Point> pts;
		pts.push_back(Point(corners[0], corners[1]));
		pts.push_back(Point(corners[2], corners[3]));
		pts.push_back(Point(corners[4], corners[5]));
		pts.push_back(Point(corners[6], corners[7]));
		return pts;
	}
	int* getCorners(){ return corners; }
	void set_height(int height) { crop_height_ = height; }

	void set_width(int width) { crop_width_ = width; }

	int crop_height() { return crop_height_; }

	int crop_width() { return crop_width_; }
	void set_padding(float padding){ padding_ = padding; }


private:
	int corners[8];
	int points_num_;
	float* std_points;
	int crop_height_;
	int crop_width_;
	float padding_;
	std::string type_;
};
