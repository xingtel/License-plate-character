#include "platex/core/chars_identify_cnn.h"
#include "platex/core/character.hpp"
#include "platex/config.h"
#include "platex/core/core_func.h"
#include "platex/core/feature.h"
#include "platex/core/params.h"

namespace platex {

  CharsIdentify_CNN* CharsIdentify_CNN::instance_ = nullptr;

  CharsIdentify_CNN* CharsIdentify_CNN::instance() {
    if (!instance_) {
      instance_ = new CharsIdentify_CNN;
    }
    return instance_;
  }

  CharsIdentify_CNN::CharsIdentify_CNN() {
    //ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(kDefaultAnnPath);
    //annChinese_ = ml::ANN_MLP::load<ml::ANN_MLP>(kChineseAnnPath);
	  ann_ = ml::CNN_MLP::load<ml::CNN_MLP>("en-cnn.dat");
	  annChinese_ = ml::CNN_MLP::load<ml::CNN_MLP>("zh-cnn.dat");
    kv_ = std::shared_ptr<Kv>(new Kv);
    kv_->load("etc/province_mapping");
  }

  void CharsIdentify_CNN::LoadModel(std::string path) {
    if (path != std::string(kDefaultAnnPath)) {

      if (!ann_->empty())
        ann_->clear();

      //ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(path);
	  ann_ = ml::CNN_MLP::load<ml::CNN_MLP>("en-cnn.dat");
    }
  }

  void CharsIdentify_CNN::LoadChineseModel(std::string path) {
    if (path != std::string(kChineseAnnPath)) {

      if (!annChinese_->empty())
        annChinese_->clear();

      //annChinese_ = ml::ANN_MLP::load<ml::ANN_MLP>(path);
	  annChinese_ = ml::CNN_MLP::load<ml::CNN_MLP>("zh-cnn.dat");

    }
  }

