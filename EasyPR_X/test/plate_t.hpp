#ifndef EASYPR_PLATE_HPP
#define EASYPR_PLATE_HPP

#include "platex\core\chars_identify_cnn.h"
#include "platex\core\chars_identify_ann.h"

#include "vcshow.h"
#include "../caffeEx/main_inter.h"

struct plate_info
{
	string color;
	string license;
	std::vector<float> fconfidence;
};

namespace platex {

	namespace demo {

		using namespace cv;
		using namespace std;

		
		Mat persTransform(Mat img, vector<Point2f> corners, vector<Point2f> corners_trans);

		void rectifyPlate(Mat& tmpSrc, std::vector<cv::Rect>& outRect);
		
		//自定义排序函数
		//注意：本函数的参数的类型一定要与vector中元素的类型一致
		bool SortByM1(const cv::Rect &v1, const cv::Rect &v2);
		

		cv::Rect GetBdRect(std::vector<Rect> outRect, Mat inMat);
		


		

		
		void GetOutRectMatE(Mat& dstMat, Mat& tmpMat, Rect rect, Color plateType);

		void GetOutRectMat(Mat& dstMat, Mat& tmpMat, Rect rect);

	
		int recocaffe(cv::Mat tmpSrc, vector<cv::Rect> outRect, platex::Color color, plate_info& pritem);

		int test_plate_detect(char * fileName, int type, vector<plate_info>& prInfos);

		int test_plate_recognize(char * filename, int param, vector<plate_info>& prInfos);
	}
}

#endif  // EASYPR_PLATE_HPP
//
//int test_plate_detect(char * fileName, int type)
//{
//
//	cout << "test_plate_detect" << endl;
//	cv::Mat src = imread(fileName);
//
//	vector<CPlate> resultVec;
//	CPlateDetect pd;
//	pd.setDetectType(type);
//	pd.setPDLifemode(true);
//
//	int result = pd.plateDetect(src, resultVec);
//	if (result == 0) {
//		size_t num = resultVec.size();
//		for (size_t j = 0; j < num; j++)
//		{
//			matBk.setTo(255);
//
//			CPlate resultMat = resultVec[j];
//			std::vector<CCharacter>  charRects = resultMat.getCopyOfMserCharacters();
//			cv::Mat plateMat = resultMat.getPlateMat();
//			std::vector<cv::Rect> outRect;
//			for (int k = 0; k < charRects.size(); ++k)
//			{
//				outRect.push_back(charRects[k].getCharacterPos());
//			}
//			int iCount = outRect.size();
//			int index = 0;
//			for (int k = 0; k < iCount; ++k)
//			{
//				if (index < outRect.size())
//				{
//					for (int j = outRect.size() - 1; j > index; j--)
//					{
//						if (fabs(outRect[index].x - outRect[j].x) <= 3)
//						{
//							outRect.erase(outRect.begin() + j);
//						}
//					}
//				}
//				index++;
//			}
//
//			std::sort(outRect.begin(), outRect.end(), SortByM1);
//			if (outRect.size() <= 3) continue;
//			//有空格的，产生框
//			for (int i = 0; i < 3; ++i)
//			{
//				Rect ri = outRect[i]; Rect ri1 = outRect[i + 1];
//				if (ri1.x - ri.x - ri.width >= (ri1.width + ri.width)*0.3)
//				{
//					outRect.insert(outRect.begin() + i + 1, cv::Rect(ri.x + ri.width, ri.y,
//						ri1.x - ri.x - ri.width, ri.height));
//				}
//			}
//
//			cv::Rect tbdRect = GetBdRect(outRect, plateMat);
//			if (platex::IsGreenColor(plateMat(tbdRect)))
//			{
//				resultMat.setPlateColor(Color::GREEN);
//			}
//
//			//把汉字 也圈住
//			if (outRect.size() == 8 && fabs(outRect[0].x + outRect[0].width - outRect[1].x) <= 3
//				&& resultMat.getPlateColor() == Color::YELLOW)
//			{
//				float f1 = (outRect[outRect.size() - 1].x - outRect[0].x + outRect[outRect.size() - 1].width)*1.0 / outRect[0].height;
//				//如果宽度和高度比不满足条件，则添加一个新的
//				Rect ri = outRect[1]; Rect ri1 = outRect[2];
//				if (ri1.x - ri.x - ri.width <= (ri1.width + ri.width)*0.3 && f1 < 4.6)
//				{
//					cv::Rect tRect = outRect[0];
//					outRect.insert(outRect.begin(),
//						cv::Rect(tRect.x - tRect.width*1.4, tRect.y, tRect.width*1.4, tRect.height));
//				}
//				else
//				{
//
//				}
//
//			}
//

