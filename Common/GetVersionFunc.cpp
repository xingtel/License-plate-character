// GlobalFunctions.cpp: Implementierung der Klasse CGetVersionFunc.
//
//////////////////////////////////////////////////////////////////////

#include <Common/CommonWin.h>
#include "GetVersionFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CString CGetVersionFunc::m_csFileVersion;
CString CGetVersionFunc::m_csProductVersion;

CGetVersionFunc::CGetVersionFunc()
{

}

CGetVersionFunc::~CGetVersionFunc()
{

}
//获取 文件版本号的
//    pName    _T("CompanyName"),_T("FileDescription"),_T("FileVersion"),
//    _T("InternalName"),_T("LegalCopyright"),_T("OriginalFilename"),
//    _T("ProductName"),_T("ProductVersion"),
/*
BOOL CTrayMgrDlg::GetFileVersionString(CHAR * pFileName, _bstr_t * pRecvStr, TCHAR * pName )
{
    DWORD    dwDummyHandle; // will always be set to zero
    DWORD    dwLen;
    BYTE    * pVersionInfo;
    BOOL    bRetVal;

    VS_FIXEDFILEINFO    FileVersion;
    *pRecvStr = "";

    struct TRANSLATION {
        WORD langID;            // language ID
        WORD charset;            // character set (code page)
    } Translation;

    HMODULE        hVerDll;
    hVerDll = LoadLibrary(_T("VERSION.dll"));
    if(hVerDll == NULL)
        return FALSE;

    typedef DWORD (WINAPI *Fun_GetFileVersionInfoSizeA)(CHAR *, DWORD *);
    typedef BOOL  (WINAPI *Fun_GetFileVersionInfoA)(CHAR *, DWORD, DWORD, LPVOID);
    typedef BOOL  (WINAPI *Fun_VerQueryValueA)(LPVOID, CHAR *, LPVOID, PUINT);


    Fun_GetFileVersionInfoSizeA        pGetFileVersionInfoSizeA;
    Fun_GetFileVersionInfoA            pGetFileVersionInfoA;
    Fun_VerQueryValueA                pVerQueryValueA;

    pGetFileVersionInfoSizeA = (Fun_GetFileVersionInfoSizeA)::GetProcAddress(hVerDll, "GetFileVersionInfoSizeA");
    pGetFileVersionInfoA     = (Fun_GetFileVersionInfoA)::GetProcAddress(hVerDll, "GetFileVersionInfoA");
    pVerQueryValueA             = (Fun_VerQueryValueA)::GetProcAddress(hVerDll, "VerQueryValueA");

    Translation.langID = 0x0409;    //
    Translation.charset = 1252;        // default = ANSI code page

    dwLen = pGetFileVersionInfoSizeA(pFileName, &dwDummyHandle);
    if (dwLen == 0)
        return FALSE;

    pVersionInfo = new BYTE[dwLen]; // allocate version info
    bRetVal = pGetFileVersionInfoA(pFileName, 0, dwLen, pVersionInfo);
    if(bRetVal == FALSE)
    {
        delete [] pVersionInfo;
        return FALSE;
    }

    VOID    * pVI;
    UINT    uLen;

    bRetVal = pVerQueryValueA(pVersionInfo, "\\", &pVI, &uLen);
    if(bRetVal == FALSE)
    {
        delete [] pVersionInfo;
        return FALSE;
    }
	
    memcpy(&FileVersion, pVI, sizeof(VS_FIXEDFILEINFO));
	
    bRetVal = pVerQueryValueA(pVersionInfo, "\\VarFileInfo\\Translation", 
        &pVI, &uLen);
    if(bRetVal && uLen >= 4)
    {
        memcpy(&Translation, pVI, sizeof(TRANSLATION));
    }
	
  //  BREAKIF(FileVersion.dwSignature != VS_FFI_SIGNATURE);
	if (FileVersion.dwSignature != VS_FFI_SIGNATURE)
	{
		return FALSE;
	}
	
    CHAR        szQuery[1024];
    CHAR        * pVal;
    UINT        iLenVal;
	
    if(pName == NULL)
    {
        sprintf(szQuery, "%d.%d.%d.%d",
            HIWORD(FileVersion.dwFileVersionMS), LOWORD(FileVersion.dwFileVersionMS),
            HIWORD(FileVersion.dwFileVersionLS), LOWORD(FileVersion.dwFileVersionLS));
        *pRecvStr = szQuery;
    }
    else
    {
        _bstr_t bStrNtyLine1 = pName;
        sprintf(szQuery, "\\StringFileInfo\\%04X%04X\\%s",
            Translation.langID, Translation.charset, (CHAR *)bStrNtyLine1);
		
        bRetVal = pVerQueryValueA(pVersionInfo, szQuery, (LPVOID*)&pVal, &iLenVal);
        if(bRetVal)
            *pRecvStr = pVal;
        else
            *pRecvStr = "";
    }
	
    delete [] pVersionInfo;
    return TRUE;
}*/
/***********************************************************************************/
/*                                                                                 */
/* Class:   CGetVersionFunc                                                       */
/* Method:  GetVersionInfo                                                         */
/*                                                                                 */
/* Parameters:                                                                     */
/* -----------                                                                     */
/*   HMODULE hLib                                                                  */
/*                Handle to the module that contains the resource (EXE or DLL)     */            
/*                A value of NULL specifies the current applications resources     */ 
/*                                                                                 */
/*   CString csEntry                                                               */
/*                Specifies the name of the resource. For more information,        */
/*                see the Remarks section.                                         */
/*                                                                                 */
/* Return Values:                                                                  */
/* --------------                                                                  */
/* If the function succeeds, the return value is a string containing the value     */
/* of the specified resource.                                                      */ 
/* If the function fails, the returned string is empty. To get extended error      */
/* information, call GetLastError.                                                 */
/*                                                                                 */
/* Remarks:                                                                        */
/* --------                                                                        */
/* Since the Win32 API resource information is encoded in Unicode, this method     */
/* also strips the strings from Unicode.                                           */
/*                                                                                 */
/* The following valid values for csEntry, as specified by Microsoft are:          */
/*   CompanyName, FileDescription, FileVersion, InternalName, LegalCopyright,      */
/*   OriginalFilename, ProductName, ProductVersion, Comments, LegalTrademarks,     */
/*   PrivateBuild, SpecialBuild                                                    */
/*                                                                                 */
/* Opening the rc-file as "text" or with a text-editor allows you to add further   */
/* entries to your version information structure and it is retrievable using       */
/* this same method.                                                               */
/*                                                                                 */
/***********************************************************************************/
CString CGetVersionFunc::GetVersionInfoPath(CString exeFullPath,CString csEntry)
{
	CString csRet;
	 DWORD    dwDummyHandle;
	int  dwLen = GetFileVersionInfoSize(exeFullPath, &dwDummyHandle);
    if (dwLen == 0)
        return csRet;
	BYTE* versionInfo = new BYTE[dwLen]; // allocate version info
    BOOL bRetVal = GetFileVersionInfo(exeFullPath, 0, dwLen, versionInfo);
    if(bRetVal == FALSE)
    {
        delete [] versionInfo;
        return csRet;
    }

  
      if (versionInfo != NULL)
      {
        DWORD vLen = 0,langD = 0;
        BOOL retVal = FALSE;    
    
        LPVOID retbuf= NULL;
    
         char fileEntry[256];

        sprintf(fileEntry,"\\VarFileInfo\\Translation");
        retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
        if (retVal && vLen==4) 
        {
          memcpy(&langD,retbuf,4);            
          sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                  (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
                  (langD & 0xff0000)>>16, csEntry);            
        }
        else 
          sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s",  GetUserDefaultLangID(), csEntry);
		
	

		if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen)) 
			 	csRet = (char*)retbuf;
			
		
		
      }



  return csRet;
}
CString CGetVersionFunc::GetVersionInfo(HMODULE hLib, CString csEntry)
{
	
  CString csRet;

  if (hLib == NULL)
    hLib = AfxGetResourceHandle();
  
  HRSRC hVersion = FindResource( hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
  if (hVersion != NULL)
  {
    HGLOBAL hGlobal = LoadResource( hLib, hVersion ); 
    if ( hGlobal != NULL)  
    {  
  
      LPVOID versionInfo  = LockResource(hGlobal);  
      if (versionInfo != NULL)
      {
        DWORD vLen = 0,langD = 0;
        BOOL retVal = FALSE;    
    
        LPVOID retbuf= NULL;
    
        static char fileEntry[256];

       sprintf(fileEntry,"\\VarFileInfo\\Translation");
        retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
        if (retVal && vLen==4) 
        {
          memcpy(&langD,retbuf,4);            
          sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                  (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
                  (langD & 0xff0000)>>16, csEntry);            
        }
        else 
          sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s",  GetUserDefaultLangID(), csEntry);
		
	

		if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen)) 
			 	csRet = (char*)retbuf;
			
		
		
      }
    }

    UnlockResource( hGlobal );  
    FreeResource( hGlobal );  
  }

  return csRet;
}

