#ifndef EASYPR_PLATE_HPP
#define EASYPR_PLATE_HPP
#include "platex\core\chars_identify_cnn.h"
#include "platex\core\chars_identify_ann.h"

#include "vcshow.h"
#include "LPKernelEx.h"


#include "../../../caffeEx/src/mini-caffe-example-master/main_inter.h"
int gICount = 0;
namespace platex {

	namespace demo {

		using namespace cv;
		using namespace std;

		int test_plate_locate() {
			cout << "test_plate_locate" << endl;

			const string file = "resources/image/plate_locate.jpg";

			cv::Mat src = imread(file);

			vector<cv::Mat> resultVec;
			CPlateLocate plate;
			//plate.setDebug(0);
			//plate.setLifemode(true);

			int result = plate.plateLocate(src, resultVec);
			if (result == 0) {
				size_t num = resultVec.size();
				for (size_t j = 0; j < num; j++) {
					cv::Mat resultMat = resultVec[j];
					imshow("plate_locate", resultMat);
					waitKey(0);
				}
				destroyWindow("plate_locate");
			}

			return result;
		}

		int test_plate_judge() {
			cout << "test_plate_judge" << endl;

			cv::Mat src = imread("resources/image/plate_judge.jpg");

			vector<cv::Mat> matVec;

			vector<cv::Mat> resultVec;

			CPlateLocate lo;
			lo.setDebug(1);
			lo.setLifemode(true);

			int resultLo = lo.plateLocate(src, matVec);

			if (0 != resultLo) return -1;

			cout << "plate_locate_img" << endl;
			size_t num = matVec.size();
			for (size_t j = 0; j < num; j++) {
				Mat resultMat = matVec[j];
				imshow("plate_judge", resultMat);
				waitKey(0);
			}
			destroyWindow("plate_judge");

			int resultJu = PlateJudge::instance()->plateJudge(matVec, resultVec);

			if (0 != resultJu) return -1;

			cout << "plate_judge_img" << endl;
			num = resultVec.size();
			for (size_t j = 0; j < num; j++) {
				Mat resultMat = resultVec[j];
				imshow("plate_judge", resultMat);
				waitKey(0);
			}
			destroyWindow("plate_judge");

			return resultJu;
		}


		Mat persTransform(Mat img, vector<Point2f> corners, vector<Point2f> corners_trans)
		{
			int img_height = img.rows;
			int img_width = img.cols;

			/*corners[0] = Point2f(0, 0);
			corners[1] = Point2f(img_width - 1, 0);
			corners[2] = Point2f(0, img_height - 1);
			corners[3] = Point2f(img_width - 1, img_height - 1);*/


			Mat transform = getPerspectiveTransform(corners, corners_trans);


			vector<Point2f> ponits, points_trans;
			for (int i = 0; i < img_height; i++) {
				for (int j = 0; j < img_width; j++) {
					ponits.push_back(Point2f(j, i));
				}
			}

			perspectiveTransform(ponits, points_trans, transform);
			Mat img_trans = Mat::zeros(img_height, img_width, CV_8UC3);
			int count = 0;
			for (int i = 0; i < img_height; i++) {
				uchar* p = img.ptr<uchar>(i);
				for (int j = 0; j < img_width; j++) {
					int y = points_trans[count].y;
					int x = points_trans[count].x;

					if (x > 0 && x < img_width && y > 0 && y < img_height)
					{
						uchar* t = img_trans.ptr<uchar>(y);


						t[x * 3] = p[j * 3];
						t[x * 3 + 1] = p[j * 3 + 1];
						t[x * 3 + 2] = p[j * 3 + 2];
					}

					count++;
				}
			}

			return img_trans;


		}



