#ifndef _COMMONWIN_H
#define _COMMONWIN_H

#define WM_IMG_FROMCAMRA (WM_USER+1111)
#define WM_IMG_FROMTHUMHIT (WM_USER+1112)
#define WM_IMG_FROMFILE (WM_USER+1113)
#define WM_IMG_PAINTSHOW (WM_USER+1114)
#define WM_IMG_TODOMERGE (WM_USER+1115)
#define WM_IMG_TOINITMERGE (WM_USER+1116)

#define WM_IMG_FILTER_CARTOON (WM_USER+1117)//catoon�˾�
#define WM_IMG_FILTER_OIL (WM_USER+1118)//�ͻ��˾�


#define WM_SHOWTOFRONT (WM_USER+1212)
#define WM_PLAYADVER   (WM_USER+1213)
#define WM_STOPADVER   (WM_USER+1214)

#define WM_ADV_XING_SHOWADV    (WM_USER+1215)//֪ͨ��ʾ���
#define WM_ADV_XING_INITADVDB    (WM_USER+1216)//֪ͨ��ʼ�����ݿ�
#define WM_ADV_XING_ADVLANG    (WM_USER+1217) //���ù����
#define WM_ADV_XINGKONG_NEWVERSIN    (WM_USER+1218) //�����µİ汾


#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <string>
#include <vector>


CString ShortName(CString name);
extern bool gbCompleteWorkFemale;//�Զ�����Ů�Է���
extern bool gbCompleteWorkMale ;//�Զ��������Է���
extern bool gbCompleteUpdate ;//�Զ�����



void ContructBih(int nWidth,int nHeight,BITMAPINFOHEADER& bih,int Bit = 24);

//���ݴ������Ʒ�����Ϣ
void PostMsgByCapname(CString capname,UINT msg,WPARAM param=0,LPARAM lparam=0);

void DrawBmpBuf(BITMAPINFOHEADER& bih,unsigned char* pDataBuf,HWND hShowWnd,int posX = 0,int posY = 0);

void DrawMyText(CString text,int posX,int posY,CDC* pDc,int FontSize,COLORREF textColor);
void DrawBmpBuf(BITMAPINFOHEADER& bih,unsigned char* pDataBuf,HWND hShowWnd,CWnd * pWnd,char * szText,int posX,int posY);



int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
bool DeleteDirectory(LPCTSTR DirName);
//�����ļ�������
void TraverseDir(CString dir, std::vector<std::string>& vec);
void TraverseDirShortName(CString dir, std::vector<std::string>& vec);

 CString	GetExePath();

bool IsNumber( LPCTSTR pszText );

void DEBUG_STRING(const char *fmt,...);
bool CreateMultipleDirectory(const CString& szPath);
void GetMacString(std::string& strMac,std::string& strIP);


unsigned long GetFileSize(const char *filename);
HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color,CWnd * pWnd);
//��ȡ�ļ�������չ��
CString GetFileTitleFromFileName(CString FileName,CString& ExtendName); 

//������
void DrawMyText(CString text,int posX,int posY,CWnd * pWnd,int FontSize = 30,COLORREF textColor = RGB(0,0,0));
void DrawMyTextBK(CString text,int posX,int posY,CWnd * pWnd,COLORREF bkColor ,int FontSize = 30,COLORREF textColor = RGB(0,0,0));

void DrawMyTextType(CString type,CString text,
					int posX,int posY,CWnd * pWnd,int FontSize = 30);


//�����ַ���ֵ����
//
int SetValue_S(HKEY ReRootKey,TCHAR *ReSubKey,TCHAR *ReValueName,LPBYTE ReSetContent_S);
//
//����DWORDֵ����
//
int SetValue_D(HKEY ReRootKey,TCHAR *ReSubKey,TCHAR *ReValueName,int ReSetContent_D[256]);
int DeleteKey(HKEY ReRootKey,TCHAR *ReSubKey,TCHAR *ReKeyName);
int DeleteValue(HKEY ReRootKey,TCHAR *ReSubKey,TCHAR *ReValueName);

//ͨ����������ý��̾��
HANDLE GetHandleByProcessName(char * name,DWORD ProcessID=0);
bool CreateShortCut( CString FilePath, CString LinkPath);

void SetIECoreVersion();
bool RaisePrivileges();


#endif //_COMMON_H

