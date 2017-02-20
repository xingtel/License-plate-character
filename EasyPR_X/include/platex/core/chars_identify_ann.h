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
 

  //! �ַ��ָ�
  string charsIdentify(Mat, bool, bool);
  string charsIdentify(Mat input);

  //! �ַ�����
  int classify(Mat, bool, bool);
  int classify(Mat f);

  //! װ��ANNģ��
  void LoadModel();

  //! װ��ANNģ��
  void LoadModel(string s);

  //! �������ȡģ��·��
  inline void setModelPath(string path) { m_path = path; }
  inline string getModelPath() const { return m_path; }

 private:
	 static  CCharsIdentify_ANN * m_instance;
	 CCharsIdentify_ANN();
  //��ʹ�õ�ANNģ��
  cv::Ptr<cv::ml::ANN_MLP>  ann;

  //! ģ�ʹ洢·��
  string m_path;

  //! �����ߴ�
  int m_predictSize;

  //! ʡ�ݶ�Ӧmap
  std::map<string, string> m_map;
};

} /* \namespace platex  */

#endif /* endif __CHARS_IDENTIFY_H__ */