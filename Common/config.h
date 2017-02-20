#if !defined(_CONFIG_H)
#define _CONFIG_H

#include <io.h>

typedef struct __SysArgsConf_STRUCT
{
	//����ϵͳ�Ĳ���
	char szConfFile[MAX_PATH];
	int  nSubAge;
	int  nRecoMinPix;//ʶ����С����

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
		{//HUPAN_START: ���û�������ļ����򴴽�һ��
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

	//���֤��Ϣ,�Ƿ��Ƕ�ȡ���ķ�ʽ
	int IsCapCard()
	{
		//Ĭ����ͼƬץ�ĵķ�ʽ��
		//���ֵΪ1�Ļ������ö�ȡ��Ƭ�ķ�ʽ
		return GetPrivateProfileInt("SysRunArgs", "CapCard", 0, szConfFile);
	}

	//������ͷindex
	int GetUsbMainCameraIndex()
	{
		return  GetPrivateProfileInt("SysRunArgs", "UsbCameraIndex", 1, szConfFile);
	}
	//���֤����ͷindex
	int GetUsbCapidentityCameraIndex()
	{
		return GetPrivateProfileInt("SysRunArgs", "UsbCapCameraIndex", 0, szConfFile);
	}

	//���֤��Ϣ������ʽ
	void SetSourceCard()
	{
		WritePrivateProfileString("SysRunArgs", "CapCard", "1", szConfFile);
	}

	//���֤ץ��ͼ��ʽ 
	void SetSourceCap()
	{
		WritePrivateProfileString("SysRunArgs", "CapCard", "0", szConfFile);
	}

	

	//��USBɫ��
	int GetMainPicHue()
	{
		return GetPrivateProfileInt("SysRunArgs", "MainPicHue", -7, szConfFile);
	}

	void SetMainPicHue(CString hue)
	{
		WritePrivateProfileString("SysRunArgs", "MainPicHue", hue, szConfFile);
	}
	//��USB���Ͷ�
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
	//��ץ�ķֱ���
	void SetMainCapRatio(CString capRatio)
	{
		WritePrivateProfileString("SysRunArgs", "UsbMainCapRatio", capRatio, szConfFile);
	}

	//��ȡ��ץ�ķֱ���
	CString GetMainRatio()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "UsbMainCapRatio","640X480",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//��ȡ���֤ץ�ķֱ���
	CString GetCapIdentityRatio()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "UsbCapIdentityRatio","640X480",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//�������֤ץ�ķֱ��� 
	void SetCapIdentityRatio(CString capRatio)
	{
		WritePrivateProfileString("SysRunArgs", "UsbCapIdentityRatio", capRatio, szConfFile);
	}

	//��ȡ�������Է�ʽ
	CString GetLanguage()
	{
		char szReturn[128];
		memset(szReturn,0,sizeof(szReturn));
		GetPrivateProfileString("SysRunArgs", "Language","����",szReturn,sizeof(szReturn),szConfFile);
		return szReturn;
	}

	//���ý������Է�ʽ
	void Setlanguage(CString language)
	{
		WritePrivateProfileString("SysRunArgs", "Language", language, szConfFile);
	}

	~__SysArgsConf_STRUCT()	{
		
	}
} SYS_ARGS_CONF_STRUCT;

#endif