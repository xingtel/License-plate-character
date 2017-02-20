#pragma once
/*
	本类提供 程序运行过程中 崩溃后的 崩溃位置截获功能
	来源于电驴

	整理者：sunjunlishi
	公司：广州魔镜科技有限公司
*/
struct _EXCEPTION_POINTERS;

class CMiniDumper
{
public:
	static void Enable(LPCTSTR pszAppName, bool bShowErrors, LPCTSTR pszDumpDir);

private:
	static TCHAR m_szAppName[MAX_PATH];
	static TCHAR m_szDumpDir[MAX_PATH];

	static HMODULE GetDebugHelperDll(FARPROC* ppfnMiniDumpWriteDump, bool bShowErrors);
	static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);
};

extern CMiniDumper theCrashDumper;