  void CharsIdentify_CNN::classify(cv::Mat featureRows, std::vector<int>& out_maxIndexs, 
    std::vector<float>& out_maxVals, std::vector<bool> isChineseVec){
    int rowNum = featureRows.rows;

    cv::Mat output(rowNum, kCharsTotalNumber, CV_32FC1);
    ann_->predict(featureRows, output);

    for (int output_index = 0; output_index < rowNum; output_index++) {
      Mat output_row = output.row(output_index);
      int result = -1;
      float maxVal = -2.f;
      bool isChinses = isChineseVec[output_index];
      if (!isChinses) {
        result = 0;
        for (int j = 0; j < kCharactersNumber; j++) {
          float val = output_row.at<float>(j);
          // std::cout << "j:" << j << "val:" << val << std::endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
      } 
      else {
        result = kCharactersNumber;
        for (int j = kCharactersNumber; j < kCharsTotalNumber; j++) {
          float val = output_row.at<float>(j);
          //std::cout << "j:" << j << "val:" << val << std::endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
      }
      out_maxIndexs[output_index] = result;
      out_maxVals[output_index] = maxVal;
    }
  }


  void CharsIdentify_CNN::classify(std::vector<CCharacter>& charVec){
    size_t charVecSize = charVec.size();

    if (charVecSize == 0)
      return;

    Mat featureRows;
    for (size_t index = 0; index < charVecSize; index++) {
      Mat charInput = charVec[index].getCharacterMat();
      Mat feature = charFeatures(charInput, kPredictSize);
      featureRows.push_back(feature);
    }

    cv::Mat output(charVecSize, kCharsTotalNumber, CV_32FC1);
    ann_->predict(featureRows, output);

    for (size_t output_index = 0; output_index < charVecSize; output_index++) {
      CCharacter& character = charVec[output_index];
      Mat output_row = output.row(output_index);

      int result = -1;
      float maxVal = -2.f;
      std::string label = "";

      bool isChinses = character.getIsChinese();
      if (!isChinses) {
        result = 0;
        for (int j = 0; j < kCharactersNumber; j++) {
          float val = output_row.at<float>(j);
          //std::cout << "j:" << j << "val:" << val << std::endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
        label = std::make_pair(kChars[result], kChars[result]).second;
      }
      else {
        result = kCharactersNumber;
        for (int j = kCharactersNumber; j < kCharsTotalNumber; j++) {
          float val = output_row.at<float>(j);
          //std::cout << "j:" << j << "val:" << val << std::endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
        const char* key = kChars[result];
        std::string s = key;
        std::string province = kv_->get(s);
        label = std::make_pair(s, province).second;
      }
      /*std::cout << "result:" << result << std::endl;
      std::cout << "maxVal:" << maxVal << std::endl;*/
      character.setCharacterScore(maxVal);
      character.setCharacterStr(label);
    }
  }


  void CharsIdentify_CNN::classifyChinese(std::vector<CCharacter>& charVec){
    size_t charVecSize = charVec.size();

    if (charVecSize == 0)
      return;

    Mat featureRows;
    for (size_t index = 0; index < charVecSize; index++) {
      Mat charInput = charVec[index].getCharacterMat();
      Mat feature = charFeatures(charInput, kChineseSize);
      featureRows.push_back(feature);
    }

    cv::Mat output(charVecSize, kChineseNumber, CV_32FC1);
    annChinese_->predict(featureRows, output);

    for (size_t output_index = 0; output_index < charVecSize; output_index++) {
      CCharacter& character = charVec[output_index];
      Mat output_row = output.row(output_index);
      bool isChinese = true;

      float maxVal = -2;
      int result = -1;

      for (int j = 0; j < kChineseNumber; j++) {
        float val = output_row.at<float>(j);
        //std::cout << "j:" << j << "val:" << val << std::endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }

      // no match
      if (-1 == result) {
        result = 0;
        maxVal = 0;
        isChinese = false;
      }

      auto index = result + kCharsTotalNumber - kChineseNumber;
      const char* key = kChars[index];
      std::string s = key;
      std::string province = kv_->get(s);

      /*std::cout << "result:" << result << std::endl;
      std::cout << "maxVal:" << maxVal << std::endl;*/

      character.setCharacterScore(maxVal);
      character.setCharacterStr(province);
      character.setIsChinese(isChinese);
    }
  }

  int CharsIdentify_CNN::classify(cv::Mat f, float& maxVal, bool isChinses){
    int result = -1;

    cv::Mat output(1, kCharsTotalNumber, CV_32FC1);
    ann_->predict(f, output);

    maxVal = -2.f;
    if (!isChinses) {
      result = 0;
      for (int j = 0; j < kCharactersNumber; j++) {
        float val = output.at<float>(j);
        // std::cout << "j:" << j << "val:" << val << std::endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }
    }
    else {
      result = kCharactersNumber;
      for (int j = kCharactersNumber; j < kCharsTotalNumber; j++) {
        float val = output.at<float>(j);
        //std::cout << "j:" << j << "val:" << val << std::endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }
    }
    //std::cout << "maxVal:" << maxVal << std::endl;
    return result;
  }

  bool CharsIdentify_CNN::isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese) {
    cv::Mat feature = charFeatures(input, kPredictSize);
    auto index = static_cast<int>(classify(feature, maxVal, isChinese));

    if (isChinese) {
      //std::cout << "maxVal:" << maxVal << std::endl;
    }


    float chineseMaxThresh = 0.2f;
    //float chineseMaxThresh = CParams::instance()->getParam2f();

    if (maxVal >= 0.9 || (isChinese && maxVal >= chineseMaxThresh)) {
      if (index < kCharactersNumber) {
        label = std::make_pair(kChars[index], kChars[index]).second;
      }
      else {
        const char* key = kChars[index];
        std::string s = key;
        std::string province = kv_->get(s);
        label = std::make_pair(s, province).second;
      }
      return true;
    }
    else
      return false;
  }

  /*bool CharsIdentify_CNN::charsJudge(std::vector<CCharacter>& charVec) {
    cv::Mat feature = charFeatures(input, kPredictSize);
    auto index = static_cast<int>(classify(feature, maxVal, isChinese));

    if (isChinese)
      std::cout << "maxVal:" << maxVal << std::endl;

    if (maxVal >= 0.9) {
      if (index < kCharactersNumber) {
        label = std::make_pair(kChars[index], kChars[index]).second;
      }
      else {
        const char* key = kChars[index];
        std::string s = key;
        std::string province = kv_->get(s);
        label = std::make_pair(s, province).second;
      }
      return true;
    }
    else
      return false;
  }*/

  std::pair<std::string, std::string> CharsIdentify_CNN::identifyChinese(cv::Mat input, float& out, bool& isChinese) {
    cv::Mat feature = charFeatures(input, kChineseSize);
    float maxVal = -2;

    int result = -1;

    cv::Mat output(1, kChineseNumber, CV_32FC1);
    annChinese_->predict(feature, output);

    for (int j = 0; j < kChineseNumber; j++) {
      float val = output.at<float>(j);
      //std::cout << "j:" << j << "val:" << val << std::endl;
      if (val > maxVal) {
        maxVal = val;
        result = j;
      }
    }

    // no match
    if (-1 == result) {
      result = 0;
      maxVal = 0;
      isChinese = false;
    }
    else if (maxVal > 0.9){
      isChinese = true;
    }

    auto index = result + kCharsTotalNumber - kChineseNumber;
    const char* key = kChars[index];
    std::string s = key;
    std::string province = kv_->get(s);

    out = maxVal;

    return std::make_pair(s, province);
  }


  std::pair<std::string, std::string> CharsIdentify_CNN::identify(cv::Mat input, bool isChinese) {
    cv::Mat feature = charFeatures(input, kPredictSize);
    float maxVal = -2;
    auto index = static_cast<int>(classify(feature, maxVal, isChinese));
    if (index < kCharactersNumber) {
      return std::make_pair(kChars[index], kChars[index]);
    }
    else {
      const char* key = kChars[index];
      std::string s = key;
      std::string province = kv_->get(s);
      return std::make_pair(s, province);
    }
  }

  int CharsIdentify_CNN::identify(std::vector<cv::Mat> inputs, std::vector<std::pair<std::string, std::string>>& outputs,
    std::vector<bool> isChineseVec) {
    Mat featureRows;
    size_t input_size = inputs.size();
    for (size_t i = 0; i < input_size; i++) {
      Mat input = inputs[i];
      cv::Mat feature = charFeatures(input, kPredictSize);
      featureRows.push_back(feature);
    }

    std::vector<int> maxIndexs;
    std::vector<float> maxVals;
    classify(featureRows, maxIndexs, maxVals, isChineseVec);

    for (size_t row_index = 0; row_index < input_size; row_index++) {
      int index = maxIndexs[row_index];
      if (index < kCharactersNumber) {
        outputs[row_index] = std::make_pair(kChars[index], kChars[index]);
      }
      else {
        const char* key = kChars[index];
        std::string s = key;
        std::string province = kv_->get(s);
        outputs[row_index] = std::make_pair(s, province);
      }
    }
    return 0;
  }
}
