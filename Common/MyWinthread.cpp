
#include "MyWinthread.h"


CMyWinthread::CMyWinthread(void)
{
	m_bWinthreadExit = TRUE;
	m_pWinmsgThead = NULL;
	m_iMsgCount = 0;
}


CMyWinthread::~CMyWinthread(void)
{

}

void CMyWinthread::StopThreadEx()
{
	m_bWinthreadExit = TRUE;
	if (m_pWinmsgThead)
	{
		PostThreadMessage(m_pWinmsgThead->m_nThreadID,WM_QUIT,0,0);
		DWORD exit = 0;
		TerminateThread(m_pWinmsgThead->m_hThread,exit);
		delete m_pWinmsgThead;
		m_pWinmsgThead = NULL;
	}
}
bool CMyWinthread::IsWorkNow()
{
	return !m_bWinthreadExit;
}
void CMyWinthread::PostMsg(int msgID,void * pMsg)
{
	PostThreadMessage(m_pWinmsgThead->m_nThreadID,msgID,
		(WPARAM)pMsg,NULL);
	m_iMsgCount++;
}

void CMyWinthread::SetReceiveThreadID(DWORD threadID)
{
	m_receiveThreadID = threadID;
}

void CMyWinthread::StartThreadEx()
{
	m_bWinthreadExit = FALSE;
	m_pWinmsgThead = AfxBeginThread(WinMsgThread,this,
							 THREAD_PRIORITY_NORMAL,
											 0,
											 CREATE_SUSPENDED);
		if (m_pWinmsgThead != NULL)
		{
			m_pWinmsgThead->m_bAutoDelete = FALSE;


			// start
			m_pWinmsgThead->ResumeThread();
		}
}

unsigned int CMyWinthread::WinMsgThread(PVOID pArg)
{
	CMyWinthread * pwinThread = (CMyWinthread*)pArg;
	pwinThread->ProcThreadExMsg();
	return 0;
}


void CMyWinthread::ProcThreadExMsg()
{


	while(!m_bWinthreadExit)
	{
		MSG Msg;
		if (PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
		{
			if (Msg.message == WM_QUIT)
			{
				break;
			}
			else if (Msg.message != 0)
			{
				DoMsg(Msg);
			}		
		}

		DoGeneralWork();
		Sleep(70);
	}
	m_bWinthreadExit = true;

}