
// FaceMatchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plate_X.h"
#include "Plate_XDlg.h"
#include "afxdialogex.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\core.hpp>
#include <Common\CommonWin.h>
#include <plate_inter.h>
#include <Common\test.h>
#include <platex\config.h>
#include <vcshow.h>
#include "../caffeEx/main_inter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


//CPalteXDlg对话框

CPalteXDlg::CPalteXDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FACEMATCH_DIALOG, pParent)
{

}

void CPalteXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listPics);
}

BEGIN_MESSAGE_MAP(CPalteXDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_IMPORT_2, &CPalteXDlg::OnBnClickedSobel)
	ON_BN_CLICKED(IDC_BTN_SHOWV, &CPalteXDlg::OnBnClickedMserS)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SHOWHOG, &CPalteXDlg::OnBnClickedColor)


	ON_BN_CLICKED(IDC_BUTTON1, &CPalteXDlg::OnBnClickedFolder)
	ON_LBN_SELCHANGE(IDC_LIST1, &CPalteXDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BTN_LABEL, &CPalteXDlg::OnBnClickedBtnLabel)
	ON_BN_CLICKED(IDC_BTN_TRAINSTART, &CPalteXDlg::OnBnClickedBtnTrainstart)
END_MESSAGE_MAP()

using namespace cv;

void CPalteXDlg::FindFileJ(CString foldername, int index)
{

	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind, foldername);
	strcat(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	CStdioFile fileW;
	if (fileW.Open("./label.txt", CFile::modeReadWrite))
	{
		fileW.Seek(0, CFile::end);
	}
	else
	{
		fileW.Open("./label.txt", CFile::modeReadWrite | CFile::modeCreate);
	}
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{

			}
		}
		else
		{

			CString strTmp = foldername + "/";
			strTmp += FindFileData.cFileName;
			strTmp.Format("%s/%s %d", foldername, FindFileData.cFileName, index);

			if ((strTmp.Find(".jpg") != -1 || strTmp.Find(".bmp") != -1))
			{
				fileW.WriteString(strTmp);
				fileW.WriteString("\n");
			}
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}

	FindClose(hFind);


}

BOOL CPalteXDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//testmain1();

	SetDlgItemText(IDC_EDIT1, "3");

	m_backMat = Mat(842,1400,CV_8UC1);
	m_backMat.setTo(240);

	gShowWnd = this;




	plate_Init(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPalteXDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPalteXDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	
	}
	else
	{
		CDialogEx::OnPaint();
	}
}




void CPalteXDlg::OnBnClickedMserS()
{
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("JPG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		
		DrawThePicPoint(this, m_backMat, 0, 53, 1500);
	
		int t1 = GetTickCount();
		m_file2 = dlg.GetPathName(); //文件名保存在了FilePathName里	

		//platex::demo::test_plate_recognize(m_file2.GetBuffer(0), platex::PR_DETECT_SOBEL);


		plate_detect(m_file2.GetBuffer(0),
			platex::PR_DETECT_SOBEL| platex::PR_DETECT_CMSER);

	
		Mat in = imread(m_file2.GetBuffer(0),1);
		int t2 = GetTickCount();
		char szBuf[128];
		sprintf(szBuf, "Easypr1.5-X   sobel+mser 耗时%d，图片大小%d %d,%s", 
						t2 - t1, in.cols, in.rows, m_file2.GetBuffer(0));
		SetWindowText(szBuf);
		return;
	}
}

void CPalteXDlg::OnDestroy()
{
	__super::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}

void CPalteXDlg::OnBnClickedSobel()
{
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("JPG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		m_file2 = dlg.GetPathName(); //文件名保存在了FilePathName里	
		DrawThePicPoint(this, m_backMat, 0, 53, 1500);
		int t1 = GetTickCount();
		cv::Mat in = imread(m_file2.GetBuffer(0), 0);
		plate_recognize(m_file2.GetBuffer(0), platex::PR_DETECT_SOBEL);
		int t2 = GetTickCount();
		char szBuf[128];
		sprintf(szBuf, "Easypr1.5-X   sobel 耗时%d，图片大小%d %d %s", t2 - t1, in.cols, in.rows, dlg.GetFileName().GetBuffer(0));
		SetWindowText(szBuf);
		return;
	}

}




