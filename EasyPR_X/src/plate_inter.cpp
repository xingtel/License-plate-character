#include <plate_inter.h>

#include "easypr.h"
#include "plate_t.hpp"
#include "vcshow.h"
#include "./minicaffe/inter_char/inter_char.h"
#include "./minicaffe/main_compare/main_compare.h"
#include <stdlib.h>
#include <algorithm>

void plate_Init(void * wnd)
{
	gShowWnd = wnd;
	caffe_InitPlateX();
}

int  plate_reco(char * fileName, TH_PlateResult * pResult, int &nRecogNum)
{
	vector<plate_info> prinfos;
	platex::demo::test_plate_recognize(fileName, platex::PR_DETECT_SOBEL, prinfos);

	  platex::demo::test_plate_detect(fileName,
		platex::PR_DETECT_SOBEL | platex::PR_DETECT_CMSER, prinfos);

	  int minCount = min(20,(int) prinfos.size());
	  for (int k = 0; k < minCount; ++k)
	  {
		  strcpy(pResult[k].color, prinfos[k].color.c_str());
		  strcpy(pResult[k].license, prinfos[k].license.c_str());
	  }
	  nRecogNum = minCount;

	  return 1;

}

int plate_recognize(char * filename, int param)
{
	vector<plate_info> prinfos;
	return platex::demo::test_plate_recognize(filename, param, prinfos);
}


int plate_detect(char * fileName, int type)
{
	vector<plate_info> prinfos;
	return platex::demo::test_plate_detect(fileName,
		type, prinfos);
}


float plate_compareCaffe(char* data1, int w1, int h1, S_RECT * rect1,
	char * data2, int w2, int h2, S_RECT *rect2)
{
	return main_compareCaffe(data1,w1,h1,(S_MY_RECT*)rect1,data2,w2,h2, (S_MY_RECT*)rect2);
}

float plate_compareCaffe2(char* datapathname1,
	S_RECT * rect1, char * datapathname2, S_RECT *rect2)
{
	return main_compareCaffe2(datapathname1, (S_MY_RECT*)rect1, datapathname2, (S_MY_RECT*)rect2);
}