#include <cassert>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <caffe/caffe.hpp>
#include "main_compare.h"
#include"opencv2/ml.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/flann.hpp"
using namespace cv;
using namespace std;
using namespace caffe;
using namespace cv::ml;
using namespace cv;
using namespace std;
using namespace caffe;

caffe::Net<float> *comparenet = NULL;


void loaddata(Net<float> * net, std::string image_path1, std::string image_path2) {
	Blob<float>* input_layer = net->input_blobs()[0];
	int width, height;
	width = input_layer->width();
	height = input_layer->height();
	int size = width*height;
	cv::Mat image1 = cv::imread(image_path1, 0);
	cv::Mat image2 = cv::imread(image_path2, 0);

	cv::Mat image_resized1;
	cv::Mat image_resized2;
	cv::resize(image1, image_resized1, cv::Size(height, width));
	cv::resize(image2, image_resized2, cv::Size(height, width));
	float* input_data = input_layer->mutable_cpu_data();
	int temp, idx;
	for (int i = 0; i < height; ++i) {
		uchar* pdata1 = image_resized1.ptr<uchar>(i);
		uchar* pdata2 = image_resized2.ptr<uchar>(i);
		for (int j = 0; j < width; ++j) {
			temp = j;
			idx = i*width + j;
			input_data[idx] = (pdata1[temp + 0] / 127.5) - 1;
			input_data[idx + size] = (pdata2[temp + 0] / 127.5) - 1;
			
		}
	}
	//cv::imshow("image",image_resized);
}



void loaddata(Net<float> * net, cv::Mat image1, cv::Mat image2) {
	Blob<float>* input_layer = net->input_blobs()[0];
	int width, height;
	width = input_layer->width();
	height = input_layer->height();
	int size = width*height;

	cv::Mat image_resized1;
	cv::Mat image_resized2;
	cv::resize(image1, image_resized1, cv::Size(height, width));
	cv::resize(image2, image_resized2, cv::Size(height, width));
	float* input_data = input_layer->mutable_cpu_data();
	int temp, idx;
	for (int i = 0; i < height; ++i) {
		uchar* pdata1 = image_resized1.ptr<uchar>(i);
		uchar* pdata2 = image_resized2.ptr<uchar>(i);
		for (int j = 0; j < width; ++j) {
			temp = j;
			idx = i*width + j;
			input_data[idx] = (pdata1[temp + 0] / 127.5) - 1;
			input_data[idx + size] = (pdata2[temp + 0] / 127.5) - 1;

		}
	}
}


float DoTestCompare(Mat file1, Mat file2)
{
	if (file1.empty() || file2.empty()) return 0;


	loaddata(comparenet, file1, file2);

	int64 t11 = getTickCount();
	comparenet->Forward();
	int64 t21 = getTickCount();

	char szBuf[512];
	sprintf(szBuf, "%I64d", t21 - t11);



	//std::cout<<"to process finish time = "<< t.elapsed() <<std::endl;
	Blob<float>* output_layer = comparenet->output_blobs()[0];
	const float* begin = output_layer->cpu_data();
	const float* end = begin + output_layer->channels();
	std::vector<float> result(begin, end);

	if (result.size() > 0) return result[0];
	return 0;
}



Mat GetRoundPlate(Mat  _mat1, S_MY_RECT * rect1)
{
	cv::Rect rectP1;
	rectP1.x = ((S_MY_RECT*)rect1)->x;
	rectP1.y = ((S_MY_RECT*)rect1)->y;
	rectP1.width = ((S_MY_RECT*)rect1)->w;
	rectP1.height = ((S_MY_RECT*)rect1)->h;

	cv::rectangle(_mat1, rectP1, Scalar(255, 0, 0), 1, cv::LINE_AA);
	int rectW = rectP1.width;
	int rectH = rectP1.height;

	cv::Rect bdRect;
	bdRect.x = rectP1.x - rectW*0.8;
	if (bdRect.x < 0) bdRect.x = 0;
	bdRect.y = rectP1.y - rectH*3.5;
	if (bdRect.y < 0) bdRect.y = 0;

	bdRect.width = rectW*2.6;
	if (bdRect.width + bdRect.x >= _mat1.cols) bdRect.width = _mat1.cols - bdRect.x;

	bdRect.height = 5.6*rectH;
	if (bdRect.height + bdRect.y >= _mat1.rows) bdRect.height = _mat1.rows - bdRect.y;

	_mat1 = _mat1(bdRect);
	return _mat1.clone();
}

