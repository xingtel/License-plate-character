#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <map>
#include <cmath>


//#include <string>

extern char g_rootpath[512];

bool  ISFileExist(char * szFileName);
void ResizeImage(unsigned char *src, int nSrcWidth, int nSrcHeight, unsigned char *dst,
		int nDstWidth, int nDstHeight, int nChannel);


bool ResizeImageGray(const unsigned char *src_im, int src_width, int src_height,
	unsigned char* dst_im, int dst_width, int dst_height);