void CPalteXDlg::OnBnClickedColor()
{
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("JPG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		m_file2 = dlg.GetPathName(); //文件名保存在了FilePathName里	
		DrawThePicPoint(this, m_backMat, 0, 53, 1500);

		int t1 = GetTickCount();
		cv::Mat in = imread(m_file2.GetBuffer(0), 0);
		plate_recognize(m_file2.GetBuffer(0),
								platex::PR_DETECT_COLOR);
		int t2 = GetTickCount();
		char szBuf[128];
		sprintf(szBuf, "Easypr1.5-X   colsor 耗时%d，图片大小%d %d,%s", t2 - t1, in.cols, in.rows, dlg.GetFileName().GetBuffer(0));
		SetWindowText(szBuf);
		return;
	}

}







void CPalteXDlg::OnBnClickedFolder()
{
	// TODO: Add your control notification handler code here
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("请选择文件夹");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl == NULL)
	{
		return;
	}

	if (SHGetPathFromIDList(pidl, pszPath))
	{
		//AfxMessageBox(pszPath);
		std::vector<std::string> files;
		TraverseDir(pszPath, files);
		m_listPics.ResetContent();
		for (int i = 0; i<files.size(); ++i)
		{
			std::string fileName = files[i];
			transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
			int len = fileName.length();
			if (fileName.find(".png") == len - 4 || fileName.find(".jpg") == len - 4 || fileName.find(".bmp") == len - 4)
			{
				m_listPics.AddString(files[i].c_str());
			}

		}


	}
}


void CPalteXDlg::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listPics.GetCurSel();
	if (curSel >= 0)
	{

		CString   strText;
		m_listPics.GetText(curSel, strText);

		DrawThePicPoint(this, m_backMat, 0, 53, 1500);

		int t1 = GetTickCount();
		m_file2 = strText; //文件名保存在了FilePathName里	

		TH_PlateResult plateR[20];
		int recoNum;
		plate_reco(m_file2.GetBuffer(0), plateR,recoNum);

		Mat in = imread(m_file2.GetBuffer(0), 1);
		int t2 = GetTickCount();
		char szBuf[128];
		sprintf(szBuf, "Easypr1.5-X   sobel+mser 耗时%d，图片大小%d %d %s", 
						t2 - t1, in.cols, in.rows, m_file2.GetBuffer(0));
		SetWindowText(szBuf);

		for (int k = 0; k < recoNum; ++k)
		{
			DrawMyTextBK(CString(plateR[k].color) + plateR[k].license, 260 + 160 * k, 5, this, RGB(240, 240, 240), 20);
		}

		return;

	}
}