		void rectifyPlate(Mat& tmpSrc, std::vector<cv::Rect>& outRect)
		{
			Rect minR = outRect[0];
			cv::Rect maxR = outRect[outRect.size() - 1];
			//如果大于这个 旋转角度
			if (fabs(maxR.y - minR.y)*1.0 / fabs(maxR.x - minR.x) > 0.04)
			{
				float newW = sqrt((maxR.y - minR.y)*(maxR.y - minR.y)*1.0 + (maxR.x + maxR.width - minR.x)*(maxR.x + maxR.width - minR.x));



				vector<Point2f> corners(4);
				vector<Point2f> corners_trans(4);

				corners[0].x = minR.x;
				corners[0].y = minR.y;

				corners[1].x = newW + minR.x;// maxR.x + maxR.width;
				corners[1].y = maxR.y;

				corners[2].x = minR.x;
				corners[2].y = minR.y + minR.height;

				corners[3].x = newW + minR.x;// maxR.x + maxR.width;
				corners[3].y = maxR.y + minR.height;

				corners_trans[0].x = minR.x;
				corners_trans[0].y = minR.y;

				corners_trans[1].x = maxR.x + maxR.width;
				corners_trans[1].y = minR.y;

				corners_trans[2].x = minR.x;
				corners_trans[2].y = minR.y + minR.height;

				corners_trans[3].x = maxR.x + maxR.width;
				corners_trans[3].y = minR.y + minR.height;

				tmpSrc = persTransform(tmpSrc,
					corners, corners_trans);

				imshow("plate_detect", tmpSrc);
				cv::waitKey();
				//调整x位置和宽度
				float fR = (newW*1.0 / (maxR.x + maxR.width - minR.x));
				for (int i = 0; i < outRect.size(); ++i)
				{
					outRect[i].x = outRect[i].x*fR;
					outRect[i].width = outRect[i].width*fR;
				}



				//更改高度
				for (int i = 1; i < outRect.size(); ++i)
				{
					outRect[i].y = outRect[0].y;
				}

			}
		}

		//自定义排序函数
		//注意：本函数的参数的类型一定要与vector中元素的类型一致
		bool SortByM1(const cv::Rect &v1, const cv::Rect &v2)
		{
			return v1.x < v2.x;
		}

		cv::Rect GetBdRect(std::vector<Rect> outRect, Mat inMat)
		{
			cv::Rect maxR = outRect[outRect.size() - 1];
			cv::Rect minR = outRect[0];
			cv::Rect bdRect;

			bdRect.x = outRect[0].x - 2;
			bdRect.y = ((maxR.y < minR.y) ? maxR.y : minR.y) - 2;

			if (bdRect.x < 0) bdRect.x = 0;
			if (bdRect.y < 0) bdRect.y = 0;

			bdRect.width = maxR.x - minR.x + maxR.width + 6;

			if (bdRect.width + bdRect.x >= inMat.cols)
				bdRect.width = inMat.cols - bdRect.x;

			bdRect.height = (((maxR.height + maxR.y) > (minR.height + minR.y)) ? (maxR.height + maxR.y) : (minR.height + minR.y)) - bdRect.y + 4;
			if (bdRect.y + bdRect.height >= inMat.rows)
				bdRect.height = inMat.rows - bdRect.y;

			return bdRect;
		}


		cv::Mat matBk(300, 400, CV_8UC3);

		void cpy2matBk(Mat& tmpSrc, cv::Rect& bdRect, std::vector<cv::Rect>& outRect)
		{
			//高度统一到 48像素
			float fR = 48.0 / bdRect.height;
			int dstW = bdRect.width*fR;
			resize(tmpSrc, tmpSrc, Size(dstW, 48));

			bdRect.height = bdRect.height*fR;
			bdRect.width = bdRect.width*fR;
			bdRect.x = bdRect.x*fR;
			bdRect.y = bdRect.y*fR;
			//outRect进行处理
			for (int k = 0; k < outRect.size(); ++k)
			{
				outRect[k].x = outRect[k].x*fR;
				outRect[k].y = outRect[k].y*fR;

				outRect[k].width = outRect[k].width*fR;
				outRect[k].height = outRect[k].height*fR;

			}
			//把像素拷贝到MatBk中进行识别
			int s1 = outRect[1].x + outRect[1].width + 3;
			int s2 = outRect[2].x + outRect[2].width*0.35;
			int s3 = outRect[outRect.size() - 1].x;
			for (int i = 0; i < bdRect.height; ++i)
			{
				int step = (i + 50)*matBk.step[0];
				int jump = 50 * 3;

				int step1 = tmpSrc.step[0] * i;
				int jump1 = 0;
				for (int j = 0; j < s1; ++j)
				{
					matBk.data[step + jump] = tmpSrc.data[step1 + jump1];
					matBk.data[step + jump + 1] = tmpSrc.data[step1 + jump1 + 1];
					matBk.data[step + jump + 2] = tmpSrc.data[step1 + jump1 + 2];

					jump1 += 3;
					jump += 3;
				}
			}

			for (int i = 0; i < bdRect.height; ++i)
			{
				int step = (i + 50)*matBk.step[0];
				int jump = 50 * 3 + s1 * 3;

				int step1 = tmpSrc.step[0] * i;
				int jump1 = s2 * 3;
				for (int j = s2; j < s3; ++j)
				{
					matBk.data[step + jump] = tmpSrc.data[step1 + jump1];
					matBk.data[step + jump + 1] = tmpSrc.data[step1 + jump1 + 1];
					matBk.data[step + jump + 2] = tmpSrc.data[step1 + jump1 + 2];

					jump1 += 3;
					jump += 3;
				}
			}

		}