void caffe_comareinit()
{
	if (comparenet == NULL)
	{
		comparenet = new Net<float>("./model_siam/notredame_deploy.prototxt", caffe::TEST);
		Caffe::set_mode(Caffe::CPU);
		comparenet->CopyTrainedLayersFrom("./model_siam/notredame.caffemodel");
	}

}

#ifdef USE_CV_COMPARE
Ptr<SURF> surf;      //创建方式和2中的不一样
bool gSurfCv = false;
float  main_compareCv2(char* data1pathname, S_MY_RECT * rect1,
	char * data2pathname, S_MY_RECT * rect2)
{
	cv::Mat outShow;

	Mat mat1 = imread(data1pathname);
	Mat mat2 = imread(data2pathname);

	Mat _mat1 = GetRoundPlate(mat1, rect1);
	Mat _mat2 = GetRoundPlate(mat2, rect2);


	if (!gSurfCv)
	{
		surf = SURF::create(500,3,2);
		gSurfCv = true;
	}


	BFMatcher matcher;
	Mat c, d;

	vector<KeyPoint>key1, key2;
	surf->detectAndCompute(_mat1, Mat(), key1, c);
	surf->detectAndCompute(_mat2, Mat(), key2, d);
	vector<DMatch> matches;
	matcher.match(c, d, matches);       //匹配
										//sort(matches.begin(), matches.end());  //筛选匹配点

	vector< DMatch > t_matches;
	float yuzhiF = 0.12;
	for (int k = 0; k < matches.size(); ++k)
	{
		if (matches[k].distance < yuzhiF)
		{
			t_matches.push_back(matches[k]);
		}
		else
		{
			continue;
		}
	}


	if (t_matches.size() > 0)
	{
		Mat outimg;
		drawMatches(_mat1, key1, _mat2, key2, t_matches, outimg, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点
		char szBuf[512];
		sprintf(szBuf, "%d", t_matches.size());
		cv::putText(outimg, szBuf, cv::Point(20, 20), 0, 0.5, Scalar(220), 2, cv::LINE_AA);

		outShow = outimg;
		return t_matches.size();
	}
	else
	{
		return -1;
	}

}
float main_compareCv(char* data1, int w1, int h1, S_MY_RECT * rect1,
	char * data2, int w2, int h2, S_MY_RECT * rect2)
{
	cv::Mat outShow;
	Mat mat1 = Mat(h1, w1, CV_8UC3);
	memcpy(mat1.data, data1, h1*w1 * 3);
	Mat mat2 = Mat(h2, w2, CV_8UC3);
	memcpy(mat2.data, data2, h2*w2 * 3);

	Mat _mat1 = GetRoundPlate(mat1, rect1);
	Mat _mat2 = GetRoundPlate(mat2, rect2);


	if (!gSurfCv)
	{
		surf = SURF::create(500,3,2);
		gSurfCv = true;
	}


	BFMatcher matcher;
	Mat c, d;

	vector<KeyPoint>key1, key2;
	surf->detectAndCompute(_mat1, Mat(), key1, c);
	surf->detectAndCompute(_mat2, Mat(), key2, d);
	vector<DMatch> matches;
	matcher.match(c, d, matches);       //匹配
										//sort(matches.begin(), matches.end());  //筛选匹配点

	vector< DMatch > t_matches;
	float yuzhiF = 0.15;
	for (int k = 0; k < matches.size(); ++k)
	{
		if (matches[k].distance < yuzhiF)
		{
			t_matches.push_back(matches[k]);
		}
		else
		{
			continue;
		}
	}

	if (t_matches.empty())
	{
		_mat1.release();
		_mat2.release();

		return 0;
	}


	Mat outimg;
	drawMatches(_mat1, key1, _mat2, key2, t_matches, outimg, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点


	if (t_matches.size() > 0)
	{

		char szBuf[512];
		sprintf(szBuf, "%d", t_matches.size());
		cv::putText(outimg, szBuf, cv::Point(20, 20), 0, 0.5, Scalar(220), 2, cv::LINE_AA);

		outShow = outimg;
		return t_matches.size();
	}

	return -1;
}
#endif

Mat GetPosRectMat(Mat mat, S_MY_RECT * Myrect, int yiX, int yiY)
{
	Mat tmpMat;
	cv::Rect rect;
	rect.x = Myrect->x + yiX;
	if (rect.x < 0) rect.x = 0;
	rect.y = Myrect->y + yiY;
	if (rect.y < 0) rect.y = 0;

	rect.width = Myrect->w;
	rect.height = Myrect->h*2;

	if (rect.x + rect.width >= mat.cols) rect.width = mat.cols - rect.x;
	if (rect.y + rect.height >= mat.rows) rect.height = mat.rows - rect.y;

	tmpMat = mat(rect).clone();

	return tmpMat;
}
float  main_compareCaffe2(char* datapathname1, S_MY_RECT * rect1, char * datapathname2, S_MY_RECT *rect2)
{
	caffe_comareinit();

	float sumScore = 0.0;
	//左右2个区域 进行比较，总共比较4个区域
	Mat mat1 = imread(datapathname1);
	Mat mat2 = imread(datapathname2);
	//
	S_MY_RECT * rectMy1 = (S_MY_RECT*)rect1;
	S_MY_RECT * rectMy2 = (S_MY_RECT*)rect2;

	Mat tmp1 = GetPosRectMat(mat1, rectMy1, -rectMy1->w / 2, 0);
	Mat tmp2 = GetPosRectMat(mat2, rectMy2, -rectMy2->w / 2, 0);
	sumScore += DoTestCompare(tmp1, tmp2);



	tmp1 = GetPosRectMat(mat1, rectMy1, -rectMy1->w / 2, -rectMy1->h * 2);
	tmp2 = GetPosRectMat(mat2, rectMy2, -rectMy2->w / 2, -rectMy1->h * 2);
	sumScore += DoTestCompare(tmp1, tmp2);

	//右边
	tmp1 = GetPosRectMat(mat1, rectMy1, rectMy1->w / 2, 0);
	tmp2 = GetPosRectMat(mat2, rectMy2, rectMy2->w / 2, 0);
	sumScore += DoTestCompare(tmp1, tmp2);



	tmp1 = GetPosRectMat(mat1, rectMy1, rectMy1->w / 2, -rectMy1->h * 2);
	tmp2 = GetPosRectMat(mat2, rectMy2, rectMy2->w / 2, -rectMy1->h * 2);
	sumScore += DoTestCompare(tmp1, tmp2);


	return sumScore;
}
float main_compareCaffe(char* data1, int w1, int h1, S_MY_RECT * rect1,
	char * data2, int w2, int h2, S_MY_RECT * rect2)
{
	caffe_comareinit();

	float sumScore = 0.0;
	//左右4个区域 进行比较，总共比较8个区域
	Mat mat1 = Mat(h1, w1, CV_8UC3);
	memcpy(mat1.data, data1, h1*w1 * 3);
	Mat mat2 = Mat(h2, w2, CV_8UC3);
	memcpy(mat2.data, data2, h2*w2 * 3);

	//
	S_MY_RECT * rectMy1 = (S_MY_RECT*)rect1;
	S_MY_RECT * rectMy2 = (S_MY_RECT*)rect2;

	Mat tmp1 = GetPosRectMat(mat1, rectMy1, -rectMy1->w / 2, 0);
	Mat tmp2 = GetPosRectMat(mat2, rectMy2, -rectMy2->w / 2, 0);
	sumScore += DoTestCompare(tmp1, tmp2);

	
	tmp1 = GetPosRectMat(mat1, rectMy1, -rectMy1->w / 2, -rectMy1->h * 2);
	tmp2 = GetPosRectMat(mat2, rectMy2, -rectMy2->w / 2, -rectMy1->h * 2);
	sumScore += DoTestCompare(tmp1, tmp2);

	//右边
	tmp1 = GetPosRectMat(mat1, rectMy1, rectMy1->w / 2, 0);
	tmp2 = GetPosRectMat(mat2, rectMy2, rectMy2->w / 2, 0);
	sumScore += DoTestCompare(tmp1, tmp2);


	tmp1 = GetPosRectMat(mat1, rectMy1, rectMy1->w / 2, -rectMy1->h * 2);
	tmp2 = GetPosRectMat(mat2, rectMy2, rectMy2->w / 2, -rectMy1->h * 2);
	sumScore += DoTestCompare(tmp1, tmp2);


	return sumScore;
}