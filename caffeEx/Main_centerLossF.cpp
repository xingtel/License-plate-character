// caffe
#include "main_inter.h"
#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

// c++
#include <string>
#include <vector>
#include <fstream>
// opencv
#include <opencv2/opencv.hpp>
// boost
#include "boost/make_shared.hpp"
#include <time.h>
//#define CPU_ONLY
using namespace caffe;

typedef struct FaceRect {
	float x1;
	float y1;
	float x2;
	float y2;
	float score; /**< Larger score should mean higher confidence. */
} FaceRect;

typedef struct FacePts {
	float x[5], y[5];
} FacePts;

typedef struct FaceInfo {
	FaceRect bbox;
	cv::Vec4f regression;
	FacePts facePts;
	double roll;
	double pitch;
	double yaw;
} FaceInfo;

class CenterLossF {
public:
	CenterLossF(const string& proto_model_dir);
	CenterLossF();
	void load(const string& proto_model_dir);
	void Detect(const cv::Mat& image, float * outFeature);

private:
	bool CvMatToDatumSignalChannel(const cv::Mat& cv_mat, Datum* datum);
	void Preprocess(const cv::Mat& img,
		std::vector<cv::Mat>* input_channels);
	void WrapInputLayer(std::vector<cv::Mat>* input_channels, Blob<float>* input_layer,
		const int height, const int width);
	void SetMean();
	void GenerateBoundingBox(Blob<float>* confidence, Blob<float>* reg,
		float scale, float thresh, int image_width, int image_height);
	void ClassifyFace(const std::vector<FaceInfo>& regressed_rects, cv::Mat &sample_single,
		boost::shared_ptr<Net<float> >& net, double thresh, char netName);
	void ClassifyFace_MulImage(const std::vector<FaceInfo> &regressed_rects, cv::Mat &sample_single,
		boost::shared_ptr<Net<float> >& net, double thresh, char netName);
	std::vector<FaceInfo> NonMaximumSuppression(std::vector<FaceInfo>& bboxes, float thresh, char methodType);
	void Bbox2Square(std::vector<FaceInfo>& bboxes);
	void Padding(int img_w, int img_h);
	std::vector<FaceInfo> BoxRegress(std::vector<FaceInfo> &faceInfo_, int stage);
	void RegressPoint(const std::vector<FaceInfo>& faceInfo);

private:
	boost::shared_ptr<Net<float> > PNet_;
	boost::shared_ptr<Net<float> > RNet_;
	boost::shared_ptr<Net<float> > ONet_;

	// x1,y1,x2,t2 and score
	std::vector<FaceInfo> condidate_rects_;
	std::vector<FaceInfo> total_boxes_;
	std::vector<FaceInfo> regressed_rects_;
	std::vector<FaceInfo> regressed_pading_;

	std::vector<cv::Mat> crop_img_;
	int curr_feature_map_w_;
	int curr_feature_map_h_;
	int num_channels_;
};


// methodType : u is IoU(Intersection Over Union)
// methodType : m is IoM(Intersection Over Maximum)
std::vector<FaceInfo> CenterLossF::NonMaximumSuppression(std::vector<FaceInfo>& bboxes,
	float thresh, char methodType) {
	std::vector<FaceInfo> bboxes_nms;
	//std::sort(bboxes.begin(), bboxes.end(), CompareBBox);

	int32_t select_idx = 0;
	int32_t num_bbox = static_cast<int32_t>(bboxes.size());
	std::vector<int32_t> mask_merged(num_bbox, 0);
	bool all_merged = false;

	while (!all_merged) {
		while (select_idx < num_bbox && mask_merged[select_idx] == 1)
			select_idx++;
		if (select_idx == num_bbox) {
			all_merged = true;
			continue;
		}

		bboxes_nms.push_back(bboxes[select_idx]);
		mask_merged[select_idx] = 1;

		FaceRect select_bbox = bboxes[select_idx].bbox;
		float area1 = static_cast<float>((select_bbox.x2 - select_bbox.x1 + 1) * (select_bbox.y2 - select_bbox.y1 + 1));
		float x1 = static_cast<float>(select_bbox.x1);
		float y1 = static_cast<float>(select_bbox.y1);
		float x2 = static_cast<float>(select_bbox.x2);
		float y2 = static_cast<float>(select_bbox.y2);

		select_idx++;
		for (int32_t i = select_idx; i < num_bbox; i++) {
			if (mask_merged[i] == 1)
				continue;

			FaceRect& bbox_i = bboxes[i].bbox;
			float x = std::max<float>(x1, bbox_i.x1);
			float y = std::max<float>(y1,bbox_i.y1);
			float w = std::min<float>(x2,bbox_i.x2) - x + 1;
			float h = std::min<float>(y2, bbox_i.y2) - y + 1;
			if (w <= 0 || h <= 0)
				continue;

			float area2 = static_cast<float>((bbox_i.x2 - bbox_i.x1 + 1) * (bbox_i.y2 - bbox_i.y1 + 1));
			float area_intersect = w * h;

			switch (methodType) {
			case 'u':
				if (static_cast<float>(area_intersect) / (area1 + area2 - area_intersect) > thresh)
					mask_merged[i] = 1;
				break;
			case 'm':
				if (static_cast<float>(area_intersect) / std::min(area1, area2) > thresh)
					mask_merged[i] = 1;
				break;
			default:
				break;
			}
		}
	}
	return bboxes_nms;
}

