// GlobalFunctions.h: Schnittstelle f�r die Klasse CGetVersionFunc.
//
//////////////////////////////////////////////////////////////////////
#pragma once

/****************************************

CGetVersionFunc ��ȡexe�ļ��İ汾��




******************************************/
class CGetVersionFunc : public CObject  
{
public:
	CGetVersionFunc();
	virtual ~CGetVersionFunc();

public:
	static CString GetFileVersionX();
	static CString GetFileVersionXPath(CString exeFullName);
	static void SetFileVersionEmpty();
	static CString GetProductVersionX();
	static CString GetVersionInfo(HMODULE hLib, CString csEntry);
	static CString GetVersionInfoPath(CString exeFullName,CString csEntry);
	static CString FormatVersion(CString cs);

private:
	static CString m_csFileVersion;
	static CString m_csProductVersion;

};


