// mousehook1.h: interface for the Cmousehook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSEHOOK1_H__E41F44E2_F4AF_4D78_A377_CA64D7DA429C__INCLUDED_)
#define AFX_MOUSEHOOK1_H__E41F44E2_F4AF_4D78_A377_CA64D7DA429C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WM_POINTS_XY WM_USER+3
#define WM_POINTS_UP WM_USER+1
class AFX_EXT_CLASS Cmousehook : public CObject  
{
public:
	Cmousehook();
	virtual ~Cmousehook();
	BOOL starthook(HWND hWnd);
	BOOL stophook();

};

#endif // !defined(AFX_MOUSEHOOK1_H__E41F44E2_F4AF_4D78_A377_CA64D7DA429C__INCLUDED_)
