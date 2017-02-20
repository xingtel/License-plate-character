
// FaceMatchDlg.h : ͷ�ļ�
//

#pragma once

#include <opencv2\core.hpp>
#include <afxdialogex.h>
#include "resource.h"
#include "afxwin.h"
/*
    ���� ������ʾ��

	��Ȩ�����������Ƽ�.����

*/
// CPalteXDlg �Ի���
class CPalteXDlg : public CDialogEx
{
// ����
public:
	CPalteXDlg(CWnd* pParent = NULL);	// ��׼���캯��


	enum { IDD = IDD_FACEMATCH_DIALOG };


	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	cv::Mat m_backMat;
	CString m_file2;

	// ���ɵ���Ϣӳ�亯��
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
