#pragma once

#include <opencv2\core.hpp>

#ifdef THIS_CAFFE_DLL
#define EXPORT_CAFFE_DLL  __declspec(dllexport) 
#else
#define EXPORT_CAFFE_DLL __declspec(dllimport) 
#endif

EXPORT_CAFFE_DLL int main_converimgeset(int argc, char** argv);
EXPORT_CAFFE_DLL void caffe_InitMtcnn();

EXPORT_CAFFE_DLL  int caffemain(int argc, char** argv);

EXPORT_CAFFE_DLL void caffe_InitPlateX();
EXPORT_CAFFE_DLL void plateX_reco(char * szBuf, std::string& outR);
EXPORT_CAFFE_DLL float plateX_recoMat(cv::Mat img, std::string& outR, bool bChinese);
EXPORT_CAFFE_DLL float plateX_recoCh(cv::Mat img, std::string& outR, bool bNum);

EXPORT_CAFFE_DLL int mainMtcnn(cv::Mat img);