		CCharsSegment cseg;
		void GetOutRectMatE(Mat& dstMat, Mat& tmpMat, Rect rect, Color plateType)
		{
			rect.y -= 2;
			rect.height += 4;
			rect.x -= 2;
			rect.width += 4;

			if (rect.y < 0) rect.y = 0;
			if (rect.x < 0) rect.x = 0;

			if (rect.width + rect.x >= dstMat.cols)
			{
				rect.width = dstMat.cols - rect.x;
			}
			if (rect.y + rect.height >= dstMat.rows)
			{
				rect.height = dstMat.rows - rect.y;
			}

			cv::Mat sMat = dstMat(rect).clone();

			cseg.GetNewRoi(plateType, sMat, tmpMat);

		}
		void GetOutRectMat(Mat& dstMat, Mat& tmpMat, Rect rect)
		{
			rect.y -= 1;
			rect.height += 2;
			rect.x -= 2;
			rect.width += 4;

			if (rect.y < 0) rect.y = 0;
			if (rect.x < 0) rect.x = 0;

			if (rect.width + rect.x >= dstMat.cols)
			{
				rect.width = dstMat.cols - rect.x;
			}
			if (rect.y + rect.height >= dstMat.rows)
			{
				rect.height = dstMat.rows - rect.y;
			}

			cv::Mat sMat = dstMat(rect).clone();

			tmpMat = Mat(28, 28, CV_8UC1);
			tmpMat.setTo(0);

			int dstW = sMat.cols * 28 / sMat.rows;
			if (dstW < 28)
			{
				Mat dstM;
				cv::resize(sMat, dstM, Size(dstW, 28));
				for (int i = 0; i < dstM.rows; ++i)
				{
					int step = tmpMat.step[0] * i;
					int jump = (28 - dstW) / 2;

					int step1 = dstM.step[0] * i;
					for (int j = 0; j < dstM.cols; ++j)
					{
						tmpMat.data[step + jump] = dstM.data[step1 + j];
						jump++;
					}
				}
			}
			else
			{
				cv::resize(sMat, tmpMat, Size(28, 28));
			}
		}

