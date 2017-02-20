#pragma once
#include <string>
#include <vector>
#include <opencv2\core.hpp>
#define  g_nFeatureSize 2048*4
#define  WM_DISPLAYRECORESULT_MSG (WM_USER+90)

#define  WM_CAPFILEINCOMIN_TOLOGIC_MESSAGE (WM_USER+91)
#define  WM_CAPFILEINCOMIN_TOSOCKET_MESSAGE (WM_USER+92)

#define WM_ORIGINDATA_MESSAGE (WM_USER+101)
#define WM_FRAME_TOLOGIC_MESSAGE (WM_USER+102)
#define WM_FEATURE_FACET_TOLOGIC_MESSAGE   (WM_USER+103)
#define WM_FEATUREINCOMING_TOLOGIC_MESSAGE (WM_USER+108)
#define WM_FEATUREINCOMING_TORECO_MESSAGE (WM_USER+109)
#define WM_FEATUREINCOMING_TOSOCKET_MESSAGE (WM_USER+110)

#define WM_CLASSFILEINFO_TOLOGIC_MESSAGE (WM_USER+111)
#define WM_TEMPLATE_TOLOGIC_MESSAGE (WM_USER+112)
#define WM_CLASSFILEINFO_TOSOCKET_MESSAGE (WM_USER+113)

#define WM_RECOINFO_TOLOGIC_MESSAGE (WM_USER+123)
#define WM_RECOINFO_TOSOCKET_MESSAGE (WM_USER+124)
#define WM_RECOINFOUNI_TOSOCKET_MESSAGE (WM_USER+125)
#define WM_RECOINFOUNI_TOLOGIC_MESSAGE  (WM_USER+126)

#define WM_SETSEPECIAL_TOLOGIC_MESSAGE (WM_USER+135)
#define WM_SETSEPECIAL_TOSOCKET_MESSAGE (WM_USER+136)

#define WM_LOGIN_MESSAGE (WM_USER+137)
#define WM_RECEIVEHEARTBEAT_MESSAGE (WM_USER+138)
#define WM_LOGOUT_MESSAGE (WM_USER+139)
#define WM_RECEIVECOUNT_MESSAGE (WM_USER+140)

#define WM_REG_CLASSIC_IMGS (WM_USER+141)
#define WM_REG_TORECO_IMGS (WM_USER+142)
struct Faceinfo_S
{
	Faceinfo_S()
	{

	}
	std::string hairID;

	std::string picname;
	std::string ptsname68;
	std::string ptsname5;

	cv::Mat matPic;
	std::vector<cv::Point> pts68;
	std::vector<cv::Point> pts5;

	float nRx;
	float nRy;
};

#define   FACE_BUF_WIDTH 640
#define   FACE_BUF_HEIGHT 480

#define MAX_WIDTH_SIZE	1920
#define  CUT_DATE_LEN 8

#define  CUSTOMER_TYPE_OTHER "其他"
#define  CUSTOMER_TYPE_GENERAL "一般粉丝"
#define  CUSTOMER_TYPE_SUPER "超级粉丝"
#define  CUSTOMER_TYPE_STAR "钻石粉丝"
#define  CUSTOMER_TYPE_VIP "VIP客户"
#define  CUSTOMER_TYPE_YUANGONG "员工"
#define  CUSTOMER_TYPE_NOTRECO "未能识别"


#include <string>
#include <vector>
using namespace std;
struct S_RecoInfo
{
	char szFileCapID[218];//抓拍图片名称或者ID
	char szDevID[64];//图片来自哪个设备的标志

	char szCUSTID[128];//CUST_ID
	char szGender[16];//性别  男，女
	char szAge[64];//少年,青年,中年,老年
	char szIdentity[128];//人物属性（黑名单，逃犯、小偷等等）

	int nImageLenCap;//抓拍人脸图片文件长度jpg
	int nImageLenReco;//模板库人脸图片长度jpg

