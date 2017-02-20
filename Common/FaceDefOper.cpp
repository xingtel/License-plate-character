#include "FaceDefOper.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\imgcodecs.hpp>
#include <Common\gLobalInfo.h>
#include<ostream>
#include <fstream>
using namespace cv;
Mat DrawObj(Faceinfo_S& info,int nSex)
{
	
	Mat image ;
	Mat imgsrc;
	if(info.matPic.empty()) 	imgsrc = imread(info.picname.c_str());
	else imgsrc = info.matPic.clone();
	if (!imgsrc.empty())
	{
		int wPic = imgsrc.cols;
		int hPic = imgsrc.rows;

		int disEye = (info.pts5[1].x - info.pts5[0].x)*1.1;
		int posy = info.pts5[0].y - disEye*0.7;
		int posx = info.pts5[0].x - disEye *0.5;

		int w = disEye * 2;
		int h = w * 220 / 200;

		cv::Rect rect;
		rect.x = posx; rect.y = posy; 
		if (rect.x < 0) rect.x = 0;
		if (rect.y < 0) rect.y = 0;
		
		rect.width = w; rect.height = h;
		if (rect.width + rect.x >= wPic) rect.width = wPic - rect.x;
		if (rect.height + rect.y > hPic) rect.height = hPic - rect.y;
	
		Mat tmpImg = imgsrc(rect).clone();
		
		int dstW = 200; 
		int dstH = dstW*tmpImg.rows/ tmpImg.cols;

		resize(tmpImg, image,cv::Size(dstW,dstH));
		
	}

	return image;
}

void savePtsFile(const char * idName, std::vector<cv::Point>& points)
{
	
	
	// TODO: 在此添加控件通知处理程序代码
	std::ofstream locations((const char*)idName, ios_base::out);
	if (!locations.is_open())
	{
		return;
	}
	string str = "version: 1";
	locations << str << "\n";
	str = "n_points:  ";
	locations << str << points.size()<< "\n";
	locations << "{" << "\n";

	for (int i = 0; i<points.size(); i++)
	{
		locations << points[i].x << " ";
		locations << points[i].y << "\n";
	}
	
	locations << "}";
	locations.close();



}
void readPtsFile(const char *  IDname, vector<cv::Point>& points)
{

	ifstream locations(IDname, ios_base::in);
	if (locations.is_open())
	{
		string line;
		// The main file contains the references to other files
		while (!locations.eof())
		{
			getline(locations, line);

			if (line.compare("{") == 0)
			{
				for (int i = 0; i<points.size(); i++)
				{
					locations >> points[i].x;
					locations >> points[i].y;
				}
			}
		}
	}

}
