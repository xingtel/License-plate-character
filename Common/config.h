#if !defined(_CONFIG_H)
#define _CONFIG_H

#include <io.h>

typedef struct __SysArgsConf_STRUCT
{
	//定义系统的参数
	char szConfFile[MAX_PATH];
	int  nSubAge;
	int  nRecoMinPix;//识别最小像素

	__SysArgsConf_STRUCT()	{
		char szCurPath[MAX_PATH] = {0};
		char *pFind = NULL;
		

		memset(szConfFile,0,sizeof(szConfFile));

		GetModuleFileName(NULL, szCurPath, sizeof(szCurPath));
		pFind = strrchr(szCurPath, '\\');
		memset((void *)pFind, 0x00, strlen(pFind));
	
		sprintf(szConfFile, "%s\\sys_conf_args.ini", szCurPath);
		nSubAge = GetSubAge();
		nRecoMinPix = GetRecoMinPix();
		{//HUPAN_START: 如果没有配置文件，则创建一个
			if (_access(szConfFile, 0) == -1)	{
				WritePrivateProfileString("SysRunArgs", "UsbCameraIndex", "1", szConfFile);
				WritePrivateProfileString("SysRunArgs", "UsbCapCameraIndex", "0", szConfFile);
				SetMainPicHue("-7");
				SetMainPicSaturation("6");
				SetMainCapRatio("640X480");
				SetCapIdentityRatio("640X480");
				SetCardToCardFazhi(50);
				SetPersonToCardFazhi(40);
				SetKuIP("127.0.0.1");
				SetKuPort(5668);
				SetKuUserName("admin");
				SetKuPWD("admin");
				SetSubAge("4");
				SetRecoMinPix(92);
				SetSourceCap();
			}
		}//HUPAN_END
	}

	int GetKuPort()
	{
		return GetPrivateProfileInt("SysRunArgs", "KuPort", 5668, szConfFile);
	}
	void SetKuPort(int port)
	{
		CString strPort;
		strPort.Format("%d",port);
		WritePrivateProfileString("SysRunArgs", "KuPort", strPort, szConfFile);
	}
	int GetRecoMinPix()
	{
		return GetPrivateProfileInt("SysRunArgs", "RecoMinPix", 120, szConfFile);
	}
	void SetRecoMinPix(int minPix)
	{
		CString text;
		text.Format("%d",minPix);
		WritePrivateProfileString("SysRunArgs", "RecoMinPix", text, szConfFile);
		nRecoMinPix = minPix;
	}

	CString GetKuUserName()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "KuUserName","admin",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	void SetKuUserName(CString  userName)
	{
		WritePrivateProfileString("SysRunArgs", "KuUserName", userName, szConfFile);
	}

	CString GetKuPWD()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "KuPWD","admin",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	void SetKuPWD(CString kuPwd)
	{
		WritePrivateProfileString("SysRunArgs", "KuPWD", kuPwd, szConfFile);
	}


	CString GetKuIP()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "KuIP","127.0.0.1",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	void SetKuIP(CString iP)
	{
		WritePrivateProfileString("SysRunArgs", "KuIp", iP, szConfFile);
	}
	int GetCardToCardFazhi()
	{
		return GetPrivateProfileInt("SysRunArgs", "CardToCardFazhi", 0, szConfFile);
	}

	void SetCardToCardFazhi(int faZhi)
	{
		CString strFazhi;
		strFazhi.Format("%d",faZhi);
		WritePrivateProfileString("SysRunArgs", "CardToCardFazhi", strFazhi, szConfFile);
	}

	int GetPersonToCardFazhi()
	{
		return GetPrivateProfileInt("SysRunArgs", "PersonToCardFazhi", 0, szConfFile);
	}

	void SetPersonToCardFazhi(int fazhi)
	{
		CString strFazhi;
		strFazhi.Format("%d",fazhi);
		WritePrivateProfileString("SysRunArgs", "PersonToCardFazhi", strFazhi, szConfFile);
	}

	//身分证信息,是否是读取卡的方式
	int IsCapCard()
	{
		//默认用图片抓拍的方式；
		//如果值为1的话，则用读取卡片的方式
		return GetPrivateProfileInt("SysRunArgs", "CapCard", 0, szConfFile);
	}

	//主摄像头index
	int GetUsbMainCameraIndex()
	{
		return  GetPrivateProfileInt("SysRunArgs", "UsbCameraIndex", 1, szConfFile);
	}
	//身分证摄像头index
	int GetUsbCapidentityCameraIndex()
	{
		return GetPrivateProfileInt("SysRunArgs", "UsbCapCameraIndex", 0, szConfFile);
	}

	//身分证信息读卡方式
	void SetSourceCard()
	{
		WritePrivateProfileString("SysRunArgs", "CapCard", "1", szConfFile);
	}

	//身分证抓拍图像方式 
	void SetSourceCap()
	{
		WritePrivateProfileString("SysRunArgs", "CapCard", "0", szConfFile);
	}

	

	//主USB色调
	int GetMainPicHue()
	{
		return GetPrivateProfileInt("SysRunArgs", "MainPicHue", -7, szConfFile);
	}

	void SetMainPicHue(CString hue)
	{
		WritePrivateProfileString("SysRunArgs", "MainPicHue", hue, szConfFile);
	}
	//主USB饱和度
	int GetMainPicSaturation()
	{
		return GetPrivateProfileInt("SysRunArgs", "MainPicSaturation", 6, szConfFile);
	}
	int GetSubAge()
	{
		return GetPrivateProfileInt("SysRunArgs", "SubAge", 15, szConfFile);
	}

	void SetSubAge(CString subAge)
	{
		WritePrivateProfileString("SysRunArgs", "SubAge", subAge, szConfFile);
	}
	void SetMainPicSaturation(CString saturation)
	{
		WritePrivateProfileString("SysRunArgs", "MainPicSaturation", saturation, szConfFile);
	}
	//主抓拍分辨率
	void SetMainCapRatio(CString capRatio)
	{
		WritePrivateProfileString("SysRunArgs", "UsbMainCapRatio", capRatio, szConfFile);
	}

	//获取主抓拍分辨率
	CString GetMainRatio()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "UsbMainCapRatio","640X480",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//获取身份证抓拍分辨率
	CString GetCapIdentityRatio()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "UsbCapIdentityRatio","640X480",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//设置身份证抓拍分辨率 
	void SetCapIdentityRatio(CString capRatio)
	{
		WritePrivateProfileString("SysRunArgs", "UsbCapIdentityRatio", capRatio, szConfFile);
	}

	//获取界面语言方式
	CString GetLanguage()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "Language","中文",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//设置界面语言方式
	void Setlanguage(CString language)
	{
		WritePrivateProfileString("SysRunArgs", "Language", language, szConfFile);
	}

	~__SysArgsConf_STRUCT()	{
		
	}
} SYS_ARGS_CONF_STRUCT;

#endif