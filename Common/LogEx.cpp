
#include "LogEx.h"
#include <afxwin.h>
CLogEx * CLogEx::m_instance = NULL;
CLogEx::CLogEx(void)
{
	m_file = NULL;

	//获得执行文件的路
	memset(m_szCurPath,0,sizeof(m_szCurPath));
	strcpy(m_szCurPath,"./");
	char *pFind = NULL;
	//GetModuleFileName(NULL, m_szCurPath, sizeof(m_szCurPath));
	//pFind = strrchr(m_szCurPath, '\\');
	//memset((void *)pFind, 0x00, strlen(pFind));

	CString logPath;
	logPath.Format("%s/log",m_szCurPath);
	if (!PathFileExists(logPath))
	{
		CreateDirectory(logPath,NULL);
	}
}

CLogEx::~CLogEx(void)
{

}

void CLogEx::Loger(char* info)
{
	if (m_instance == NULL)
	{
		m_instance = new CLogEx();
	}

	m_instance->WriteToFile(info);
}

void CLogEx::Release()
{
	if (m_instance)
	{
		m_instance->CloseFile();
		delete m_instance;
		m_instance = NULL;
	}
}

void CLogEx::CloseFile()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

void CLogEx::WriteToFile(char * info)
{
	if(m_file == NULL)
	{
		CTime timeNow = CTime::GetCurrentTime();
		char strTime[128];
		sprintf(strTime,"%d-%02d-%02d %02d",
			timeNow.GetYear(),timeNow.GetMonth(),timeNow.GetDay(),timeNow.GetHour());
		sprintf(m_fileName,"%s/log/%s.txt",m_szCurPath,strTime);
		m_file = fopen(m_fileName,"at");
	}

	//判断和当前的时间是否一致
	CTime timeNow = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format("%d-%02d-%02d %02d",
		timeNow.GetYear(),timeNow.GetMonth(),timeNow.GetDay(),timeNow.GetHour());
	
	std::string strname = m_fileName;
	if (strname.find(strTime) == -1)
	{
		sprintf(m_fileName,"%s/log/%s.txt",m_szCurPath,strTime);

		fclose(m_file);
		m_file = fopen(m_fileName,"at");
	}
	//往里面写数据
    fwrite(info,1,strlen(info),m_file);
	fwrite("\r\n",1,2,m_file);
}
