#ifndef EASYPR_CORE_CHARSIDENTIFYCNN_H_
#define EASYPR_CORE_CHARSIDENTIFYCNN_H_

#include <memory>
#include "opencv2/opencv.hpp"

#include "platex/util/kv.h"
#include "platex/core/character.hpp"
#include "thirdparty/CNN/cnn.hpp"

namespace platex {

class CharsIdentify_CNN {
 public:
  static CharsIdentify_CNN* instance();

  int classify(cv::Mat f, float& maxVal, bool isChinses = false);
  void classify(cv::Mat featureRows, std::vector<int>& out_maxIndexs,
    std::vector<float>& out_maxVals, std::vector<bool> isChineseVec);
  void classify(std::vector<CCharacter>& charVec);
  void classifyChinese(std::vector<CCharacter>& charVec);

  std::pair<std::string, std::string> identify(cv::Mat input, bool isChinese = false);
  int identify(std::vector<cv::Mat> inputs, std::vector<std::pair<std::string, std::string>>& outputs,
    std::vector<bool> isChineseVec);

  std::pair<std::string, std::string> identifyChinese(cv::Mat input, float& result, bool& isChinese);

  bool isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese = false);

  void LoadModel(std::string path);
  void LoadChineseModel(std::string path);

 private:
  CharsIdentify_CNN();

  static CharsIdentify_CNN* instance_;
  //cv::Ptr<cv::ml::ANN_MLP> ann_;
  //cv::Ptr<cv::ml::ANN_MLP> annChinese_;
  cv::Ptr<cv::ml::CNN_MLP> ann_;
  cv::Ptr<cv::ml::CNN_MLP> annChinese_;
  std::shared_ptr<Kv> kv_;
};
}

#endif  //  EASYPR_CORE_CHARSIDENTIFY_H_