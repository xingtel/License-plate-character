// QR_Encode.h : CQR_Encode �N���X�錾����уC���^�[�t�F�C�X��`
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// �萔

// ����ˮƽ 
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// /����ģʽ 
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// �汾��ģ�ͣ��� 
#define QR_VRESION_S	0 // 1~9 
#define QR_VRESION_M	1 // 10 ~ 26
#define QR_VRESION_L	2 // 27 ~ 40

#define MAX_ALLCODEWORD	 3706 // ������������ 
#define MAX_DATACODEWORD 2956 // ����������֣��汾40 - L���� 
#define MAX_CODEBLOCK	  153 // ������� Burokkudetakodowado��RS���ֵȣ� 
#define MAX_MODULESIZE	  177 // ģ�����������ı� 

// ����λͼ�ڱ߼� 
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// ң�п� 
	int ncAllCodeWord;	// ������� 
	int ncDataCodeWord;	// �����������֣������Ĵ��� - RS��������


} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
//  QR��汾��ģ�ͣ�����Ϣ 

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // �汾���ͺţ���������1-40�� 
	int ncAllCodeWord; // ������������ 

	// ������һ������������������� (0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	//�������֣��������������� - RS�������� 

	int ncAlignPoint;	// ��������Araimentopatan 
	int nAlignPoint[6];	// ��������Araimentopatan 

	RS_BLOCKINFO RS_BlockInfo1[4]; // ң����Ϣ�飨1�� 
	RS_BLOCKINFO RS_BlockInfo2[4]; // ң�п���Ϣ��2�� 

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode �N���X

class CQR_Encode
{
// �\�z/����
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		//�������ˮƽ 
	int m_nVersion;		// �汾���ͺţ� 
	BOOL m_bAutoExtent;	// �汾���ͺţ���־ָ���Զ���չ 
	int m_nMaskingNo;	// /�ڱ�ģʽ������ 

public:
	int m_nSymbleSize;
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// Bit5������ģ�飨���������ڱΣ���־
	// Bit4����ʾ���ݵĹ���ģ��
	// 04��Bit1����������
	// Bit0������������ģͼ������
	//ͨ���߼��͹���ģ�飬11Сʱ���������߼������������յ��ж�20Сʱ�� 

private:
	int m_ncDataCodeWordBit; // ����Detakodowadobitto 
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // ����Detaenkodoeria


	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // �����������֣�ң�����ݣ��������������� 
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // / /������������ִ��� 
	BYTE m_byRSWork[MAX_CODEBLOCK]; // RS���ּ��㹤�� 

// ������ص����ݱ��� 
public:
	BOOL EncodeData(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, LPCSTR lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, LPCSTR lpsSource, int ncLength);
	BOOL EncodeSourceData(LPCSTR lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	BOOL IsNumeralData(unsigned char c);
	BOOL IsAlphabetData(unsigned char c);
	BOOL IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

// ����ģ����صİ��� 
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