/***********************************************************************************/
/*                                                                                 */
/* Class:   CGetVersionFunc                                                       */
/* Method:  FormatVersion                                                          */
/*                                                                                 */
/* Parameters:                                                                     */
/* -----------                                                                     */
/*   CString cs                                                                    */
/*                Specifies a version number such as "FileVersion" or              */
/*                "ProductVersion" in the format "m, n, o, p"                      */
/*                (e.g. "1, 2, 3, a")                                              */
/*                                                                                 */
/* Return Values:                                                                  */
/* --------------                                                                  */
/* If the function succeeds, the return value is a string containing the version   */
/* in the format "m.nop" (e.g. "1.23a")                                            */ 
/*                                                                                 */
/* If the function fails, the returned string is empty.                            */
/*                                                                                 */
/***********************************************************************************/
CString CGetVersionFunc::FormatVersion(CString cs)
{
  CString csRet;
  if (!cs.IsEmpty())
  {
    cs.TrimRight();

	char splitC = ',';
    int iPos = cs.Find(splitC);
    if (iPos == -1)
	{
		splitC = '.';
		iPos = cs.Find(splitC);
		if(iPos == -1)
		{
			return "";
		}
		
	}
      
    cs.TrimLeft();
    cs.TrimRight();
    csRet.Format("%s", cs.Left(iPos));

    while (1)
    {
      cs = cs.Mid(iPos + 1);
      cs.TrimLeft();
      iPos = cs.Find(splitC);
      if (iPos == -1)
      {
		 csRet +=".";
        csRet +=cs;
        break;
      }
	  csRet +=".";
      csRet += cs.Left(iPos);
    }
  }

  return csRet;
}