		int indexP = 0;
		int recocaffe(cv::Mat tmpSrc, vector<cv::Rect> outRect, platex::Color color)
		{
			Mat dstMat;
			//缩放到指定宽高
			int dstH = 42;
			float fR = 42.0 / tmpSrc.rows;
			int dstW = tmpSrc.cols*fR;
			resize(tmpSrc, dstMat, Size(dstW, dstH));

			for (int k = 0; k < outRect.size(); ++k)
			{
				outRect[k].x = outRect[k].x*fR;
				outRect[k].y = outRect[k].y*fR;
				outRect[k].width = outRect[k].width*fR;
				outRect[k].height = outRect[k].height*fR;
			}

		

			vector<cv::Mat> rectMats;

			bool bRectOk = true;
			for (int k = 0; k < int(outRect.size()) - 1; ++k)
			{
				if ((outRect[k + 1].x - outRect[k].x - outRect[k].width) > outRect[0].width*0.9)
				{
					bRectOk = false;
					break;
				}
			}
			char szBuf[256];
			sprintf(szBuf, "%d", outRect.size());
			bool bOk = false;
			if (1)
			{
				cseg.charsSegment9(dstMat, rectMats, color);
				//为了保证后面逻辑一直，2插入空mat
				if (rectMats.size() > 0)
				{
					Mat matNull;
					rectMats.insert(rectMats.begin() + 2, matNull);
				}

				if (color == GREEN)
				{
					if (rectMats.size() >= 9)
					{
						bOk = true;
						gICount++;
					}
					
				}
				else if (rectMats.size() >= 8)
				{
					bOk = true;
					gICount++;
				}
			}
			if (outRect.size() == 0)
			

			if (!bOk && outRect.size() > 0)
			{
				cvtColor(dstMat, dstMat, CV_BGR2GRAY);
				//spatial_ostu(dstMat, 8, 3, color);
				rectMats.clear();
				//提取字符进行二值化识别
				for (int k = 0; k < outRect.size(); ++k)
				{

					Mat tmpMat;
					//GetOutRectMat(dstMat, tmpMat, outRect[k]);
					GetOutRectMatE(dstMat, tmpMat, outRect[k], color);
					rectMats.push_back(tmpMat);

				}
				if (color == GREEN)
				{
					bOk = ((rectMats.size() >= 8));
					gICount++;
				}
				else
				{
					bOk = ((rectMats.size() >= 7));
					gICount++;
				}
			}

			int iCount = (rectMats.size() > 9) ? 9 : rectMats.size();
			if (color != GREEN)
			{
				iCount = (rectMats.size() > 8) ? 8 : rectMats.size();
			}

			char szScore[128];
			std::string plateLicense;
			std::string strscore;
			float sumscore = 0.0;
			if (bOk)
			{
				for (int k = 0; k < iCount; ++k)
				{
					Mat tmpMat = rectMats[k];
					string outStr;
					char szBuf[256];
					sprintf(szBuf, "D:/save%d.jpg", indexP);
					//imwrite(szBuf, tmpMat);
					indexP++;
					float maxf = -10;
					
					if (k == 0)
					{
						maxf = plateX_recoMat(tmpMat, outStr, (k == 0) ? true : false);
						sprintf(szScore,"%.1f ", maxf);
						strscore += szScore;
						sumscore += maxf;
					}
					else if (k == 1)
					{
						maxf = plateX_recoCh(tmpMat, outStr, false);
						maxf = plateX_recoMat(tmpMat, outStr, (k == 0) ? true : false);
						sprintf(szScore, "%.1f ", maxf);
						strscore += szScore;
						sumscore += maxf;
					}
					else if (k == 2)
					{
						 plateLicense.append(" ");
					}
					else
					{
						maxf = plateX_recoMat(tmpMat, outStr, false);
						maxf = plateX_recoMat(tmpMat, outStr, (k == 0) ? true : false);
						sprintf(szScore, "%.1f ", maxf);
						strscore += szScore;
						sumscore += maxf;
					}

					plateLicense.append(outStr);

					int iTop = 0; if (outRect.size() == 0) iTop = 750;
					DrawThePicPoint(gShowWnd, tmpMat, 30+ gICount * 210 + 22 * k, 805 - iTop);
				}
				
				sprintf(szScore, " :%.1f ", sumscore);
				strscore += szScore;

				string colorInfo;
				COLORREF bkColor = RGB(0, 0, 190);
				COLORREF textColor = RGB(255, 255, 255);
				if (color == GREEN) { bkColor = RGB(0, 180, 0); textColor = RGB(0, 0, 0); }
				if (color == YELLOW) { bkColor = RGB(200, 160, 0); textColor = RGB(0, 0, 0); }
				if (color == BLACK) { bkColor = RGB(20, 20, 20); textColor = RGB(255, 255, 255); }

				int iTop = 0; if (outRect.size() == 0) iTop = 750;
				DrawMyTextBK((colorInfo + plateLicense).c_str(),
					30 + 210 * gICount, 828 - iTop,
					(CWnd*)gShowWnd, bkColor, 28, textColor);

				DrawMyTextBK(strscore.c_str(),
					30 + 210 * gICount, 828 - iTop+36,
					(CWnd*)gShowWnd, bkColor, 16, textColor);

			}
			
			return bOk;
		}

