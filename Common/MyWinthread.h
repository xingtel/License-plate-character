#pragma once
/*
	windows下的线程类的封装 
	线程的启动和停止
	线程消息的处理

*/

#include <afxwin.h>         // MFC 核心组件和标准组件
class CMyWinthread
{
public:
	CMyWinthread(void);
	~CMyWinthread(void);

	//本线程处理完后,设置指定线程发送处理完消息
	void SetReceiveThreadID(DWORD receiveTheadID);
	//启动线程
	virtual void StartThreadEx();
	//停止线程
	virtual	void StopThreadEx();
	//获取线程ID
	DWORD GetThreadID(){return m_pWinmsgThead->m_nThreadID;}
	//是否可用
	virtual bool IsWorkNow();
	CWinThread *GetWinThread(){return m_pWinmsgThead;};
	void PostMsg(int msgID,void * pMsg);
	int MsgCount(){return m_iMsgCount;}

	
protected:
	virtual void DoMsg(MSG& msg) {};
	virtual void DoGeneralWork(){};
private:
	static unsigned int WinMsgThread(LPVOID pArg);
	virtual void ProcThreadExMsg();
protected:
	CWinThread * m_pWinmsgThead;
	DWORD m_receiveThreadID;
	bool m_bWinthreadExit;
	unsigned int m_iMsgCount;
};

