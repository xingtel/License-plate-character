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
	//�ȽϷ���1
	//�������rect1,rect2 ΪS_MY_RECT�ṹ��ַ 
	//data1,�����Ƚϵ�ͼ��1��ͼ������ָ���ַ��w1ͼ���h1ͼ��ߣ�ͼ��Ϊ��ͨ��BGR��ʽ����RGB��ʽ
	EXPORT_CAFFE_DLL float  main_compareCv(char* data1, int w1, int h1, S_MY_RECT * rect1,
		char * data2, int w2, int h2, S_MY_RECT * rect2);

	EXPORT_CAFFE_DLL float  main_compareCv2(char* data1pathname, S_MY_RECT * rect1,
		char * data2pathname, S_MY_RECT * rect2);
#endif

	//�ȽϷ���2
	//�������rect1,rect2 ΪS_MY_RECT�ṹ��ַ 
	//data1,�����Ƚϵ�ͼ��1��ͼ������ָ���ַ��w1ͼ���h1ͼ��ߣ�ͼ��Ϊ��ͨ��BGR��ʽ����RGB��ʽ
	EXPORT_CAFFE_DLL float  main_compareCaffe(char* data1, int w1, int h1, S_MY_RECT * rect1,
		char * data2, int w2, int h2, S_MY_RECT *rect2);
	EXPORT_CAFFE_DLL float  main_compareCaffe2(char* datapathname1,
		S_MY_RECT * rect1, char * datapathname2, S_MY_RECT *rect2);

}