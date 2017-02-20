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

//中国车牌
const char strCharacters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F', 'G',
                              'H', /* 没有I */
                              'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q',
                              'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const int numCharacter = 34; /* 没有I和0,10个数字与24个英文字符之和 */

//以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
//有些后面加数字2的表示在训练时常看到字符的一种变形，也作为训练数据存储
const string strChinese[] = {
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
    "zh_zhe" /* 浙 */};

const int numChinese = 31;
const int numAll = 65; /* 34+20=54 */

CCharsIdentify_ANN::CCharsIdentify_ANN() {
  // cout << "CCharsIdentify_ANN" << endl;
  m_predictSize = 10;
  m_path = "resources/model/ann_ann.xml";
  LoadModel();

  typedef pair<string, string> CodeProvince;

  if (m_map.empty()) {
    m_map.insert(CodeProvince("zh_cuan", "川"));
    m_map.insert(CodeProvince("zh_e", "鄂"));
    m_map.insert(CodeProvince("zh_gan", "赣"));
    m_map.insert(CodeProvince("zh_gan1", "甘"));
    m_map.insert(CodeProvince("zh_gui", "贵"));
    m_map.insert(CodeProvince("zh_gui1", "桂"));
    m_map.insert(CodeProvince("zh_hei", "黑"));
    m_map.insert(CodeProvince("zh_hu", "沪"));
    m_map.insert(CodeProvince("zh_ji", "冀"));
    m_map.insert(CodeProvince("zh_jin", "津"));
    m_map.insert(CodeProvince("zh_jing", "京"));
    m_map.insert(CodeProvince("zh_jl", "吉"));
    m_map.insert(CodeProvince("zh_liao", "辽"));
    m_map.insert(CodeProvince("zh_lu", "鲁"));
    m_map.insert(CodeProvince("zh_meng", "蒙"));
    m_map.insert(CodeProvince("zh_min", "闽"));
    m_map.insert(CodeProvince("zh_ning", "宁"));
    m_map.insert(CodeProvince("zh_qing", "青"));
    m_map.insert(CodeProvince("zh_qiong", "琼"));
    m_map.insert(CodeProvince("zh_shan", "陕"));
    m_map.insert(CodeProvince("zh_su", "苏"));
    m_map.insert(CodeProvince("zh_sx", "晋"));
    m_map.insert(CodeProvince("zh_wan", "皖"));
    m_map.insert(CodeProvince("zh_xiang", "湘"));
    m_map.insert(CodeProvince("zh_xin", "新"));
    m_map.insert(CodeProvince("zh_yu", "豫"));
    m_map.insert(CodeProvince("zh_yu1", "渝"));
    m_map.insert(CodeProvince("zh_yue", "粤"));
    m_map.insert(CodeProvince("zh_yun", "云"));
    m_map.insert(CodeProvince("zh_zang", "藏"));
    m_map.insert(CodeProvince("zh_zhe", "浙"));
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
  ann->predict(f, output);  //使用ann对字符做判断

  if (!isChinses)  //对中文字符的判断
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
            maxVal)  //求得中文字符权重最大的那个，也就是通过ann认为最可能的字符
        {
          maxVal = val;
          result = j;
        }
      }
    }
  } else  //对数字和英文字母的判断
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
  ann->predict(f, output);  //使用ann对字符做判断

  float maxVal = -2;
  for (int j = 0; j < numCharacter; j++) {
    float val = output.at<float>(j);
    // cout << "j:" << j << "val:"<< val << endl;
    if (val > maxVal)
    //求得中文字符权重最大的那个，也就是通过ann认为最可能的字符
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
	//抠取中间区域
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
	// feature,ANN的样本特征为水平、垂直直方图和低分辨率图像所组成的矢量
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
  int index = classify(f);  //使用ann来判别那个字符

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

//输入当个字符Mat,生成字符的string
string CCharsIdentify_ANN::charsIdentify(Mat input, bool isChinese, bool isSpeci) {
  Mat f = charFeatures(input, m_predictSize);

  string result = "";
  int index = classify(f, isChinese, isSpeci);  //使用ann来判别那个字符

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
