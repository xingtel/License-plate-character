
#pragma once
/*
����  ͼƬ��ʾ�ӿ�

����:sunjunlishi

��Ȩ�����������Ƽ�.����

*/
#include <opencv2\core.hpp>

extern void *  gShowWnd;

void DrawThePicPoint(void * pWnd, cv::Mat matSrc, int posX, int posY, int dstW = 700);