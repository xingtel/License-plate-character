#pragma once
// Base64.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
class  CBase64  
{
public:
CBase64();
virtual ~CBase64();

public:
int DecodeBase64(const char* pSrc, unsigned char* pDst, int nSrcLen);
std::string EncodeBase64(const unsigned char* pSrc, int nSrcLen, int nMaxLineLen = 21000000);

};



