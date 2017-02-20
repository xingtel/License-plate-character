/*
功能  积分图快速定位车牌

作者:sunjunlishi

版权：厦门兴联科技.深圳

*/
#ifndef localContrastGlobal_hpp
#define localContrastGlobal_hpp


#include <opencv2/opencv.hpp>


void localContrastGlobal(cv::Mat dataMat, int maskSize);

 class localFangchaContrasMy
{
public:
	void CalContrasMy(cv::Mat dataMat, cv::Mat& dataOut,int maskSize);

	void BinaryCalContrasMy(cv::Mat dataMat, cv::Mat& dataOut, int maskSize);

	void GetPlateRects(cv::Mat inMat,std::vector<cv::Rect>& outRect);
	void GetPlateRectsNew(cv::Mat inMat, std::vector<cv::Rect>& outRect);
	//把小的线滤掉
	void FilterSmallThinline(cv::Mat datamat, cv::Mat& dataOut);
private:
	 int width;
	 int height;

	  int*sum;

	   int64 * squaresum;

	  int getBlockSum(int x, int y, int m, int n);
	  int64 getBlockSquareSum(int x, int y, int m, int n);
	  void ProcessJuanji(cv::Mat& dataMat);
	  void ProcessJuanjiSum(cv::Mat& dataMat);

};

#endif /* localContrastGlobal_hpp */