void CenterLossF::Bbox2Square(std::vector<FaceInfo>& bboxes) {
	for (int i = 0; i<bboxes.size(); i++) {
		float h = bboxes[i].bbox.x2 - bboxes[i].bbox.x1;
		float w = bboxes[i].bbox.y2 - bboxes[i].bbox.y1;
		float side = h>w ? h : w;
		bboxes[i].bbox.x1 += (h - side)*0.5;
		bboxes[i].bbox.y1 += (w - side)*0.5;

		bboxes[i].bbox.x2 = bboxes[i].bbox.x1 + side;
		bboxes[i].bbox.y2 =bboxes[i].bbox.y1 + side;
		bboxes[i].bbox.x1 = bboxes[i].bbox.x1;
		bboxes[i].bbox.y1 = bboxes[i].bbox.y1;

	}
}

std::vector<FaceInfo> CenterLossF::BoxRegress(std::vector<FaceInfo>& faceInfo, int stage) {
	std::vector<FaceInfo> bboxes;
	for (int bboxId = 0; bboxId<faceInfo.size(); bboxId++) {
		FaceRect faceRect;
		FaceInfo tempFaceInfo;
		float regw = faceInfo[bboxId].bbox.y2 - faceInfo[bboxId].bbox.y1;
		regw += (stage == 1) ? 0 : 1;
		float regh = faceInfo[bboxId].bbox.x2 - faceInfo[bboxId].bbox.x1;
		regh += (stage == 1) ? 0 : 1;
		faceRect.y1 = faceInfo[bboxId].bbox.y1 + regw * faceInfo[bboxId].regression[0];
		faceRect.x1 = faceInfo[bboxId].bbox.x1 + regh * faceInfo[bboxId].regression[1];
		faceRect.y2 = faceInfo[bboxId].bbox.y2 + regw * faceInfo[bboxId].regression[2];
		faceRect.x2 = faceInfo[bboxId].bbox.x2 + regh * faceInfo[bboxId].regression[3];
		faceRect.score = faceInfo[bboxId].bbox.score;

		tempFaceInfo.bbox = faceRect;
		tempFaceInfo.regression = faceInfo[bboxId].regression;
		if (stage == 3)
			tempFaceInfo.facePts = faceInfo[bboxId].facePts;
		bboxes.push_back(tempFaceInfo);
	}
	return bboxes;
}

// compute the padding coordinates (pad the bounding boxes to square)
void CenterLossF::Padding(int img_w, int img_h) {
	for (int i = 0; i<regressed_rects_.size(); i++) {
		FaceInfo tempFaceInfo;
		tempFaceInfo = regressed_rects_[i];
		tempFaceInfo.bbox.y2 = (regressed_rects_[i].bbox.y2 >= img_w) ? img_w : regressed_rects_[i].bbox.y2;
		tempFaceInfo.bbox.x2 = (regressed_rects_[i].bbox.x2 >= img_h) ? img_h : regressed_rects_[i].bbox.x2;
		tempFaceInfo.bbox.y1 = (regressed_rects_[i].bbox.y1 <1) ? 1 : regressed_rects_[i].bbox.y1;
		tempFaceInfo.bbox.x1 = (regressed_rects_[i].bbox.x1 <1) ? 1 : regressed_rects_[i].bbox.x1;
		regressed_pading_.push_back(tempFaceInfo);
	}
}

