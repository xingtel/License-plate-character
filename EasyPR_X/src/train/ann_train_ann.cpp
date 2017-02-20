// ann_train.cpp : annģ�͵�ѵ���ļ�����Ҫ����OCR��

#include <opencv2/opencv.hpp>
#include "easypr/plate_recognize.h"
#include "easypr/util.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include <vector>
#include <iostream>
#include <cstdlib>

#ifdef OS_WINDOWS
#include <io.h>
#endif

using namespace easypr;
using namespace cv;
using namespace std;

#define HORIZONTAL 1
#define VERTICAL 0

cv::Ptr<cv::ml::ANN_MLP>  ann;

//�й�����
const char strCharacters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F', 'G',
                              'H', /* û��I */
                              'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q',
                              'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const int numCharacter = 34;
/* û��I��O,10��������24��Ӣ���ַ�֮�� */

//���¶�����ѵ��ʱ�õ��������ַ����ݣ�����ȫ�棬��Щʡ��û��ѵ����������û���ַ�
//��Щ���������2�ı�ʾ��ѵ��ʱ�������ַ���һ�ֱ��Σ�Ҳ��Ϊѵ�����ݴ洢
const std::string strChinese[] = {
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
const int numAll = 65;

/* 34+20=54 */

cv::Mat getSyntheticImage(const Mat& image) {
	int rand_type = rand();
	Mat result = image.clone();

	if (rand_type % 2 == 0) {
		int ran_x = rand() % 5 - 2;
		int ran_y = rand() % 5 - 2;

		result = translateImg(result, ran_x, ran_y);
	}
	else if (rand_type % 2 != 0) {
		float angle = float(rand() % 15 - 7);

		result = rotateImg(result, angle);
	}

	return result;
}
cv::Ptr<cv::ml::TrainData> sdata(size_t number_for_count) {
	

	cv::Mat samples;
	std::vector<int> labels;

	int classNumber = 0;
	classNumber = numAll;


	srand((unsigned)time(0));

	for (int i = 0; i < classNumber; ++i) {

		auto char_key = kChars[i + kCharsTotalNumber - classNumber];
		char sub_folder[512] = { 0 };

		sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
		fprintf(stdout, ">> Testing characters %s in %s \n", char_key, sub_folder);

		auto chars_files = utils::getFiles(sub_folder);
		size_t char_size = chars_files.size();
		fprintf(stdout, ">> Characters count: %d \n", char_size);

		std::vector<cv::Mat> matVec;
		matVec.reserve(number_for_count);
		for (auto file : chars_files) {
			auto img = cv::imread(file, 0);  // a grayscale image
			matVec.push_back(img);
		}

		for (int t = 0; t < (int)number_for_count - (int)char_size; t++) {
			int rand_range = char_size + t;
			int ran_num = rand() % rand_range;
			auto img = matVec.at(ran_num);
			auto simg = getSyntheticImage(img);
			matVec.push_back(simg);
			if (1) {
				std::stringstream ss(std::stringstream::in | std::stringstream::out);
				ss << sub_folder << "/" << i << "_" << t << "_" << ran_num << ".jpg";
				imwrite(ss.str(), simg);
			}
		}

		fprintf(stdout, ">> Characters count: %d \n", matVec.size());

		for (auto img : matVec) {
			auto fps = charFeatures2(img, kPredictSize);

			samples.push_back(fps);
			labels.push_back(i);
		}
	}

	cv::Mat samples_;
	samples.convertTo(samples_, CV_32F);
	cv::Mat train_classes =
		cv::Mat::zeros((int)labels.size(), classNumber, CV_32F);

	for (int i = 0; i < train_classes.rows; ++i) {
		train_classes.at<float>(i, labels[i]) = 1.f;
	}

	return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,
		train_classes);
}



void annTrain(Mat TrainData, Mat classes, int nNeruns)
{

  Mat layers(1, 3, CV_32SC1);
  layers.at<int>(0) = TrainData.cols;
  layers.at<int>(1) = nNeruns;
  layers.at<int>(2) = numAll;

  ann =  cv::ml::ANN_MLP::create();

  // Prepare trainClases
  // Create a mat with n trained data by m classes
  Mat trainClasses;
  trainClasses.create(TrainData.rows, numAll, CV_32FC1);
  for (int i = 0; i < trainClasses.rows; i++) {
    for (int k = 0; k < trainClasses.cols; k++) {
      // If class of data i is same than a k class
      if (k == classes.at<int>(i))
        trainClasses.at<float>(i, k) = 1;
      else
        trainClasses.at<float>(i, k) = 0;
    }
  }
  Mat weights(1, TrainData.rows, CV_32FC1, Scalar::all(1));

  // Learn classifier
  // ann.train( TrainData, trainClasses, weights );

  // Setup the BPNetwork

  // params.train_method=CvANN_MLP_TrainParams::RPROP;
  // params.rp_dw0 = 0.1;
  // params.rp_dw_plus = 1.2;
  // params.rp_dw_minus = 0.5;
  // params.rp_dw_min = FLT_EPSILON;
  // params.rp_dw_max = 50.;


  ann->train(TrainData);




  int classNumber = 0;

  cv::Mat layers;

  int input_number = 0;
  int hidden_number = 0;
  int output_number = 0;


	classNumber = numAll;

	input_number = TrainData.cols;
	hidden_number = nNeruns;
	output_number = classNumber;
 



  int N = input_number;
  int m = output_number;

  int first_hidden_neurons = int(std::sqrt((m + 2) * N) + 2 * std::sqrt(N / (m + 2)));
  int second_hidden_neurons = int(m * std::sqrt(N / (m + 2)));



  ann->setLayerSizes(layers);
  ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  ann->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
  ann->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));
  ann->setBackpropWeightScale(0.1);
  ann->setBackpropMomentumScale(0.1);



  std::cout << "Training ANN model, please wait..." << std::endl;
  long start = utils::getTimestamp();
  ann->train(traindata);


  long end = utils::getTimestamp();
  ann->save(ann_xml_);


  std::cout << "Your ANN Model was saved to " << ann_xml_ << std::endl;
  std::cout << "Training done. Time elapse: " << (end - start) / (1000 * 60) << "minute"
	  << std::endl;
}




