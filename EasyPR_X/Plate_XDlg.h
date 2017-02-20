
// FaceMatchDlg.h : 头文件
//

#pragma once

#include <opencv2\core.hpp>
#include <afxdialogex.h>
#include "resource.h"
#include "afxwin.h"
/*
    功能 界面显示类

	版权：厦门兴联科技.深圳

*/
// CPalteXDlg 对话框
class CPalteXDlg : public CDialogEx
{
// 构造
public:
	CPalteXDlg(CWnd* pParent = NULL);	// 标准构造函数


	enum { IDD = IDD_FACEMATCH_DIALOG };


	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	cv::Mat m_backMat;
	CString m_file2;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	void FindFileJ(CString foldername, int index);
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedSobel();
	afx_msg void OnBnClickedMserS();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedColor();
	CListBox m_listPics;
	afx_msg void OnBnClickedFolder();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedBtnLabel();
	afx_msg void OnBnClickedBtnTrainstart();
};
