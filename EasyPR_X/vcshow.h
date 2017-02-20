
#pragma once
/*
功能  图片显示接口

作者:sunjunlishi

版权：厦门兴联科技.深圳

*/
#include <opencv2\core.hpp>

extern void *  gShowWnd;

void DrawThePicPoint(void * pWnd, cv::Mat matSrc, int posX, int posY, int dstW = 700);