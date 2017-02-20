
#pragma once
#include <opencv2\core\core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>


std::string GetBase64ByfileName(std::string fileName);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

//unsigned char SrcEncode[256] = {2,5,};

void MatImgcodeInit();
void MatImgEncode(cv::Mat& mat);
void MatImgDecode(cv::Mat& mat);

void BufEncode(unsigned char * szBuf,int bufLen);
void BufDecode(unsigned char * szBuf,int bufLen);

//对文件加密或者解密，并保存到目的位置
void copyfileAndEncode(char * szSrcName,char * dstName);
void copyfileAndDecode(char * szSrcName,char * dstName);
void decodePngfileToMat(char * szSrcName, cv::Mat& outMat);

//从png文件流中提取数据
bool loadFromStream(unsigned char* data, const unsigned int dataSize,cv::Mat& outMat);