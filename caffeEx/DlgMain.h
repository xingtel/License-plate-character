#pragma once


// CDlgMain 对话框

class CDlgMain : public CDialog
{
	DECLARE_DYNAMIC(CDlgMain)

public:
	CDlgMain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void FindFileJ(CString foldername,int index);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFivepts();
	afx_msg void OnBnClickedButtonMtcnn();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCmpIn1();
	afx_msg void OnBnClickedButtonPatchcnn();
	afx_msg void OnBnClickedButtonCaffeyolo();
	afx_msg void OnStnClickedStaticDeepland1();
	afx_msg void OnStnClickedStaticDeepland2();
	afx_msg void OnBnClickedButtonAgegender();
	afx_msg void OnBnClickedBtnRecochar();
};