void CenterLossF::GenerateBoundingBox(Blob<float>* confidence, Blob<float>* reg,
	float scale, float thresh, int image_width, int image_height) {
	int stride = 2;
	int cellSize = 12;

	int curr_feature_map_w_ = std::ceil((image_width - cellSize)*1.0 / stride) + 1;
	int curr_feature_map_h_ = std::ceil((image_height - cellSize)*1.0 / stride) + 1;

	//std::cout << "Feature_map_size:"<< curr_feature_map_w_ <<" "<<curr_feature_map_h_<<std::endl;
	int regOffset = curr_feature_map_w_*curr_feature_map_h_;
	// the first count numbers are confidence of face
	int count = confidence->count() / 2;
	const float* confidence_data = confidence->cpu_data();
	confidence_data += count;
	const float* reg_data = reg->cpu_data();

	condidate_rects_.clear();
	for (int i = 0; i<count; i++) {
		if (*(confidence_data + i) >= thresh) {
			int y = i / curr_feature_map_w_;
			int x = i - curr_feature_map_w_ * y;

			float xTop = ((x*stride + 1) / scale);
			float yTop = ((y*stride + 1) / scale);
			float xBot = ((x*stride + cellSize - 1 + 1) / scale);
			float yBot = ((y*stride + cellSize - 1 + 1) / scale);
			FaceRect faceRect;
			faceRect.x1 = xTop;
			faceRect.y1 = yTop;
			faceRect.x2 = xBot;
			faceRect.y2 = yBot;
			faceRect.score = *(confidence_data + i);
			FaceInfo faceInfo;
			faceInfo.bbox = faceRect;
			faceInfo.regression = cv::Vec4f(reg_data[i + 0 * regOffset], reg_data[i + 1 * regOffset], reg_data[i + 2 * regOffset], reg_data[i + 3 * regOffset]);
			condidate_rects_.push_back(faceInfo);
		}
	}
}
CenterLossF::CenterLossF()
{

}