int saveTrainData() 
{

  cout << "Begin saveTrainData" << endl;
  Mat classes;
  Mat trainingDataf5;
  Mat trainingDataf10;
  Mat trainingDataf15;
  Mat trainingDataf20;

  vector<int> trainingLabels;
  string path = "resources/train/data/chars_recognise_ann/chars2";

  for (int i = 0; i < numCharacter; i++) {
    cout << "Character: " << strCharacters[i] << "\n";
    stringstream ss(stringstream::in | stringstream::out);
    ss << path << "/" << strCharacters[i];

    auto files = Utils::getFiles(ss.str());

    size_t size = files.size();
	for (size_t j = 0; j < size; j++) {
      cout << files[j].c_str() << endl;
      Mat img = imread(files[j].c_str(), 0);
      Mat f5 = features(img, 5);
      Mat f10 = features(img, 10);
      Mat f15 = features(img, 15);
      Mat f20 = features(img, 20);

      trainingDataf5.push_back(f5);
      trainingDataf10.push_back(f10);
      trainingDataf15.push_back(f15);
      trainingDataf20.push_back(f20);
      trainingLabels.push_back(i);  //ÿһ���ַ�ͼƬ����Ӧ���ַ���������±�
    }
  }

  path = "resources/train/data/chars_recognise_ann/charsChinese";

  for (int i = 0; i < numChinese; i++) {
    cout << "Character: " << strChinese[i] << "\n";
    stringstream ss(stringstream::in | stringstream::out);
    ss << path << "/" << strChinese[i];

    auto files = Utils::getFiles(ss.str());

    int size = files.size();
    for (int j = 0; j < size; j++) {
      cout << files[j].c_str() << endl;
      Mat img = imread(files[j].c_str(), 0);
      Mat f5 = features(img, 5);
      Mat f10 = features(img, 10);
      Mat f15 = features(img, 15);
      Mat f20 = features(img, 20);

      trainingDataf5.push_back(f5);
      trainingDataf10.push_back(f10);
      trainingDataf15.push_back(f15);
      trainingDataf20.push_back(f20);
      trainingLabels.push_back(i + numCharacter);
    }
  }

  trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
  trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
  trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
  trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
  Mat(trainingLabels).copyTo(classes);

  FileStorage fs("resources/train/ann_data.xml", FileStorage::WRITE);
  fs << "TrainingDataF5" << trainingDataf5;
  fs << "TrainingDataF10" << trainingDataf10;
  fs << "TrainingDataF15" << trainingDataf15;
  fs << "TrainingDataF20" << trainingDataf20;
  fs << "classes" << classes;
  fs.release();

  cout << "End saveTrainData" << endl;

  return 0;
}

void saveModel(int _predictsize, int _neurons) {
  FileStorage fs;
  fs.open("resources/train/ann_data.xml", FileStorage::READ);

  Mat TrainingData;
  Mat Classes;

  string training;
  if (1) {
    stringstream ss(stringstream::in | stringstream::out);
    ss << "TrainingDataF" << _predictsize;
    training = ss.str();
  }

  fs[training] >> TrainingData;
  fs["classes"] >> Classes;

  // train the Ann
  cout << "Begin to saveModelChar predictSize:" << _predictsize
       << " neurons:" << _neurons << endl;

  long start = Utils::getTimestamp();
  annTrain(TrainingData, Classes, _neurons);

  long end = Utils::getTimestamp();
  cout << "Elapse:" << (end - start) / 1000 << endl;

  cout << "End the saveModelChar" << endl;

  string model_name = "resources/train/ann.xml";
  // if(1)
  //{
  //	stringstream ss(stringstream::in | stringstream::out);
  //	ss << "ann_prd" << _predictsize << "_neu"<< _neurons << ".xml";
  //	model_name = ss.str();
  //}

  FileStorage fsTo(model_name, cv::FileStorage::WRITE);
  ann.write(*fsTo, "ann");
}

int annMain() {
  cout << "To be begin." << endl;

  saveTrainData();

  //�ɸ�����Ҫѵ����ͬ��predictSize����neurons��ANNģ��
  // for (int i = 2; i <= 2; i ++)
  //{
  //	int size = i * 5;
  //	for (int j = 5; j <= 10; j++)
  //	{
  //		int neurons = j * 10;
  //		saveModel(size, neurons);
  //	}
  //}

  //������ʾֻѵ��model�ļ����µ�ann.xml����ģ����һ��predictSize=10,neurons=40��ANNģ�͡�
  //���ݻ����Ĳ�ͬ��ѵ��ʱ�䲻һ������һ����Ҫ10�������ң�����������һ��ɡ�
  saveModel(10, 40);

  cout << "To be end." << endl;
  int end;
  cin >> end;
  return 0;
}