#include <cassert>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ex.hpp"
#include "main_inter.h"
using namespace cv;
using namespace std;
using namespace caffe;

void showLandmarks(Mat &image, Rect &bbox, vector<Point2f> &landmarks) {
  Mat img;
  image.copyTo(img);
  rectangle(img, bbox, Scalar(0, 0, 255), 2);
  for (int i = 0; i < landmarks.size(); i++) {
    Point2f &point = landmarks[i];
    circle(img, point, 2, Scalar(0, 255, 0), -1);
  }

}






int main_CNN5Align(char * fileName) {





  FaceDetector fd;
  Landmarker lder;
  fd.LoadXML("../deeplandmark/haarcascade_frontalface_alt2.xml");
  lder.LoadModel("../deeplandmark");



  Mat gray;
  Mat image = imread(fileName);
  if (image.data == NULL) return -1;
  cvtColor(image, gray, CV_BGR2GRAY);

  int64 t0 = getTickCount();
  vector<Rect> bboxes;
  fd.DetectFace(gray, bboxes);

  vector<Point2f> landmarks;
  int64 t1 = getTickCount();  
  printf("DetectFace cost time %lld.....\n", t1 - t0);
  for (int i = 0; i < bboxes.size(); i++) {
    BBox bbox_ = BBox(bboxes[i]).subBBox(0.1, 0.9, 0.2, 1);
    landmarks = lder.DetectLandmark(gray, bbox_); 
	
	int64 t2 = getTickCount();

    printf("landmarks cost time %I64d.....\n",t2-t1);
    showLandmarks(image, bbox_.rect, landmarks);
  }

  return 0;
}
