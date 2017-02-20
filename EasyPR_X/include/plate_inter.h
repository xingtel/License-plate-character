#pragma once

#ifdef THIS_PLATEDLL
#define PLATEDLL_EXPORT  __declspec(dllexport) 
#else
#define PLATEDLL_EXPORT __declspec(dllimport) 
#endif
struct S_RECT
{
	int x;
	int y;
	int w;
	int h;
};
typedef struct TH_RECT
{
	int left;
	int top;
	int right;
	int bottom;
}TH_RECT;
typedef struct TH_PlateResult
{
	char license[16];   // 车牌号码
	char color[8];      // 车牌颜色
	int nColor;			// 车牌颜色序号
	int nType;			// 车牌类型
	int nConfidence;	// 车牌可信度
	int nBright;		// 亮度评价
	int nDirection;		// 运动方向，0 unknown, 1 left, 2 right, 3 up , 4 down	
	TH_RECT rcLocation; //车牌位置
	int nTime;          //识别所用时间
	unsigned char nCarBright;		//车的亮度
	unsigned char nCarColor;		//车的颜色
	char reserved[100];				// 保留
}TH_PlateResult;

extern "C"
{
	//若不需要底层显示处理过程 wnd 设置为0
	PLATEDLL_EXPORT void plate_Init(void * wnd = 0);

	PLATEDLL_EXPORT int  plate_reco(char * fileName, TH_PlateResult * pResult, int &nRecogNum);
}

PLATEDLL_EXPORT int  plate_recognize(char * filename, int param);
PLATEDLL_EXPORT int  plate_detect(char * fileName, int type);


PLATEDLL_EXPORT float plate_compareCaffe(char* data1, int w1, int h1, S_RECT * rect1,
	char * data2, int w2, int h2, S_RECT *rect2);

PLATEDLL_EXPORT float plate_compareCaffe2(char* datapathname1,
	S_RECT * rect1, char * datapathname2, S_RECT *rect2);