	char * pImageReco;//模板库人脸图片数据
	char * pImageCap;//抓拍人脸图片数据
	char * pBufSerialize;
	int GetTotalLength()
	{
		return sizeof(szFileCapID) + sizeof(szDevID) +
			sizeof(szCUSTID) + sizeof(szGender)
			+ sizeof(szAge) + sizeof(szIdentity)
			+ sizeof(nImageLenCap) + nImageLenCap
			+ sizeof(nImageLenReco) + nImageLenReco;
	}
	char * ChangeToBuf()
	{
		pBufSerialize = new char[GetTotalLength()];
		int writeLen = 0;
		memcpy(pBufSerialize + writeLen, szFileCapID, sizeof(szFileCapID));
		writeLen += sizeof(szFileCapID);
		memcpy(pBufSerialize + writeLen, szDevID, sizeof(szDevID));
		writeLen += sizeof(szDevID);

		memcpy(pBufSerialize + writeLen, szCUSTID, sizeof(szCUSTID));
		writeLen += sizeof(szCUSTID);
		memcpy(pBufSerialize + writeLen, szGender, sizeof(szGender));
		writeLen += sizeof(szGender);
		memcpy(pBufSerialize + writeLen, szAge, sizeof(szAge));
		writeLen += sizeof(szAge);
		memcpy(pBufSerialize + writeLen, szIdentity, sizeof(szIdentity));
		writeLen += sizeof(szIdentity);


		memcpy(pBufSerialize + writeLen, &nImageLenCap, 4);
		writeLen += 4;
		if (nImageLenCap > 0)
		{
			memcpy(pBufSerialize + writeLen, pImageCap, nImageLenCap);
			writeLen += nImageLenCap;
		}
		memcpy(pBufSerialize + writeLen, &nImageLenReco, 4);
		writeLen += 4;
		if (nImageLenReco > 0)
		{
			memcpy(pBufSerialize + writeLen, pImageReco, nImageLenReco);
		}


		return pBufSerialize;
	}

	void AnalyzeBuf(char * buf)
	{
		int writeLen = 0;
		memcpy(szFileCapID, buf + writeLen, sizeof(szFileCapID));
		writeLen += sizeof(szFileCapID);
		memcpy(szDevID, buf + writeLen, sizeof(szDevID));
		writeLen += sizeof(szDevID);

		memcpy(szCUSTID, buf + writeLen, sizeof(szCUSTID));
		writeLen += sizeof(szCUSTID);
		memcpy(szGender, buf + writeLen, sizeof(szGender));
		writeLen += sizeof(szGender);
		memcpy(szAge, buf + writeLen, sizeof(szAge));
		writeLen += sizeof(szAge);
		memcpy(szIdentity, buf + writeLen, sizeof(szIdentity));
		writeLen += sizeof(szIdentity);


		memcpy(&nImageLenCap, buf + writeLen, 4);
		writeLen += 4;
		if (nImageLenCap > 0)
		{
			pImageCap = new char[nImageLenCap];
			memcpy(pImageCap, buf + writeLen, nImageLenCap);
			writeLen += nImageLenCap;
		}
		memcpy(&nImageLenReco, buf + writeLen, 4);
		writeLen += 4;
		if (nImageLenReco > 0)
		{
			pImageReco = new char[nImageLenReco];
			memcpy(pImageReco, buf + writeLen, nImageLenReco);
		}

	}
	void Init()
	{
		memset(szFileCapID, 0, sizeof(szFileCapID));
		memset(szDevID, 0, sizeof(szDevID));
		memset(szCUSTID, 0, sizeof(szCUSTID));
		memset(szAge, 0, sizeof(szAge));
		memset(szGender, 0, sizeof(szGender));
		memset(szIdentity, 0, sizeof(szIdentity));

		nImageLenCap = 0;
		nImageLenReco = 0;

		pImageCap = NULL;
		pImageReco = NULL;
		pBufSerialize = NULL;
	}
	S_RecoInfo()
	{
		Init();
	}
	S_RecoInfo(int imageLenCap, int imageLenReco)
	{
		Init();
		nImageLenCap = imageLenCap;
		nImageLenReco = imageLenReco;
		if (nImageLenCap > 0)
		{

			pImageCap = new char[nImageLenCap];
		}
		if (nImageLenReco > 0)
		{

			pImageReco = new  char[nImageLenReco];
		}
	}
	~S_RecoInfo()
	{
		if (pImageCap)
		{
			delete[] pImageCap;
			pImageCap = NULL;
		}
		if (pImageReco)
		{
			delete[] pImageReco;
			pImageReco = NULL;
		}
		if (pBufSerialize)
		{
			delete[] pBufSerialize;
			pBufSerialize = NULL;
		}
	}
};



cv::Mat DrawObj(Faceinfo_S& hairInfo,int nSex);
void savePtsFile(const char * idName, std::vector<cv::Point>& points);
void readPtsFile(const char *  IDname, std::vector<cv::Point>& points);