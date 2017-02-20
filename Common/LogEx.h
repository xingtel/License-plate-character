#pragma once
#include <string>
#include <stdio.h>
class CLogEx
{
public:
	static void Loger(char * info);
	static void Release();
public:

	~CLogEx(void);

private:
	void WriteToFile(char * info);
	void CloseFile();
private:
	CLogEx(void);
	static CLogEx * m_instance;

	char m_szCurPath[256];
	
	FILE * m_file;
	char m_fileName[512];
};