		int test_plate_detect(char * fileName, int type)
		{
			cv::Mat src = imread(fileName);
			if (src.cols > 1400)
			{
				int dstW = 1000;
				int dstH = dstW*src.rows/src.cols;
				resize(src, src, Size(dstW, dstH));

			}



			vector<CPlate> resultVec;
			CPlateDetect pd;
			pd.setDetectType(type);
			pd.setPDLifemode(true);

			int result = pd.plateDetect(src, resultVec);
			if (result == 0) {
				size_t num = resultVec.size();
				gICount = -1;
				for (size_t j = 0; j < num; j++)
				{

					CPlate resultMat = resultVec[j];
					std::vector<CCharacter>  charRects = resultMat.getCopyOfMserCharacters();

					cv::Mat plateMat = resultMat.getPlateMat();
					std::vector<cv::Rect> outRect;
					for (int k = 0; k < charRects.size(); ++k)
					{
						outRect.push_back(charRects[k].getCharacterPos());
					}


					int iCount = outRect.size();
					int index = 0;
					for (int k = 0; k < iCount; ++k)
					{
						if (index < outRect.size())
						{
							for (int j = outRect.size() - 1; j > index; j--)
							{
								if (fabs(outRect[index].x - outRect[j].x) <= 3)
								{
									outRect.erase(outRect.begin() + j);
								}
							}
						}
						index++;
					}
					if (outRect.size() <= 4 || outRect.size() > 14)
					{
						continue;
					}

					std::sort(outRect.begin(), outRect.end(), SortByM1);




					cv::Rect tbdRect = GetBdRect(outRect, plateMat);
					if (platex::IsGreenColor(plateMat(tbdRect).clone()))
					{
						resultMat.setPlateColor(Color::GREEN);
					}
					else if (platex::IsBlackColor(plateMat(tbdRect).clone()))
					{
						resultMat.setPlateColor(Color::BLACK);
					}
						
	

					GetLeftRightPos(plateMat, outRect, resultMat.getPlateColor());

					


					//抠出矩形
					cv::Rect bdRect = GetBdRect(outRect, plateMat);
					Mat tmpSrcRect = plateMat(bdRect);
					//outRect进行处理
					for (int k = 0; k < outRect.size(); ++k)
					{
						outRect[k].x = outRect[k].x - bdRect.x;
						outRect[k].y = outRect[k].y - bdRect.y;
					}


					if (recocaffe(tmpSrcRect, outRect, resultMat.getPlateColor()) == 1)
					{
						Mat showTmp = tmpSrcRect.clone();
						for (int k = 0; k < outRect.size(); ++k)
						{
							rectangle(showTmp, outRect[k], Scalar(255, 0, 0), 1);
						}
						resize(showTmp, showTmp, Size(140, 36));
						DrawThePicPoint(gShowWnd, showTmp, gICount * 210 + 20, 765);
					}
	
				}

			}

			return result;
		}

		int test_plate_recognize(char * filename, int param) {
			cout << "test_plate_recognize" << endl;

			Mat src = imread(filename);
			if (src.cols > 1400)
			{
				int dstW = 1000;
				int dstH = dstW*src.rows / src.cols;
				resize(src, src, Size(dstW, dstH));

			}

			CPlateRecognize pr;
			pr.setLifemode(true);
			pr.setDebug(false);
			pr.setMaxPlates(4);
			//pr.setDetectType(PR_DETECT_COLOR | PR_DETECT_SOBEL);
			pr.setDetectType(param);

			//vector<string> plateVec;
			vector<CPlate> plateVec;
			string colorInfo;
			COLORREF bkColor = RGB(0, 0, 190);

			int result = pr.plateRecognize(src, plateVec);
			gICount = -1;
			for (int k = 0; k < plateVec.size(); ++k)
			{
				vector<Rect> outRect; 
				recocaffe(plateVec[k].getPlateMat(), outRect,  plateVec[k].getPlateColor());
			}

			return 1;

		}
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