//			//抠出矩形
//			cv::Rect bdRect = GetBdRect(outRect, plateMat);
//
//			Mat tmpSrcRect = plateMat(bdRect);
//
//			//outRect进行处理
//			for (int k = 0; k < outRect.size(); ++k)
//			{
//				outRect[k].x = outRect[k].x - bdRect.x;
//				outRect[k].y = outRect[k].y - bdRect.y;
//			}
//
//			recocaffe(tmpSrcRect, outRect, j, resultMat.getPlateColor());
//
//			////把车牌拷贝到纯白区域进行识别
//			//cpy2matBk(tmpSrcRect,bdRect,outRect);
//			//TH_PlateResult thPlate;
//			//int nRecoNum = 0;
//			//TH_RECT thR;
//			//thR.right = 0;
//			//thR.bottom = 0;
//			//thR.left = 0;
//			//thR.top = 0;
//			//LPR_MemeryPictureEx(matBk.data, matBk.cols, matBk.rows, &thPlate, nRecoNum, &thR);
//
//
//			for (int k = 0; k < outRect.size(); ++k)
//			{
//				rectangle(tmpSrcRect, outRect[k], Scalar(255, 0, 0), 1);
//			}
//
//			DrawThePicPoint(gShowWnd, tmpSrcRect, j * 230, 700);
//
//			// DrawThePicPoint(gShowWnd, matBk, j * 230, 760);
//
//			//DrawMyText(thPlate.license, 0 + j * 230, 760, (CWnd*)gShowWnd, 40,RGB(255,0,0));
//		}
//
//	}
//
//	return result;
//}
////把车牌拷贝到纯白区域进行识别
//cpy2matBk(tmpSrcRect,bdRect,outRect);
//TH_PlateResult thPlate;
//int nRecoNum = 0;
//TH_RECT thR;
//thR.right = 0;
//thR.bottom = 0;
//thR.left = 0;
//thR.top = 0;
//LPR_MemeryPictureEx(matBk.data, matBk.cols, matBk.rows, &thPlate, nRecoNum, &thR);
//cv::Mat matBk(300, 400, CV_8UC3);
//
//void cpy2matBk(Mat& tmpSrc, cv::Rect& bdRect, std::vector<cv::Rect>& outRect)
//{
//	//高度统一到 48像素
//	float fR = 48.0 / bdRect.height;
//	int dstW = bdRect.width*fR;
//	resize(tmpSrc, tmpSrc, Size(dstW, 48));
//
//	bdRect.height = bdRect.height*fR;
//	bdRect.width = bdRect.width*fR;
//	bdRect.x = bdRect.x*fR;
//	bdRect.y = bdRect.y*fR;
//	//outRect进行处理
//	for (int k = 0; k < outRect.size(); ++k)
//	{
//		outRect[k].x = outRect[k].x*fR;
//		outRect[k].y = outRect[k].y*fR;
//
//		outRect[k].width = outRect[k].width*fR;
//		outRect[k].height = outRect[k].height*fR;
//
//	}
//	//把像素拷贝到MatBk中进行识别
//	int s1 = outRect[1].x + outRect[1].width + 3;
//	int s2 = outRect[2].x + outRect[2].width*0.35;
//	int s3 = outRect[outRect.size() - 1].x;
//	for (int i = 0; i < bdRect.height; ++i)
//	{
//		int step = (i + 50)*matBk.step[0];
//		int jump = 50 * 3;
//
//		int step1 = tmpSrc.step[0] * i;
//		int jump1 = 0;
//		for (int j = 0; j < s1; ++j)
//		{
//			matBk.data[step + jump] = tmpSrc.data[step1 + jump1];
//			matBk.data[step + jump + 1] = tmpSrc.data[step1 + jump1 + 1];
//			matBk.data[step + jump + 2] = tmpSrc.data[step1 + jump1 + 2];
//
//			jump1 += 3;
//			jump += 3;
//		}
//	}
//
//	for (int i = 0; i < bdRect.height; ++i)
//	{
//		int step = (i + 50)*matBk.step[0];
//		int jump = 50 * 3 + s1 * 3;
//
//		int step1 = tmpSrc.step[0] * i;
//		int jump1 = s2 * 3;
//		for (int j = s2; j < s3; ++j)
//		{
//			matBk.data[step + jump] = tmpSrc.data[step1 + jump1];
//			matBk.data[step + jump + 1] = tmpSrc.data[step1 + jump1 + 1];
//			matBk.data[step + jump + 2] = tmpSrc.data[step1 + jump1 + 2];
//
//			jump1 += 3;
//			jump += 3;
//		}
//	}
//
//}