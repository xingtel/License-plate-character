// DlgMain.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgMain.h"
#include "afxdlgs.h"
#include "afxdialogex.h"
#include "../caffeEx/main_inter.h"
#include "../caffeEx/main_compare.h"
#include "resource.h"
#include <opencv2\imgcodecs.hpp>
#include <opencv2\highgui.hpp>
#include <time.h>


//CDlgMain 对话框
IMPLEMENT_DYNAMIC(CDlgMain, CDialog)

CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_MAIN, pParent)
{

}

CDlgMain::~CDlgMain()
{
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMain, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FIVEPTS, &CDlgMain::OnBnClickedButtonFivepts)
	ON_BN_CLICKED(IDC_BUTTON_MTCNN, &CDlgMain::OnBnClickedButtonMtcnn)
	ON_BN_CLICKED(IDC_BUTTON_CMP_IN1, &CDlgMain::OnBnClickedButtonCmpIn1)
	ON_BN_CLICKED(IDC_BUTTON_PATCHCNN, &CDlgMain::OnBnClickedButtonPatchcnn)
	ON_BN_CLICKED(IDC_BUTTON_CAFFEYOLO, &CDlgMain::OnBnClickedButtonCaffeyolo)
	ON_STN_CLICKED(IDC_STATIC_DEEPLAND1, &CDlgMain::OnStnClickedStaticDeepland1)
	ON_STN_CLICKED(IDC_STATIC_DEEPLAND2, &CDlgMain::OnStnClickedStaticDeepland2)
	ON_BN_CLICKED(IDC_BUTTON_AGEGENDER, &CDlgMain::OnBnClickedButtonAgegender)
	ON_BN_CLICKED(IDC_BTN_RECOCHAR, &CDlgMain::OnBnClickedBtnRecochar)
END_MESSAGE_MAP()


// CDlgMain 消息处理程序

BOOL CDlgMain::OnInitDialog()
{
	CDialog::OnInitDialog();
	//界面的初始化

	SetDlgItemText(IDC_STATIC_DEEPLAND1, "https://github.com/luoyetx/deep-landmark");
	SetDlgItemText(IDC_STATIC_DEEPLAND2, "https://github.com/luoyetx/mini-caffe-example");


	char szCurPath[MAX_PATH] = { 0 };
	char *pFind = NULL;
	GetModuleFileName(NULL, szCurPath, sizeof(szCurPath));
	//简单的 一个日志初始化
	//后面的每个网络单独初始化，因为加载模型的原因可能有点慢，实际检测速度都还可以的
	// TODO:  在此添加额外的初始化
	//google::InitGoogleLogging(szCurPath);
	//google::SetLogDestination(google::LogSeverity(), "./");

	caffe_InitMtcnn();



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

	/*for (int k = 0; k < numCharacter; ++k)
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
	}*/


	
	char * cmds[10];
	for (int i = 0; i < 10; ++i)
	{
		cmds[i] = new char[256];
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
	
	strcpy(cmds[0], "ex");
	strcpy(cmds[1], "train");
	strcpy(cmds[2], "--solver=examples/mnist/lenet_solver.prototxt");
	//caffemain(3, cmds);

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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgMain::OnBnClickedButtonFivepts()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
	
		CString strFile = fileDlg.GetPathName();//返回选择或输入的文件名称，
		//main_CNN5Align(strFile.GetBuffer(0));
	}
}


void CDlgMain::OnBnClickedButtonMtcnn()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
		CString strFile = fileDlg.GetPathName();//返回选择或输入的文件名称，

		cv::Mat img = cv::imread(strFile.GetBuffer(0));

		clock_t t1 = clock();
		int costTime = mainMtcnn(img);
		CString strInfo;
		clock_t t2 = clock();
		strInfo.Format("耗时 %d ms",t2-t1);

		cv::imshow("detect",img);
		SetWindowText(strInfo);
		
	}
}



CString file1;
CString file2;
void CDlgMain::OnBnClickedButtonCmpIn1()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{

		file1 = fileDlg.GetPathName();//返回选择或输入的文件名称，

	}
}


void CDlgMain::OnBnClickedButtonPatchcnn()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{

		file2 = fileDlg.GetPathName();//返回选择或输入的文件名称，

		if (!file1.IsEmpty() && !file2.IsEmpty())
		{
			S_MY_RECT r1; S_MY_RECT r2;

			r1.x = 240;
			r1.y = 230;
			r1.w = 120;
			r1.h = 50;

			r2.x = 240;
			r2.y = 230;
			r2.w = 120;
			r2.h = 50;

			float score = main_compareCaffe2(file1.GetBuffer(0),&r1,file2.GetBuffer(0),&r2);

			char szBuf[512];
			sprintf(szBuf,"比较结果 %f", score);
			SetDlgItemText(IDC_STATIC_FSCORE,szBuf);
		}
	}
}


void CDlgMain::OnBnClickedButtonCaffeyolo()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();//返回选择或输入的文件名称，
		//main_yolo(filename.GetBuffer(0));
	}
}


void CDlgMain::OnStnClickedStaticDeepland1()
{
	CString text;
	GetDlgItemText(IDC_STATIC_DEEPLAND1, text);
	ShellExecute(0, NULL, text, NULL, NULL, SW_NORMAL);
}


void CDlgMain::OnStnClickedStaticDeepland2()
{
	CString text;
	GetDlgItemText(IDC_STATIC_DEEPLAND2, text);
	ShellExecute(0, NULL, text, NULL, NULL, SW_NORMAL);
}


void CDlgMain::FindFileJ(CString foldername,int index)
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
		fileW.Open("./label.txt", CFile::modeReadWrite|CFile::modeCreate);
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
			strTmp.Format("%s/%s %d",foldername, FindFileData.cFileName,index);

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

void CDlgMain::OnBnClickedButtonAgegender()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();//返回选择或输入的文件名称，
		//main_ageGender(filename.GetBuffer(0));
	}
}


void CDlgMain::OnBnClickedBtnRecochar()
{
	CFileDialog fileDlg(TRUE,  // TRUE打开Open，FALSE保存Save As文件对话框
		".jpg",  // 默认的打开文件的类型
		"", // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		"jpg文件(*.jpg)|*.jpg|所有文件(*.*) |*.*||"  // 打开的文件类型
	);

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();//返回选择或输入的文件名称，

		std::string outstr;

		cv::Mat img = cv::imread(filename.GetBuffer(0),0);
		//plateX_recoMat(img, outstr,false);
		SetDlgItemText(IDC_STATIC_OUTSTR,outstr.c_str());
	}

}