void CenterLossF::load(const string& proto_model_dir)
{
#ifdef CPU_ONLY
	Caffe::set_mode(Caffe::CPU);
#else
	Caffe::set_mode(Caffe::GPU);
#endif
	/* Load the network. */
	PNet_.reset(new Net<float>((proto_model_dir + "/LightenedCNN_A_deploy.prototxt"), TEST));
	PNet_->CopyTrainedLayersFrom(proto_model_dir + "/LightenedCNN_A.caffemodel");

	//PNet_.reset(new Net<float>((proto_model_dir + "/det1.prototxt"), TEST));
	//PNet_->CopyTrainedLayersFrom(proto_model_dir + "/det1.caffemodel");


	//CHECK_EQ(PNet_->num_inputs(), 1) << "Network should have exactly one input.";
	//CHECK_EQ(PNet_->num_outputs(), 2) << "Network should have exactly two output, one"
	//	" is bbox and another is confidence.";


	Blob<float>* input_layer;
	input_layer = PNet_->input_blobs()[0];
	num_channels_ = input_layer->channels();
	CHECK(num_channels_ == 3 || num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
}


CenterLossF::CenterLossF(const std::string &proto_model_dir) {
#ifdef CPU_ONLY
	Caffe::set_mode(Caffe::CPU);
#else
	Caffe::set_mode(Caffe::GPU);
#endif
	/* Load the network. */
	PNet_.reset(new Net<float>((proto_model_dir + "/det1.prototxt"), TEST));
	PNet_->CopyTrainedLayersFrom(proto_model_dir + "/det1.caffemodel");

	CHECK_EQ(PNet_->num_inputs(), 1) << "Network should have exactly one input.";
	CHECK_EQ(PNet_->num_outputs(), 2) << "Network should have exactly two output, one"
		" is bbox and another is confidence.";

#ifdef CPU_ONLY
	RNet_.reset(new Net<float>((proto_model_dir + "/det2.prototxt"), TEST));
#else
	RNet_.reset(new Net<float>((proto_model_dir + "/det2_input.prototxt"), TEST));
#endif
	RNet_->CopyTrainedLayersFrom(proto_model_dir + "/det2.caffemodel");

	//  CHECK_EQ(RNet_->num_inputs(), 0) << "Network should have exactly one input.";
	//  CHECK_EQ(RNet_->num_outputs(),3) << "Network should have exactly two output, one"
	//                                     " is bbox and another is confidence.";

#ifdef CPU_ONLY
	ONet_.reset(new Net<float>((proto_model_dir + "/det3.prototxt"), TEST));
#else
	ONet_.reset(new Net<float>((proto_model_dir + "/det3_input.prototxt"), TEST));
#endif
	ONet_->CopyTrainedLayersFrom(proto_model_dir + "/det3.caffemodel");

	//  CHECK_EQ(ONet_->num_inputs(), 1) << "Network should have exactly one input.";
	//  CHECK_EQ(ONet_->num_outputs(),3) << "Network should have exactly three output, one"
	//                                     " is bbox and another is confidence.";

	Blob<float>* input_layer;
	input_layer = PNet_->input_blobs()[0];
	num_channels_ = input_layer->channels();
	CHECK(num_channels_ == 3 || num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
}

void CenterLossF::WrapInputLayer(std::vector<cv::Mat>* input_channels,
	Blob<float>* input_layer, const int height, const int width)
{
	float* input_data = input_layer->mutable_cpu_data();

	for (int i = 0; i < input_layer->channels(); ++i) 
	{
		cv::Mat channel(height, width, CV_32FC1, input_data);
		input_channels->push_back(channel);
		input_data += width * height;
	}
}

void CenterLossF::ClassifyFace(const std::vector<FaceInfo>& regressed_rects, cv::Mat &sample_single,
	boost::shared_ptr<Net<float> >& net, double thresh, char netName) {
	int numBox = regressed_rects.size();
	Blob<float>* crop_input_layer = net->input_blobs()[0];
	int input_channels = crop_input_layer->channels();
	int input_width = crop_input_layer->width();
	int input_height = crop_input_layer->height();
	crop_input_layer->Reshape(1, input_channels, input_width, input_height);
	net->Reshape();

	condidate_rects_.clear();
	// load crop_img data to datum
	for (int i = 0; i<numBox; i++) {
		std::vector<cv::Mat> channels;
		WrapInputLayer(&channels, net->input_blobs()[0], input_width, input_height);
		int pad_top = std::abs(regressed_pading_[i].bbox.x1 - regressed_rects[i].bbox.x1);
		int pad_left = std::abs(regressed_pading_[i].bbox.y1 - regressed_rects[i].bbox.y1);
		int pad_right = std::abs(regressed_pading_[i].bbox.y2 - regressed_rects[i].bbox.y2);
		int pad_bottom = std::abs(regressed_pading_[i].bbox.x2 - regressed_rects[i].bbox.x2);

		cv::Mat crop_img = sample_single(cv::Range(regressed_pading_[i].bbox.y1 - 1, regressed_pading_[i].bbox.y2),
			cv::Range(regressed_pading_[i].bbox.x1 - 1, regressed_pading_[i].bbox.x2));
		cv::copyMakeBorder(crop_img, crop_img, pad_left, pad_right, pad_top, pad_bottom, cv::BORDER_CONSTANT, cv::Scalar(0));

		cv::resize(crop_img, crop_img, cv::Size(input_width, input_height), 0, 0, cv::INTER_AREA);
		crop_img = (crop_img - 127.5)*0.0078125;
		cv::split(crop_img, channels);

		CHECK(reinterpret_cast<float*>(channels.at(0).data) == net->input_blobs()[0]->cpu_data())
			<< "Input channels are not wrapping the input layer of the network.";
		net->Forward();

		int reg_id = 0;
		int confidence_id = 1;
		if (netName == 'o') confidence_id = 2;
		const Blob<float>* reg = net->output_blobs()[reg_id];
		const Blob<float>* confidence = net->output_blobs()[confidence_id];
		// ONet points_offset != NULL
		const Blob<float>* points_offset = net->output_blobs()[1];

		const float* confidence_data = confidence->cpu_data() + confidence->count() / 2;
		const float* reg_data = reg->cpu_data();
		const float* points_data;
		if (netName == 'o') points_data = points_offset->cpu_data();

		if (*(confidence_data) > thresh) {
			FaceRect faceRect;
			faceRect.x1 = regressed_rects[i].bbox.x1;
			faceRect.y1 = regressed_rects[i].bbox.y1;
			faceRect.x2 = regressed_rects[i].bbox.x2;
			faceRect.y2 = regressed_rects[i].bbox.y2;
			faceRect.score = *(confidence_data);
			FaceInfo faceInfo;
			faceInfo.bbox = faceRect;
			faceInfo.regression = cv::Vec4f(reg_data[0], reg_data[1], reg_data[2], reg_data[3]);

			// x x x x x y y y y y
			if (netName == 'o') {
				FacePts face_pts;
				float w = faceRect.y2 - faceRect.y1 + 1;
				float h = faceRect.x2 - faceRect.x1 + 1;
				for (int j = 0; j<5; j++) {
					face_pts.y[j] = faceRect.y1 + *(points_data + j) * h - 1;
					face_pts.x[j] = faceRect.x1 + *(points_data + j + 5) * w - 1;
				}
				faceInfo.facePts = face_pts;
			}
			condidate_rects_.push_back(faceInfo);
		}
	}
	regressed_pading_.clear();
}

// multi test image pass a forward
void CenterLossF::ClassifyFace_MulImage(const std::vector<FaceInfo>& regressed_rects, cv::Mat &sample_single,
	boost::shared_ptr<Net<float> >& net, double thresh, char netName) {
	condidate_rects_.clear();

	int numBox = regressed_rects.size();
	std::vector<Datum> datum_vector;

	boost::shared_ptr<MemoryDataLayer<float> > mem_data_layer;
	mem_data_layer = boost::static_pointer_cast<MemoryDataLayer<float> >(net->layers()[0]);
	int input_width = mem_data_layer->width();
	int input_height = mem_data_layer->height();

	// load crop_img data to datum
	for (int i = 0; i<numBox; i++) {
		int pad_top = std::abs(regressed_pading_[i].bbox.x1 - regressed_rects[i].bbox.x1);
		int pad_left = std::abs(regressed_pading_[i].bbox.y1 - regressed_rects[i].bbox.y1);
		int pad_right = std::abs(regressed_pading_[i].bbox.y2 - regressed_rects[i].bbox.y2);
		int pad_bottom = std::abs(regressed_pading_[i].bbox.x2 - regressed_rects[i].bbox.x2);

		cv::Mat crop_img = sample_single(cv::Range(regressed_pading_[i].bbox.y1 - 1, regressed_pading_[i].bbox.y2),
			cv::Range(regressed_pading_[i].bbox.x1 - 1, regressed_pading_[i].bbox.x2));
		cv::copyMakeBorder(crop_img, crop_img, pad_left, pad_right, pad_top, pad_bottom, cv::BORDER_CONSTANT, cv::Scalar(0));

		cv::resize(crop_img, crop_img, cv::Size(input_width, input_height), 0, 0, cv::INTER_AREA);
		crop_img = (crop_img - 127.5)*0.0078125;
		Datum datum;
		CvMatToDatumSignalChannel(crop_img, &datum);
		datum_vector.push_back(datum);
	}
	regressed_pading_.clear();

	/* extract the features and store */
	mem_data_layer->set_batch_size(numBox);
	mem_data_layer->AddDatumVector(datum_vector);
	/* fire the network */
	float no_use_loss = 0;
	net->Forward(&no_use_loss);
	//  CHECK(reinterpret_cast<float*>(crop_img_set.at(0).data) == net->input_blobs()[0]->cpu_data())
	//          << "Input channels are not wrapping the input layer of the network.";

	// return RNet/ONet result
	std::string outPutLayerName = (netName == 'r' ? "conv5-2" : "conv6-2");
	std::string pointsLayerName = "conv6-3";

	const boost::shared_ptr<Blob<float> > reg = net->blob_by_name(outPutLayerName);
	const boost::shared_ptr<Blob<float> > confidence = net->blob_by_name("prob1");
	// ONet points_offset != NULL
	const boost::shared_ptr<Blob<float> > points_offset = net->blob_by_name(pointsLayerName);

	const float* confidence_data = confidence->cpu_data();
	const float* reg_data = reg->cpu_data();
	const float* points_data;
	if (netName == 'o') points_data = points_offset->cpu_data();

	for (int i = 0; i<numBox; i++) {
		if (*(confidence_data + i * 2 + 1) > thresh) {
			FaceRect faceRect;
			faceRect.x1 = regressed_rects[i].bbox.x1;
			faceRect.y1 = regressed_rects[i].bbox.y1;
			faceRect.x2 = regressed_rects[i].bbox.x2;
			faceRect.y2 = regressed_rects[i].bbox.y2;
			faceRect.score = *(confidence_data + i * 2 + 1);
			FaceInfo faceInfo;
			faceInfo.bbox = faceRect;
			faceInfo.regression = cv::Vec4f(reg_data[4 * i + 0], reg_data[4 * i + 1], reg_data[4 * i + 2], reg_data[4 * i + 3]);

			// x x x x x y y y y y
			if (netName == 'o') {
				FacePts face_pts;
				float w = faceRect.y2 - faceRect.y1 + 1;
				float h = faceRect.x2 - faceRect.x1 + 1;
				for (int j = 0; j<5; j++) {
					face_pts.y[j] = faceRect.y1 + *(points_data + j + 10 * i) * h - 1;
					face_pts.x[j] = faceRect.x1 + *(points_data + j + 5 + 10 * i) * w - 1;
				}
				faceInfo.facePts = face_pts;
			}
			condidate_rects_.push_back(faceInfo);
		}
	}
}
bool CenterLossF::CvMatToDatumSignalChannel(const cv::Mat& cv_mat, Datum* datum) {
	if (cv_mat.empty())
		return false;
	int channels = cv_mat.channels();

	datum->set_channels(cv_mat.channels());
	datum->set_height(cv_mat.rows);
	datum->set_width(cv_mat.cols);
	datum->set_label(0);
	datum->clear_data();
	datum->clear_float_data();
	datum->set_encoded(false);

	int datum_height = datum->height();
	int datum_width = datum->width();
	if (channels == 3) 
	{
		for (int c = 0; c < channels; c++)
		{
			for (int h = 0; h < datum_height; ++h)
			{
				const float* ptr = cv_mat.ptr<float>(h);
				for (int w = 0; w < datum_width; ++w)
				{
					datum->add_float_data(ptr[w*channels + c]);
				}
			}
		}
	}

	return true;
}

void CenterLossF::Detect(const cv::Mat& image1,float * outFeature) {

	cv::Mat image;
	cvtColor(image1, image,CV_BGR2GRAY);

	int height = image.rows;
	int width = image.cols;
	// 11ms main consum
	Blob<float>* input_layer = PNet_->input_blobs()[0];
	cv::Mat  resized;
	// wrap image and normalization using INTER_AREA method
	cv::resize(image, resized, cv::Size(128, 128), 0, 0, cv::INTER_AREA);
	resized.convertTo(resized, CV_32FC1, 0.0078125, -127.5*0.0078125);



	const vector<Blob<float>*> &intput_blobs = PNet_->input_blobs();
	float *blob_data = intput_blobs[0]->mutable_cpu_data();
	const float *ptr = NULL;
	for (int i = 0; i < resized.rows; i++) {
		ptr = resized.ptr<float>(i);
		for (int j = 0; j < resized.cols; j++) {
			blob_data[i*resized.cols + j] = ptr[j];
		}
		//memcpy(blob_data+i*resized.cols , ptr, resized.cols*4);
	}


	PNet_->Forward();

	//Blob<float>* reg = caffe_net.output_blobs()[0];
	shared_ptr<Blob<float>> reg = PNet_->blob_by_name("fc2");
	const float* reg_data = reg->cpu_data();

	memcpy(outFeature,reg_data,10575);




}
CenterLossF detector;