void CPalteXDlg::OnBnClickedBtnLabel()
{
	char szCurPath[MAX_PATH] = { 0 };
	char *pFind = NULL;
	GetModuleFileName(NULL, szCurPath, sizeof(szCurPath));
	//简单的 一个日志初始化
	//后面的每个网络单独初始化，因为加载模型的原因可能有点慢，实际检测速度都还可以的
	// TODO:  在此添加额外的初始化
	//google::InitGoogleLogging(szCurPath);
	//google::SetLogDestination(google::LogSeverity(), "./");

	//中国车牌
	const char strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', /* 没有I */
		'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	const int numCharacter = 34; /* 没有I和0,10个数字与24个英文字符之和 */

								 //以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
								 //有些后面加数字2的表示在训练时常看到字符的一种变形，也作为训练数据存储
	const std::string strChinese[] = {
		"zh_cuan" /* 川 */,  "zh_e" /* 鄂 */,    "zh_gan" /* 赣*/,
		"zh_gan1" /*甘*/,    "zh_gui" /* 贵 */,  "zh_gui1" /* 桂 */,
		"zh_hei" /* 黑 */,   "zh_hu" /* 沪 */,   "zh_ji" /* 冀 */,
		"zh_jin" /* 津 */,   "zh_jing" /* 京 */, "zh_jl" /* 吉 */,
		"zh_liao" /* 辽 */,  "zh_lu" /* 鲁 */,   "zh_meng" /* 蒙 */,
		"zh_min" /* 闽 */,   "zh_ning" /* 宁 */, "zh_qing" /* 青 */,
		"zh_qiong" /* 琼 */, "zh_shan" /* 陕 */, "zh_su" /* 苏 */,
		"zh_sx" /* 晋 */,    "zh_wan" /* 皖 */,  "zh_xiang" /* 湘 */,
		"zh_xin" /* 新 */,   "zh_yu" /* 豫 */,   "zh_yu1" /* 渝 */,
		"zh_yue" /* 粤 */,   "zh_yun" /* 云 */,  "zh_zang" /* 藏 */,
		"zh_zhe" /* 浙 */ };

	const int numChinese = 31;

	for (int k = 0; k < numCharacter; ++k)
	{
		char szBuf[256];
		sprintf(szBuf, "./resources/train/%c", strCharacters[k]);
		FindFileJ(szBuf, k);
	}

	for (int k = 0; k < numChinese; ++k)
	{
		char szBuf[256];
		sprintf(szBuf, "./resources/train/%s", strChinese[k].c_str());
		FindFileJ(szBuf, k+ numCharacter);
	}

}


void CPalteXDlg::OnBnClickedBtnTrainstart()
{

	char szCurPath[MAX_PATH] = { 0 };
	char *pFind = NULL;
	GetModuleFileName(NULL, szCurPath, sizeof(szCurPath));
	//简单的 一个日志初始化
	//后面的每个网络单独初始化，因为加载模型的原因可能有点慢，实际检测速度都还可以的
	// TODO:  在此添加额外的初始化
	/*google::InitGoogleLogging(szCurPath);
	google::SetLogDestination(google::LogSeverity(), "./");*/

	//char  cmds[10][256];
	char  *cmds[10];
	for (int k = 0; k < 10; ++k)
	{
		cmds[k] = new char[256];
	}
	//./build/tools/caffe test -model examples/mnist/lenet_train_test.prototxt 
	//   -weights examples/mnist/lenet_iter_10000.caffemodel -gpu 0 -iterations 100

	/*strcpy(cmds[0], "ex");
	strcpy(cmds[1], "test");
	strcpy(cmds[2], "-model");
	strcpy(cmds[3], "examples/mnist/lenet_train_test.prototxt");
	strcpy(cmds[4], " -weights");
	strcpy(cmds[5], "examples/mnist/lenet_iter_10000.caffemodel");
	strcpy(cmds[6], "-gpu");
	strcpy(cmds[7], "0");
	strcpy(cmds[8], "-iterations");
	strcpy(cmds[9], "100");*/
	//char * szCmd = "ex  train  --solver=examples/mnist/lenet_solver.prototxt";

	strcpy(cmds[0], "EasyPR_XL");
	strcpy(cmds[1], "train");
	strcpy(cmds[2], "--solver=examples/mnist/lenet_solver.prototxt");
	//strcpy(cmds[3], "-model");
	//strcpy(cmds[4], "examples/mnist/lenet_train_test.prototxt");
	//strcpy(cmds[5], " -weights");
	//strcpy(cmds[6], "examples/mnist/lenet_iter_10000.caffemodel");

	caffemain(3, cmds);

	//InitPlateX();


	/*strcpy(cmds[0], "ex");
	strcpy(cmds[1], "--shuffle");
	strcpy(cmds[2], "--resize_height=28");
	strcpy(cmds[3], "--resize_width=28");
	strcpy(cmds[4], "c:/opencv/bin");
	strcpy(cmds[5], "./label.txt");
	strcpy(cmds[6], "./img_train_leveldb");

	main_converimgeset(7, cmds);*/


	//caffemain(10, cmds);
	//char * szCmd = "ex  train  --solver=examples/mnist/lenet_solver.prototxt";


}
