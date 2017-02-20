#include <cassert>
#include "main_inter.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <caffe/caffe.hpp>

using namespace cv;
using namespace std;
using namespace caffe;
void getbox(std::vector<float> result, float* pro_obj, int* idx_class, std::vector<std::vector<int> >& bboxs, float thresh, cv::Mat image);
void getbox(std::vector<float> result, float* pro_obj, int* idx_class, std::vector<std::vector<int> >& bboxs, float thresh, cv::Mat image) {
	float pro_class[49];
	int idx;
	float max_idx;
	float max;
	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 7; ++j) {
			max = 0;
			max_idx = 0;
			idx = 20 * (i * 7 + j);
			for (int k = 0; k < 20; ++k) {
				if (result[idx + k] > max) {
					max = result[idx + k];
					max_idx = k + 1;
				}
			}
			idx_class[i * 7 + j] = max_idx;
			pro_class[i * 7 + j] = max;
			pro_obj[(i * 7 + j) * 2] = max*result[7 * 7 * 20 + (i * 7 + j) * 2];
			pro_obj[(i * 7 + j) * 2 + 1] = max*result[7 * 7 * 20 + (i * 7 + j) * 2 + 1];
		}
	}
	std::vector<int> bbox;
	int x_min, x_max, y_min, y_max;
	float x, y, w, h;
	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 7; ++j) {
			for (int k = 0; k < 2; ++k) {
				if (pro_obj[(i * 7 + j) * 2 + k] > thresh) {
					//std::cout << "(" << i << "," << j << "," << k << ")" << " prob="<<pro_obj[(i*7+j)*2 + k] << " class="<<idx_class[i*7+j]<<std::endl;
					idx = 49 * 20 + 49 * 2 + ((i * 7 + j) * 2 + k) * 4;
					x = image.cols*(result[idx++] + j) / 7;
					y = image.rows*(result[idx++] + i) / 7;
					w = image.cols*result[idx] * result[idx++];
					h = image.rows*result[idx] * result[idx];
					//std::cout << x <<" "<< y << " " << w <<" "<< h <<std::endl;
					x_min = x - w / 2;
					y_min = y - h / 2;
					x_max = x + w / 2;
					y_max = y + h / 2;
					bbox.clear();
					bbox.push_back(idx_class[i * 7 + j]);
					bbox.push_back(x_min);
					bbox.push_back(y_min);
					bbox.push_back(x_max);
					bbox.push_back(y_max);
					bbox.push_back(int(pro_obj[(i * 7 + j) * 2 + k] * 100));
					bboxs.push_back(bbox);
				}
			}
		}
	}
}



void loaddata(Net<float> * net, std::string image_path) {
	Blob<float>* input_layer = net->input_blobs()[0];
	int width, height;
	width = input_layer->width();
	height = input_layer->height();
	int size = width*height;
	cv::Mat image = cv::imread(image_path, 1);
	cv::Mat image_resized;
	cv::resize(image, image_resized, cv::Size(height, width));
	float* input_data = input_layer->mutable_cpu_data();
	int temp, idx;
	for (int i = 0; i < height; ++i) {
		uchar* pdata = image_resized.ptr<uchar>(i);
		for (int j = 0; j < width; ++j) {
			temp = 3 * j;
			idx = i*width + j;
			input_data[idx] = (pdata[temp + 2] / 255.0) ;
			input_data[idx + size] = (pdata[temp + 1] / 255.0) ;
			input_data[idx + 2 * size] = (pdata[temp + 0] / 255.0);
		}
	}
	//cv::imshow("image",image_resized);
}


template<typename Dtype>
Dtype lap(Dtype x1_min, Dtype x1_max, Dtype x2_min, Dtype x2_max) {
	if (x1_min < x2_min) {
		if (x1_max < x2_min) {
			return 0;
		}
		else {
			if (x1_max > x2_min) {
				if (x1_max < x2_max) {
					return x1_max - x2_min;
				}
				else {
					return x2_max - x2_min;
				}
			}
			else {
				return 0;
			}
		}
	}
	else {
		if (x1_min < x2_max) {
			if (x1_max < x2_max)
				return x1_max - x1_min;
			else
				return x2_max - x1_min;
		}
		else {
			return 0;
		}
	}
}



void DoTestRectObject(char * fileName)
{

	caffe::Net<float> *net = new Net<float>("../yolomodel/yolo_tiny_deploy.prototxt", caffe::TEST);
	net->CopyTrainedLayersFrom("../yolomodel/yolo_tiny.caffemodel");
	int64 t11 = getTickCount();

	loaddata(net, fileName);
	net->Forward();
	int64 t21 = getTickCount();

	char szBuf[512];
	sprintf(szBuf,"%I64d",t21-t11);


	//std::cout<<"to process finish time = "<< t.elapsed() <<std::endl;
	Blob<float>* output_layer = net->output_blobs()[0];
	const float* begin = output_layer->cpu_data();
	const float* end = begin + output_layer->channels();
	std::vector<float> result(begin, end);
	//接下来就是生成框。
	std::vector<std::vector<int> > bboxs;
	float pro_obj[49][2];
	int idx_class[49];
	cv::Mat image = cv::imread(fileName);
	float overlap;
	float overlap_thresh = 0.4;
	//cv::resize(image,image,cv::Size(448,448));
	getbox(result, &pro_obj[0][0], idx_class, bboxs, 0.2, image);
	std::vector<bool> mark(bboxs.size(), true);
	for (int i = 0; i < bboxs.size(); ++i) {
	 for (int j = i + 1; j < bboxs.size(); ++j) {
	  int overlap_x = lap(bboxs[i][0], bboxs[i][2], bboxs[j][0], bboxs[j][2]);
	  int overlap_y = lap(bboxs[i][1], bboxs[i][3], bboxs[j][1], bboxs[j][3]);
	  overlap = (overlap_x*overlap_y)*1.0 / ((bboxs[i][0] - bboxs[i][2])*(bboxs[i][1] - bboxs[i][3]) + (bboxs[j][0] - bboxs[j][2])*(bboxs[j][1] - bboxs[j][3]) - (overlap_x*overlap_y));
	  if (overlap > overlap_thresh) {
		  if (bboxs[i][4] > bboxs[j][4]) {
			  mark[j] = false;
		  }
		  else {
			  mark[i] = false;
		  }
	  }
	 }
	}
	char *labelname[] = { "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor" };
	for (int i = 0; i < bboxs.size(); ++i) {
	 if (mark[i]) {
	  cv::Point point1(bboxs[i][1], bboxs[i][2]);
	  cv::Point point2(bboxs[i][3], bboxs[i][4]);
	  cv::rectangle(image, cv::Rect(point1, point2), cv::Scalar(0, bboxs[i][0] / 20.0 * 225, 255), bboxs[i][5] / 8);
	  char ch[100];
	  sprintf(ch, "%s %.2f", labelname[bboxs[i][0] - 1], bboxs[i][5] * 1.0 / 100);
	  std::string temp(ch);
	  cv::putText(image, temp, point1, CV_FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255, 255, 255));
	 }
	}

}

int main_yolo(char * fileName) {
 

	DoTestRectObject(fileName);
  
  return 0;
}
