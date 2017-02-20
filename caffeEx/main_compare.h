#pragma once



#ifdef THIS_CAFFE_DLL
#define EXPORT_CAFFE_DLL  __declspec(dllexport) 
#else
#define EXPORT_CAFFE_DLL __declspec(dllimport) 
#endif


struct S_MY_RECT
{
	int x;
	int y;
	int w;
	int h;
};
//EXPORT_CAFFE_DLL void caffe_comareinit(char * szPath);
extern "C"
{

#ifdef USE_CV_COMPARE
	//比较方法1
	//输入参数rect1,rect2 为S_MY_RECT结构地址 
	//data1,参数比较的图像1的图像数据指针地址，w1图像宽，h1图像高，图像为三通道BGR格式或者RGB格式
	EXPORT_CAFFE_DLL float  main_compareCv(char* data1, int w1, int h1, S_MY_RECT * rect1,
		char * data2, int w2, int h2, S_MY_RECT * rect2);

	EXPORT_CAFFE_DLL float  main_compareCv2(char* data1pathname, S_MY_RECT * rect1,
		char * data2pathname, S_MY_RECT * rect2);
#endif

	//比较方法2
	//输入参数rect1,rect2 为S_MY_RECT结构地址 
	//data1,参数比较的图像1的图像数据指针地址，w1图像宽，h1图像高，图像为三通道BGR格式或者RGB格式
	EXPORT_CAFFE_DLL float  main_compareCaffe(char* data1, int w1, int h1, S_MY_RECT * rect1,
		char * data2, int w2, int h2, S_MY_RECT *rect2);
	EXPORT_CAFFE_DLL float  main_compareCaffe2(char* datapathname1,
		S_MY_RECT * rect1, char * datapathname2, S_MY_RECT *rect2);

}