
// FaceMatch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFaceMatchApp: 
// �йش����ʵ�֣������ FaceMatch.cpp
//

class CFaceMatchApp : public CWinApp
{
public:
	CFaceMatchApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFaceMatchApp theApp;