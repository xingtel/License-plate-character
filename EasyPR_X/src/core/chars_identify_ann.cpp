#include "platex/core/chars_identify_Ann.h"
#include "platex/core/core_func.h"
#include "platex/core/feature.h"
/*! \namespace platex
        Namespace where all the C++ EasyPR functionality resides
        */
namespace platex {

	CCharsIdentify_ANN * CCharsIdentify_ANN::m_instance = NULL;

	CCharsIdentify_ANN * CCharsIdentify_ANN::instance()
	{
		if (m_instance == NULL)
		{
			m_instance = new CCharsIdentify_ANN();
		}
		return m_instance;
	}

//�й�����
const char strCharacters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F', 'G',
                              'H', /* û��I */
                              'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q',
                              'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const int numCharacter = 34; /* û��I��0,10��������24��Ӣ���ַ�֮�� */

//���¶�����ѵ��ʱ�õ��������ַ����ݣ�����ȫ�棬��Щʡ��û��ѵ����������û���ַ�
//��Щ���������2�ı�ʾ��ѵ��ʱ�������ַ���һ�ֱ��Σ�Ҳ��Ϊѵ�����ݴ洢
const string strChinese[] = {
    "zh_cuan" /* �� */,  "zh_e" /* �� */,    "zh_gan" /* ��*/,
    "zh_gan1" /*��*/,    "zh_gui" /* �� */,  "zh_gui1" /* �� */,
    "zh_hei" /* �� */,   "zh_hu" /* �� */,   "zh_ji" /* �� */,
    "zh_jin" /* �� */,   "zh_jing" /* �� */, "zh_jl" /* �� */,
    "zh_liao" /* �� */,  "zh_lu" /* ³ */,   "zh_meng" /* �� */,
    "zh_min" /* �� */,   "zh_ning" /* �� */, "zh_qing" /* �� */,
    "zh_qiong" /* �� */, "zh_shan" /* �� */, "zh_su" /* �� */,
    "zh_sx" /* �� */,    "zh_wan" /* �� */,  "zh_xiang" /* �� */,
    "zh_xin" /* �� */,   "zh_yu" /* ԥ */,   "zh_yu1" /* �� */,
    "zh_yue" /* �� */,   "zh_yun" /* �� */,  "zh_zang" /* �� */,
    "zh_zhe" /* �� */};

const int numChinese = 31;
const int numAll = 65; /* 34+20=54 */

CCharsIdentify_ANN::CCharsIdentify_ANN() {
  // cout << "CCharsIdentify_ANN" << endl;
  m_predictSize = 10;
  m_path = "resources/model/ann_ann.xml";
  LoadModel();

  typedef pair<string, string> CodeProvince;

  if (m_map.empty()) {
    m_map.insert(CodeProvince("zh_cuan", "��"));
    m_map.insert(CodeProvince("zh_e", "��"));
    m_map.insert(CodeProvince("zh_gan", "��"));
    m_map.insert(CodeProvince("zh_gan1", "��"));
    m_map.insert(CodeProvince("zh_gui", "��"));
    m_map.insert(CodeProvince("zh_gui1", "��"));
    m_map.insert(CodeProvince("zh_hei", "��"));
    m_map.insert(CodeProvince("zh_hu", "��"));
    m_map.insert(CodeProvince("zh_ji", "��"));
    m_map.insert(CodeProvince("zh_jin", "��"));
    m_map.insert(CodeProvince("zh_jing", "��"));
    m_map.insert(CodeProvince("zh_jl", "��"));
    m_map.insert(CodeProvince("zh_liao", "��"));
    m_map.insert(CodeProvince("zh_lu", "³"));
    m_map.insert(CodeProvince("zh_meng", "��"));
    m_map.insert(CodeProvince("zh_min", "��"));
    m_map.insert(CodeProvince("zh_ning", "��"));
    m_map.insert(CodeProvince("zh_qing", "��"));
    m_map.insert(CodeProvince("zh_qiong", "��"));
    m_map.insert(CodeProvince("zh_shan", "��"));
    m_map.insert(CodeProvince("zh_su", "��"));
    m_map.insert(CodeProvince("zh_sx", "��"));
    m_map.insert(CodeProvince("zh_wan", "��"));
    m_map.insert(CodeProvince("zh_xiang", "��"));
    m_map.insert(CodeProvince("zh_xin", "��"));
    m_map.insert(CodeProvince("zh_yu", "ԥ"));
    m_map.insert(CodeProvince("zh_yu1", "��"));
    m_map.insert(CodeProvince("zh_yue", "��"));
    m_map.insert(CodeProvince("zh_yun", "��"));
    m_map.insert(CodeProvince("zh_zang", "��"));
    m_map.insert(CodeProvince("zh_zhe", "��"));
  }
}

void CCharsIdentify_ANN::LoadModel() {


	//if (!ann->empty())
	//	ann->clear();
	ann = ml::ANN_MLP::load<ml::ANN_MLP>(m_path.c_str());


}

void CCharsIdentify_ANN::LoadModel(string s) {
	if (!ann->empty())
		ann->clear();
	ann = ml::ANN_MLP::load<ml::ANN_MLP>(s.c_str());
}

int CCharsIdentify_ANN::classify(Mat f, bool isChinses, bool isSpeci) {
  int result = -1;
  Mat output(1, numAll, CV_32FC1);
  ann->predict(f, output);  //ʹ��ann���ַ����ж�

  if (!isChinses)  //�������ַ����ж�
  {
    if (isSpeci) {
      result = 0;
      float maxVal = -2;
      for (int j = 10; j < numCharacter; j++) {
        float val = output.at<float>(j);
        // cout << "j:" << j << "val:"<< val << endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }

    } else {
      result = 0;
      float maxVal = -2;
      for (int j = 0; j < numCharacter; j++) {
        float val = output.at<float>(j);
        // cout << "j:" << j << "val:"<< val << endl;
        if (val >
            maxVal)  //��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
        {
          maxVal = val;
          result = j;
        }
      }
    }
  } else  //�����ֺ�Ӣ����ĸ���ж�
  {
    result = numCharacter;
    float maxVal = -2;
    for (int j = numCharacter; j < numAll; j++) {
      float val = output.at<float>(j);
      // cout << "j:" << j << "val:"<< val << endl;
      if (val > maxVal) {
        maxVal = val;
        result = j;
      }
    }
  }
  return result;
}

int CCharsIdentify_ANN::classify(Mat f) {
  int result = -1;
  Mat output(1, numAll, CV_32FC1);
  ann->predict(f, output);  //ʹ��ann���ַ����ж�

  float maxVal = -2;
  for (int j = 0; j < numCharacter; j++) {
    float val = output.at<float>(j);
    // cout << "j:" << j << "val:"<< val << endl;
    if (val > maxVal)
    //��������ַ�Ȩ�������Ǹ���Ҳ����ͨ��ann��Ϊ����ܵ��ַ�
    {
      maxVal = val;
      result = j;
    }
  }

  for (int j = numCharacter; j < numAll; j++) {
    float val = output.at<float>(j);
    // cout << "j:" << j << "val:"<< val << endl;
    if (val > maxVal) {
      maxVal = val;
      result = j;
    }
  }

  return result;
}
#define HORIZONTAL 1
#define VERTICAL 0


Mat features(Mat in, int sizeData) {
	//��ȡ�м�����
	Rect _rect = GetCenterRect(in);

	Mat tmpIn = CutTheRect(in, _rect);
	// Mat tmpIn = in.clone();
	// Low data feature
	Mat lowData;
	resize(tmpIn, lowData, Size(sizeData, sizeData));

	// Histogram features
	Mat vhist = ProjectedHistogram(lowData, VERTICAL);
	Mat hhist = ProjectedHistogram(lowData, HORIZONTAL);

	// Last 10 is the number of moments components
	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

	Mat out = Mat::zeros(1, numCols, CV_32F);
	// Asign values to
	// feature,ANN����������Ϊˮƽ����ֱֱ��ͼ�͵ͷֱ���ͼ������ɵ�ʸ��
	int j = 0;
	for (int i = 0; i < vhist.cols; i++) {
		out.at<float>(j) = vhist.at<float>(i);
		j++;
	}
	for (int i = 0; i < hhist.cols; i++) {
		out.at<float>(j) = hhist.at<float>(i);
		j++;
	}
	for (int x = 0; x < lowData.cols; x++) {
		for (int y = 0; y < lowData.rows; y++) {
			out.at<float>(j) += (float)lowData.at<unsigned char>(x, y);
			j++;
		}
	}
	// if(DEBUG)
	//	cout << out << "\n===========================================\n";
	return out;
}



string CCharsIdentify_ANN::charsIdentify(Mat input) {
  Mat f = features(input, m_predictSize);//
  string result = "";
  int index = classify(f);  //ʹ��ann���б��Ǹ��ַ�

  if (index >= numCharacter) {
    string s = strChinese[index - numCharacter];
    string province = m_map[s];
    return s;
  } else {
    char s = strCharacters[index];
    char szBuf[216];
    sprintf(szBuf, "%c", s);
    return szBuf;
  }
}

//���뵱���ַ�Mat,�����ַ���string
string CCharsIdentify_ANN::charsIdentify(Mat input, bool isChinese, bool isSpeci) {
  Mat f = charFeatures(input, m_predictSize);

  string result = "";
  int index = classify(f, isChinese, isSpeci);  //ʹ��ann���б��Ǹ��ַ�

  if (!isChinese) {
    result = result + strCharacters[index];
  } else {
    string s = strChinese[index - numCharacter];
    string province = m_map[s];
    result = province + result;
  }

  return result;
}

} /*! \namespace platex*/