/***********************************************************************************/
/*                                                                                 */
/* Class:   CGetVersionFunc                                                       */
/* Method:  GetFileVersionX                                                        */
/*                                                                                 */
/* Parameters:                                                                     */
/* -----------                                                                     */
/*                                                                                 */
/* Return Values:                                                                  */
/* --------------                                                                  */
/* If the function succeeds, the return value is a string containing the           */
/* "FileVersion" in the format "m.nop" (e.g. "1.23a")                              */ 
/*                                                                                 */
/* If the function fails, the returned string is empty.                            */
/*                                                                                 */
/***********************************************************************************/
CString CGetVersionFunc::GetFileVersionX()
{
  if (m_csFileVersion.IsEmpty())
  {                                                         
    CString csVersion = FormatVersion(GetVersionInfo(NULL, "FILEVERSION"));

    m_csFileVersion.Format("%s", csVersion);
  }

  return m_csFileVersion;
}

void CGetVersionFunc::SetFileVersionEmpty()
{
	m_csFileVersion = "";
}


CString CGetVersionFunc::GetFileVersionXPath(CString exeFullName)
{
  if(m_csFileVersion.IsEmpty())
  {                                                         
		CString csVersion = FormatVersion(GetVersionInfoPath(exeFullName, "FILEVERSION"));

		m_csFileVersion.Format("%s", csVersion);
  }

  return m_csFileVersion;

}
/***********************************************************************************/
/*                                                                                 */
/* Class:   CGetVersionFunc                                                       */
/* Method:  GetFileVersionX                                                        */
/*                                                                                 */
/* Parameters:                                                                     */
/* -----------                                                                     */
/*                                                                                 */
/* Return Values:                                                                  */
/* --------------                                                                  */
/* If the function succeeds, the return value is a string containing the           */
/* "ProductVersion" in the format "m.nop" (e.g. "1.23a")                           */ 
/*                                                                                 */
/* If the function fails, the returned string is empty.                            */
/*                                                                                 */
/***********************************************************************************/
CString CGetVersionFunc::GetProductVersionX()
{
  if (m_csProductVersion.IsEmpty())
    m_csProductVersion = FormatVersion(GetVersionInfo(NULL, "ProductVersion"));

  return m_csProductVersion;
}