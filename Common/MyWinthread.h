#pragma once
/*
	windows�µ��߳���ķ�װ 
	�̵߳�������ֹͣ
	�߳���Ϣ�Ĵ���

*/

#include <afxwin.h>         // MFC ��������ͱ�׼���
class CMyWinthread
{
public:
	CMyWinthread(void);
	~CMyWinthread(void);

	//���̴߳������,����ָ���̷߳��ʹ�������Ϣ
	void SetReceiveThreadID(DWORD receiveTheadID);
	//�����߳�
	virtual void StartThreadEx();
	//ֹͣ�߳�
	virtual	void StopThreadEx();
	//��ȡ�߳�ID
	DWORD GetThreadID(){return m_pWinmsgThead->m_nThreadID;}
	//�Ƿ����
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

