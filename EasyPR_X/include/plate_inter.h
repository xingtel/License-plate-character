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
	char license[16];   // ���ƺ���
	char color[8];      // ������ɫ
	int nColor;			// ������ɫ���
	int nType;			// ��������
	int nConfidence;	// ���ƿ��Ŷ�
	int nBright;		// ��������
	int nDirection;		// �˶�����0 unknown, 1 left, 2 right, 3 up , 4 down	
	TH_RECT rcLocation; //����λ��
	int nTime;          //ʶ������ʱ��
	unsigned char nCarBright;		//��������
	unsigned char nCarColor;		//������ɫ
	char reserved[100];				// ����
}TH_PlateResult;

extern "C"
{
	//������Ҫ�ײ���ʾ������� wnd ����Ϊ0
	PLATEDLL_EXPORT void plate_Init(void * wnd = 0);

	PLATEDLL_EXPORT int  plate_reco(char * fileName, TH_PlateResult * pResult, int &nRecogNum);
}

PLATEDLL_EXPORT int  plate_recognize(char * filename, int param);
PLATEDLL_EXPORT int  plate_detect(char * fileName, int type);


PLATEDLL_EXPORT float plate_compareCaffe(char* data1, int w1, int h1, S_RECT * rect1,
	char * data2, int w2, int h2, S_RECT *rect2);

PLATEDLL_EXPORT float plate_compareCaffe2(char* datapathname1,
	S_RECT * rect1, char * datapathname2, S_RECT *rect2);