//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    xinglian
// Copyright:   xinglian
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Desciption:
// Defines CCharsIdentify_ANN
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFYANN_H__
#define __CHARS_IDENTIFYANN_H__

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*! \namespace platex
    Namespace where all the C++ EasyPR functionality resides
*/
namespace platex {

class CCharsIdentify_ANN {
 public:

	 static CCharsIdentify_ANN * instance();
 

  //! 字符分割
  string charsIdentify(Mat, bool, bool);
  string charsIdentify(Mat input);

  //! 字符分类
  int classify(Mat, bool, bool);
  int classify(Mat f);

  //! 装载ANN模型
  void LoadModel();

  //! 装载ANN模型
  void LoadModel(string s);

  //! 设置与读取模型路径
  inline void setModelPath(string path) { m_path = path; }
  inline string getModelPath() const { return m_path; }

 private:
	 static  CCharsIdentify_ANN * m_instance;
	 CCharsIdentify_ANN();
  //！使用的ANN模型
  cv::Ptr<cv::ml::ANN_MLP>  ann;

  //! 模型存储路径
  string m_path;

  //! 特征尺寸
  int m_predictSize;

  //! 省份对应map
  std::map<string, string> m_map;
};

} /* \namespace platex  */

#endif /* endif __CHARS_IDENTIFY_H__ */