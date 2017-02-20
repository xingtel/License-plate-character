#ifdef WITH_PYTHON_LAYER
#include "boost/python.hpp"
namespace bp = boost::python;
#endif

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "main_inter.h"
#include "boost/algorithm/string.hpp"
#include "caffe/caffe.hpp"
#include "caffe\layers\memory_data_layer.hpp"
#include "caffe/util/signal_handler.h"
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include "../Common/gLobalInfo.h"

using caffe::Blob;
using caffe::Caffe;
using caffe::Net;
using caffe::Layer;
using caffe::Solver;
using caffe::shared_ptr;
using caffe::string;
using caffe::Timer;
using caffe::vector;
using std::ostringstream;
DEFINE_string(gpu, "",
	"Optional; run in GPU mode on given device IDs separated by ','."
	"Use '-gpu all' to run on all available GPUs. The effective training "
	"batch size is multiplied by the number of devices.");
DEFINE_string(solver, "",
	"The solver definition protocol buffer text file.");
DEFINE_string(model, "",
	"The model definition protocol buffer text file.");
DEFINE_string(snapshot, "",
	"Optional; the snapshot solver state to resume training.");
DEFINE_string(weights, "",
	"Optional; the pretrained weights to initialize finetuning, "
	"separated by ','. Cannot be set simultaneously with snapshot.");
DEFINE_int32(iterations, 50,
	"The number of iterations to run.");
DEFINE_string(sigint_effect, "stop",
	"Optional; action to take when a SIGINT signal is received: "
	"snapshot, stop or none.");
DEFINE_string(sighup_effect, "snapshot",
	"Optional; action to take when a SIGHUP signal is received: "
	"snapshot, stop or none.");

// A simple registry for caffe commands.
typedef int(*BrewFunction)();
typedef std::map<caffe::string, BrewFunction> BrewMap;
BrewMap g_brew_map;

#define RegisterBrewFunction(func) \
namespace { \
class __Registerer_##func { \
 public: /* NOLINT */ \
  __Registerer_##func() { \
    g_brew_map[#func] = &func; \
  } \
}; \
__Registerer_##func g_registerer_##func; \
}

static BrewFunction GetBrewFunction(const caffe::string& name) {
	if (g_brew_map.count(name)) {
		return g_brew_map[name];
	}
	else {
		LOG(ERROR) << "Available caffe actions:";
		for (BrewMap::iterator it = g_brew_map.begin();
			it != g_brew_map.end(); ++it) {
			LOG(ERROR) << "\t" << it->first;
		}
		LOG(FATAL) << "Unknown action: " << name;
		return NULL;  // not reachable, just to suppress old compiler warnings.
	}
}

// Parse GPU ids or use all available devices
static void get_gpus(vector<int>* gpus) {
	if (FLAGS_gpu == "all") {
		int count = 0;
#ifndef CPU_ONLY
		CUDA_CHECK(cudaGetDeviceCount(&count));
#else
		NO_GPU;
#endif
		for (int i = 0; i < count; ++i) {
			gpus->push_back(i);
		}
	}
	else if (FLAGS_gpu.size()) {
		vector<string> strings;
		boost::split(strings, FLAGS_gpu, boost::is_any_of(","));
		for (int i = 0; i < strings.size(); ++i) {
			gpus->push_back(boost::lexical_cast<int>(strings[i]));
		}
	}
	else {
		CHECK_EQ(gpus->size(), 0);
	}
}

// caffe commands to call by
//     caffe <command> <args>
//
// To add a command, define a function "int command()" and register it with
// RegisterBrewFunction(action);

// Device Query: show diagnostic information for a GPU device.
int device_query() {
	LOG(INFO) << "Querying GPUs " << FLAGS_gpu;
	vector<int> gpus;
	get_gpus(&gpus);
	for (int i = 0; i < gpus.size(); ++i) {
		caffe::Caffe::SetDevice(gpus[i]);
		caffe::Caffe::DeviceQuery();
	}
	return 0;
}
RegisterBrewFunction(device_query);

// Load the weights from the specified caffemodel(s) into the train and
// test nets.
void CopyLayers(caffe::Solver<float>* solver, const std::string& model_list) {
	std::vector<std::string> model_names;
	boost::split(model_names, model_list, boost::is_any_of(","));
	for (int i = 0; i < model_names.size(); ++i) {
		LOG(INFO) << "Finetuning from " << model_names[i];
		solver->net()->CopyTrainedLayersFrom(model_names[i]);
		for (int j = 0; j < solver->test_nets().size(); ++j) {
			solver->test_nets()[j]->CopyTrainedLayersFrom(model_names[i]);
		}
	}
}

// Translate the signal effect the user specified on the command-line to the
// corresponding enumeration.
caffe::SolverAction::Enum GetRequestedAction(
	const std::string& flag_value) {
	if (flag_value == "stop") {
		return caffe::SolverAction::STOP;
	}
	if (flag_value == "snapshot") {
		return caffe::SolverAction::SNAPSHOT;
	}
	if (flag_value == "none") {
		return caffe::SolverAction::NONE;
	}
	LOG(FATAL) << "Invalid signal effect \"" << flag_value << "\" was specified";
}

// Train / Finetune a model.
int train() {
	CHECK_GT(FLAGS_solver.size(), 0) << "Need a solver definition to train.";
	CHECK(!FLAGS_snapshot.size() || !FLAGS_weights.size())
		<< "Give a snapshot to resume training or weights to finetune "
		"but not both.";

	caffe::SolverParameter solver_param;
	caffe::ReadSolverParamsFromTextFileOrDie(FLAGS_solver, &solver_param);

	// If the gpus flag is not provided, allow the mode and device to be set
	// in the solver prototxt.
	if (FLAGS_gpu.size() == 0
		&& solver_param.solver_mode() == caffe::SolverParameter_SolverMode_GPU) {
		if (solver_param.has_device_id()) {
			FLAGS_gpu = "" +
				boost::lexical_cast<string>(solver_param.device_id());
		}
		else {  // Set default GPU if unspecified
			FLAGS_gpu = "" + boost::lexical_cast<string>(0);
		}
	}

	vector<int> gpus;
	get_gpus(&gpus);
	gpus.clear();

	if (gpus.size() == 0) {
		LOG(INFO) << "Use CPU.";
		Caffe::set_mode(Caffe::CPU);
	}
	else {
		ostringstream s;
		for (int i = 0; i < gpus.size(); ++i) {
			s << (i ? ", " : "") << gpus[i];
		}
		LOG(INFO) << "Using GPUs " << s.str();
#ifndef CPU_ONLY
		cudaDeviceProp device_prop;
		for (int i = 0; i < gpus.size(); ++i) {
			cudaGetDeviceProperties(&device_prop, gpus[i]);
			LOG(INFO) << "GPU " << gpus[i] << ": " << device_prop.name;
		}
#endif
		solver_param.set_device_id(gpus[0]);
		Caffe::SetDevice(gpus[0]);
		Caffe::set_mode(Caffe::GPU);
		Caffe::set_solver_count(gpus.size());
	}

	caffe::SignalHandler signal_handler(
		GetRequestedAction(FLAGS_sigint_effect),
		GetRequestedAction(FLAGS_sighup_effect));

	shared_ptr<caffe::Solver<float> >
		solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));

	solver->SetActionFunction(signal_handler.GetActionFunction());

	if (FLAGS_snapshot.size()) {
		LOG(INFO) << "Resuming from " << FLAGS_snapshot;
		solver->Restore(FLAGS_snapshot.c_str());
	}
	else if (FLAGS_weights.size()) {
		CopyLayers(solver.get(), FLAGS_weights);
	}
	char szBuf[512];
	strcpy(szBuf, FLAGS_weights.c_str());

	
	LOG(INFO) << "Starting Optimization";
	solver->Solve();
	
	LOG(INFO) << "Optimization Done.";
	return 0;
}
RegisterBrewFunction(train);

void loadOneChanneldata(Net<float> * net, cv::Mat image) 
{
	vector<Blob<float>*> input_layer = net->input_blobs();
	int width, height;
	width = input_layer[0]->width();
	height = input_layer[0]->height();
	int size = width*height;

	cv::Mat image_resized;
	cv::resize(image, image_resized, cv::Size(height, width));

	float* input_data = input_layer[0]->mutable_cpu_data();
	int temp, idx;
	for (int i = 0; i < height; ++i) {
		uchar* pdata = image_resized.ptr<uchar>(i);
		for (int j = 0; j < width; ++j) {
			temp =j;

			idx = i*width + j;
			input_data[idx] = (pdata[temp + 0] / 255.0);
		
		}
	}

}

// Test: score a model.
int test1() 
{
	//FLAGS_model = "examples/mnist/lenet.prototxt";
	FLAGS_model = "examples/mnist/lenet_train_test.prototxt";
	FLAGS_weights = "examples/mnist/lenet_iter_10000.caffemodel";

	CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to score.";
	CHECK_GT(FLAGS_weights.size(), 0) << "Need model weights to score.";

	// Set device id and mode
	vector<int> gpus;
	get_gpus(&gpus);
	gpus.clear();

	if (gpus.size() != 0) {
		LOG(INFO) << "Use GPU with device ID " << gpus[0];
#ifndef CPU_ONLY
		cudaDeviceProp device_prop;
		cudaGetDeviceProperties(&device_prop, gpus[0]);
		LOG(INFO) << "GPU device name: " << device_prop.name;
#endif
		Caffe::SetDevice(gpus[0]);
		Caffe::set_mode(Caffe::GPU);
	}
	else {
		LOG(INFO) << "Use CPU.";
		Caffe::set_mode(Caffe::CPU);
	}
	Net<float>  * caffe_net = new  Net<float>(FLAGS_model, caffe::TEST);
	Caffe::set_mode(Caffe::CPU);
	
	caffe_net->CopyTrainedLayersFrom(FLAGS_weights);


	LOG(INFO) << "Running for " << FLAGS_iterations << " iterations.";
	//loadOneChanneldata(caffe_net, "D:/0.jpg");
	caffe::MemoryDataLayer<Net<float>> *m_layer_ = (caffe::MemoryDataLayer<Net<float>>*)caffe_net->layers()[0].get();
	cv::Mat mat = cv::imread("D:/1.jpg",0);
	cv::resize(mat,mat,cv::Size(28,28));

	std::vector<cv::Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(mat);
	testLabel.push_back(0);

	m_layer_->AddMatVector(test,testLabel);
	caffe_net->Forward();
	Blob<float>* reg = caffe_net->output_blobs()[0];
	//shared_ptr<Blob<float>> reg = caffe_net->blob_by_name("ip2");
	const float* reg_data = reg->cpu_data();

	std::string strInfo;
	for (int k = 0; k < 10; ++k)
	{
		char szBuf[256];
		sprintf(szBuf," %.1f", reg_data[k]);
		strInfo += szBuf;
	}

	strInfo += "result";
	return 0;
}
// Test: score a model.
int test() {
	FLAGS_model = "examples/mnist/lenet_train_test.prototxt";
	FLAGS_weights = "examples/mnist/lenet_iter_10000.caffemodel";

	CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to score.";
	CHECK_GT(FLAGS_weights.size(), 0) << "Need model weights to score.";

	// Set device id and mode
	vector<int> gpus;
	get_gpus(&gpus);
	gpus.clear();

	if (gpus.size() != 0) {
		LOG(INFO) << "Use GPU with device ID " << gpus[0];
#ifndef CPU_ONLY
		cudaDeviceProp device_prop;
		cudaGetDeviceProperties(&device_prop, gpus[0]);
		LOG(INFO) << "GPU device name: " << device_prop.name;
#endif
		Caffe::SetDevice(gpus[0]);
		Caffe::set_mode(Caffe::GPU);
	}
	else {
		LOG(INFO) << "Use CPU.";
		Caffe::set_mode(Caffe::CPU);
	}
	// Instantiate the caffe net.
	Net<float> caffe_net(FLAGS_model, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(FLAGS_weights);
	LOG(INFO) << "Running for " << FLAGS_iterations << " iterations.";

	caffe::MemoryDataLayer<Net<float>> *m_layer_ = (caffe::MemoryDataLayer<Net<float>>*)caffe_net.layers()[0].get();
	cv::Mat mat = cv::imread("D:/3.jpg", 0);
	cv::resize(mat, mat, cv::Size(28, 28));

	std::vector<cv::Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(mat);
	testLabel.push_back(0);

	m_layer_->AddMatVector(test, testLabel);
	caffe_net.Forward();

	//Blob<float>* reg = caffe_net.output_blobs()[0];
	shared_ptr<Blob<float>> reg = caffe_net.blob_by_name("ip2");
	const float* reg_data = reg->cpu_data();

	std::string strInfo;
	for (int k = 0; k < 10; ++k)
	{
		char szBuf[256];
		sprintf(szBuf, " %.1f", reg_data[k]);
		strInfo += szBuf;
	}


	vector<int> test_score_output_id;
	vector<float> test_score;
	float loss = 0;
	for (int i = 0; i < FLAGS_iterations; ++i) 
	{
		float iter_loss;
		const vector<Blob<float>*>& result =
			caffe_net.Forward(&iter_loss);

		loss += iter_loss;
		int idx = 0;
		for (int j = 0; j < result.size(); ++j)
		{
			const float* result_vec = result[j]->cpu_data();

			for (int k = 0; k < result[j]->count(); ++k, ++idx) 
			{
				const float score = result_vec[k];
				char szBuf[512];
				sprintf(szBuf, "%d %d %f", i, result[j]->count(), result.size(), score);

				
				if (i == 0) 
				{
					test_score.push_back(score);
					test_score_output_id.push_back(j);
				}
				else 
				{
					test_score[idx] += score;
				}

				const std::string& output_name = caffe_net.blob_names()[caffe_net.output_blob_indices()[j]];
				
				LOG(INFO) << "Batch " << i << ", " << output_name << " = " << score;

				sprintf(szBuf,"%d %s %.1f",i,output_name.c_str(),score);

				int kk = 0;
				kk--;
			}
		}
	}
	loss /= FLAGS_iterations;
	LOG(INFO) << "Loss: " << loss;
	for (int i = 0; i < test_score.size(); ++i) 
	{
		const std::string& output_name = caffe_net.blob_names()[
			caffe_net.output_blob_indices()[test_score_output_id[i]]];
		const float loss_weight = caffe_net.blob_loss_weights()[
			caffe_net.output_blob_indices()[test_score_output_id[i]]];
		std::ostringstream loss_msg_stream;
		const float mean_score = test_score[i] / FLAGS_iterations;
		if (loss_weight) {
			loss_msg_stream << " (* " << loss_weight
				<< " = " << loss_weight * mean_score << " loss)";
		}
		LOG(INFO) << output_name << " = " << mean_score << loss_msg_stream.str();
	}

	return 0;
}
RegisterBrewFunction(test);


// Time: benchmark the execution time of a model.
int time() {
	CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to time.";

	// Set device id and mode
	vector<int> gpus;
	get_gpus(&gpus);
	if (gpus.size() != 0) {
		LOG(INFO) << "Use GPU with device ID " << gpus[0];
		Caffe::SetDevice(gpus[0]);
		Caffe::set_mode(Caffe::GPU);
	}
	else {
		LOG(INFO) << "Use CPU.";
		Caffe::set_mode(Caffe::CPU);
	}
	// Instantiate the caffe net.
	Net<float> caffe_net(FLAGS_model, caffe::TRAIN);

	// Do a clean forward and backward pass, so that memory allocation are done
	// and future iterations will be more stable.
	LOG(INFO) << "Performing Forward";
	// Note that for the speed benchmark, we will assume that the network does
	// not take any input blobs.
	float initial_loss;
	caffe_net.Forward(&initial_loss);
	LOG(INFO) << "Initial loss: " << initial_loss;
	LOG(INFO) << "Performing Backward";
	caffe_net.Backward();

	const vector<shared_ptr<Layer<float> > >& layers = caffe_net.layers();
	const vector<vector<Blob<float>*> >& bottom_vecs = caffe_net.bottom_vecs();
	const vector<vector<Blob<float>*> >& top_vecs = caffe_net.top_vecs();
	const vector<vector<bool> >& bottom_need_backward =
		caffe_net.bottom_need_backward();
	LOG(INFO) << "*** Benchmark begins ***";
	LOG(INFO) << "Testing for " << FLAGS_iterations << " iterations.";
	Timer total_timer;
	total_timer.Start();
	Timer forward_timer;
	Timer backward_timer;
	Timer timer;
	std::vector<double> forward_time_per_layer(layers.size(), 0.0);
	std::vector<double> backward_time_per_layer(layers.size(), 0.0);
	double forward_time = 0.0;
	double backward_time = 0.0;
	for (int j = 0; j < FLAGS_iterations; ++j) {
		Timer iter_timer;
		iter_timer.Start();
		forward_timer.Start();
		for (int i = 0; i < layers.size(); ++i) {
			timer.Start();
			layers[i]->Forward(bottom_vecs[i], top_vecs[i]);
			forward_time_per_layer[i] += timer.MicroSeconds();
		}
		forward_time += forward_timer.MicroSeconds();
		backward_timer.Start();
		for (int i = layers.size() - 1; i >= 0; --i) {
			timer.Start();
			layers[i]->Backward(top_vecs[i], bottom_need_backward[i],
				bottom_vecs[i]);
			backward_time_per_layer[i] += timer.MicroSeconds();
		}
		backward_time += backward_timer.MicroSeconds();
		LOG(INFO) << "Iteration: " << j + 1 << " forward-backward time: "
			<< iter_timer.MilliSeconds() << " ms.";
	}
	LOG(INFO) << "Average time per layer: ";
	for (int i = 0; i < layers.size(); ++i) {
		const caffe::string& layername = layers[i]->layer_param().name();
		LOG(INFO) << std::setfill(' ') << std::setw(10) << layername <<
			"\tforward: " << forward_time_per_layer[i] / 1000 /
			FLAGS_iterations << " ms.";
		LOG(INFO) << std::setfill(' ') << std::setw(10) << layername <<
			"\tbackward: " << backward_time_per_layer[i] / 1000 /
			FLAGS_iterations << " ms.";
	}
	total_timer.Stop();
	LOG(INFO) << "Average Forward pass: " << forward_time / 1000 /
		FLAGS_iterations << " ms.";
	LOG(INFO) << "Average Backward pass: " << backward_time / 1000 /
		FLAGS_iterations << " ms.";
	LOG(INFO) << "Average Forward-Backward: " << total_timer.MilliSeconds() /
		FLAGS_iterations << " ms.";
	LOG(INFO) << "Total Time: " << total_timer.MilliSeconds() << " ms.";
	LOG(INFO) << "*** Benchmark ends ***";
	return 0;
}
RegisterBrewFunction(time);

Net<float> * caffe_net;
void caffe_InitPlateX()
{
	// Print output to stderr (while still logging).
	FLAGS_alsologtostderr = 1;
	// Set version
	gflags::SetVersionString(AS_STRING(CAFFE_VERSION));
	// Usage message.
	gflags::SetUsageMessage("command line brew\n"
		"usage: caffe <command> <args>\n\n"
		"commands:\n"
		"  train           train or finetune a model\n"
		"  test            score a model\n"
		"  device_query    show GPU diagnostic information\n"
		"  time            benchmark model execution time");
	// Run tool or show usage.
	//caffe::GlobalInit(&argc, &argv);
	::google::InitGoogleLogging("ex");

	FLAGS_model = "examples/mnist/lenet_train_test.prototxt";
	FLAGS_weights = "examples/mnist/lenet_iter_10000.caffemodel";

	CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to score.";
	CHECK_GT(FLAGS_weights.size(), 0) << "Need model weights to score.";

	// Set device id and mode
	vector<int> gpus;
	get_gpus(&gpus);
	gpus.clear();

	if (gpus.size() != 0) {
		LOG(INFO) << "Use GPU with device ID " << gpus[0];
#ifndef CPU_ONLY
		cudaDeviceProp device_prop;
		cudaGetDeviceProperties(&device_prop, gpus[0]);
		LOG(INFO) << "GPU device name: " << device_prop.name;
#endif
		Caffe::SetDevice(gpus[0]);
		Caffe::set_mode(Caffe::GPU);
	}
	else {
		LOG(INFO) << "Use CPU.";
		Caffe::set_mode(Caffe::CPU);
	}
	// Instantiate the caffe net.
	caffe_net  = new Net<float>(FLAGS_model, caffe::TEST);
	caffe_net->CopyTrainedLayersFrom(FLAGS_weights);
	LOG(INFO) << "Running for " << FLAGS_iterations << " iterations.";

	
}
//中国车牌
const char strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', /* 没有I */
'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q',
'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
const int numCharacter = 34; /* 没有I和0,10个数字与24个英文字符之和 */

							 //以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
							 //有些后面加数字2的表示在训练时常看到字符的一种变形，也作为训练数据存储
							 //const std::string strChinese[] = {
							 //	"zh_cuan" /* 川 */,  "zh_e" /* 鄂 */,    "zh_gan" /* 赣*/,
							 //	"zh_gan1" /*甘*/,    "zh_gui" /* 贵 */,  "zh_gui1" /* 桂 */,
							 //	"zh_hei" /* 黑 */,   "zh_hu" /* 沪 */,   "zh_ji" /* 冀 */,
							 //	"zh_jin" /* 津 */,   "zh_jing" /* 京 */, "zh_jl" /* 吉 */,
							 //	"zh_liao" /* 辽 */,  "zh_lu" /* 鲁 */,   "zh_meng" /* 蒙 */,
							 //	"zh_min" /* 闽 */,   "zh_ning" /* 宁 */, "zh_qing" /* 青 */,
							 //	"zh_qiong" /* 琼 */, "zh_shan" /* 陕 */, "zh_su" /* 苏 */,
							 //	"zh_sx" /* 晋 */,    "zh_wan" /* 皖 */,  "zh_xiang" /* 湘 */,
							 //	"zh_xin" /* 新 */,   "zh_yu" /* 豫 */,   "zh_yu1" /* 渝 */,
							 //	"zh_yue" /* 粤 */,   "zh_yun" /* 云 */,  "zh_zang" /* 藏 */,
							 //	"zh_zhe" /* 浙 */ };
const std::string strChinese[] = {
	"川" /* 川 */,  "鄂" /* 鄂 */,    "赣" /* 赣*/,
	"甘" /*甘*/,    "贵" /* 贵 */,  "桂" /* 桂 */,
	"黑" /* 黑 */,   "沪" /* 沪 */,   "冀" /* 冀 */,
	"津" /* 津 */,   "京" /* 京 */, "吉" /* 吉 */,
	"辽" /* 辽 */,  "鲁" /* 鲁 */,   "蒙" /* 蒙 */,
	"闽" /* 闽 */,   "宁" /* 宁 */, "青" /* 青 */,
	"琼" /* 琼 */, "陕" /* 陕 */, "苏" /* 苏 */,
	"晋" /* 晋 */,    "皖" /* 皖 */,  "湘" /* 湘 */,
	"新" /* 新 */,   "豫" /* 豫 */,   "渝" /* 渝 */,
	"粤" /* 粤 */,   "云" /* 云 */,  "藏" /* 藏 */,
	"浙" /* 浙 */ };


float plateX_recoCh(cv::Mat img, std::string& outR, bool bNum)
{

	const int numChinese = 31;

	caffe::MemoryDataLayer<Net<float>> *m_layer_ = (caffe::MemoryDataLayer<Net<float>>*)caffe_net->layers()[0].get();
	cv::Mat mat;
	cv::resize(img, mat, cv::Size(28, 28));
	std::vector<cv::Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(mat);
	testLabel.push_back(0);

	loadOneChanneldata(caffe_net, img);
	//m_layer_->AddMatVector(test, testLabel);
	caffe_net->Forward();

	//Blob<float>* reg = caffe_net.output_blobs()[0];
	shared_ptr<Blob<float>> reg = caffe_net->blob_by_name("ip2");
	const float* reg_data = reg->cpu_data();

	std::string strInfo;
	float maxf = -1000;
	int maxIndex = 0;

	if (bNum)
	{
		for (int k = 0; k < 10; ++k)
		{
			char szBuf[256];
			sprintf(szBuf, " %.1f", reg_data[k]);
			strInfo += szBuf;
			if (reg_data[k] > maxf)
			{
				maxf = reg_data[k];
				maxIndex = k;
			}
		}
	}
	else
	{
		for (int k = 10; k < numCharacter; ++k)
		{
			char szBuf[256];
			sprintf(szBuf, " %.1f", reg_data[k]);
			strInfo += szBuf;
			if (reg_data[k] > maxf)
			{
				maxf = reg_data[k];
				maxIndex = k;
			}
		}
	}

	char outC;
	string outStr;
	if (maxIndex < numCharacter)
	{
		outC = strCharacters[maxIndex];
		char szBuf[256];
		sprintf(szBuf, "%c", outC);
		outR = szBuf;
	}
	else
	{
		outStr = strChinese[maxIndex - numCharacter];
		outR = outStr;
	}

	return maxf;
}

float plateX_recoMat(cv::Mat img, std::string& outR,bool bChinese)
{
	
	const int numChinese = 31;

	caffe::MemoryDataLayer<Net<float>> *m_layer_ = (caffe::MemoryDataLayer<Net<float>>*)caffe_net->layers()[0].get();
	cv::Mat mat;
	cv::resize(img, mat, cv::Size(28, 28));
	std::vector<cv::Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(mat);
	testLabel.push_back(0);


	loadOneChanneldata(caffe_net,img);
	//m_layer_->AddMatVector(test, testLabel);
	caffe_net->Forward();

	//Blob<float>* reg = caffe_net.output_blobs()[0];
	shared_ptr<Blob<float>> reg = caffe_net->blob_by_name("ip2");
	const float* reg_data = reg->cpu_data();

	std::string strInfo;
	float maxf = -1000;
	int maxIndex = 0;

	if (bChinese)
	{
		for (int k = numCharacter; k < 65; ++k)
		{
			char szBuf[256];
			sprintf(szBuf, " %.1f", reg_data[k]);
			strInfo += szBuf;
			if (reg_data[k] > maxf)
			{
				maxf = reg_data[k];
				maxIndex = k;
			}
		}
	}
	else
	{
		for (int k = 0; k < numCharacter; ++k)
		{
			char szBuf[256];
			sprintf(szBuf, " %.1f", reg_data[k]);
			strInfo += szBuf;
			if (reg_data[k] > maxf)
			{
				maxf = reg_data[k];
				maxIndex = k;
			}
		}
	}

	char outC;
	string outStr;
	if (maxIndex < numCharacter)
	{
		outC = strCharacters[maxIndex];
		char szBuf[256];
		sprintf(szBuf, "%c", outC);
		outR = szBuf;
	}
	else
	{
		outStr = strChinese[maxIndex - numCharacter];
		outR = outStr;
	}

	int kk = 0;
	for (int j = 0; j <= 3; ++j)
	{
		kk = 0;
	}
	strInfo += "ok";
	return maxf;
}
void plateX_reco(char * szBuf,std::string& outR)
{
	//中国车牌
	const char strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', /* 没有I */
		'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q',
		'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	const int numCharacter = 34; /* 没有I和0,10个数字与24个英文字符之和 */

								 //以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
								 //有些后面加数字2的表示在训练时常看到字符的一种变形，也作为训练数据存储
	const std::string strChinese[] = {
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
		"zh_zhe" /* 浙 */ };

	const int numChinese = 31;

	caffe::MemoryDataLayer<Net<float>> *m_layer_ = (caffe::MemoryDataLayer<Net<float>>*)caffe_net->layers()[0].get();
	cv::Mat mat = cv::imread(szBuf, 0);
	/*cv::resize(mat, mat, cv::Size(28, 28));
	std::vector<cv::Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(mat);
	testLabel.push_back(0);*/


	//m_layer_->AddMatVector(test, testLabel);

	int dstW = 28; int dstH = 28;
	unsigned char * dstImg = new unsigned char[dstW *dstH * 3];
	//resized.convertTo(resized, CV_32FC1, 0.0078125, -127.5*0.0078125);

	ResizeImage(mat.data, mat.cols, mat.rows, dstImg, dstW, dstH, 3);

	const vector<Blob<float>*> &intput_blobs = caffe_net->input_blobs();
	//float *blob_data = intput_blobs[0]->mutable_cpu_data();

	//loadOneChanneldata(caffe_net, szBuf);

	
	
	caffe_net->Forward();

	//Blob<float>* reg = caffe_net.output_blobs()[0];
	shared_ptr<Blob<float>> reg = caffe_net->blob_by_name("ip2");
	const float* reg_data = reg->cpu_data();

	std::string strInfo;
	float maxf = -1000;
	int maxIndex = 0;
	for (int k = 0; k < 65; ++k)
	{
		char szBuf[256];
		sprintf(szBuf, " %.1f", reg_data[k]);
		strInfo += szBuf;
		if (reg_data[k] > maxf)
		{
			maxf = reg_data[k];
			maxIndex = k;
		}
	}
	char outC;
	string outStr;
	if (maxIndex < numCharacter)
	{
		outC = strCharacters[maxIndex];
		char szBuf[256];
		sprintf(szBuf,"%c",outC);
		outR = szBuf;
	}
	else
	{
		outStr = strChinese[maxIndex - numCharacter];
		outR = outStr;
	}

	int kk = 0;
	for (int j = 0; j <= 3; ++j)
	{
		kk = 0;
	}
	strInfo += "ok";
}

int caffemain(int argc, char** argv) {
	// Print output to stderr (while still logging).
	FLAGS_alsologtostderr = 1;
	// Set version
	gflags::SetVersionString(AS_STRING(CAFFE_VERSION));
	// Usage message.
	gflags::SetUsageMessage("command line brew\n"
		"usage: caffe <command> <args>\n\n"
		"commands:\n"
		"  train           train or finetune a model\n"
		"  test            score a model\n"
		"  device_query    show GPU diagnostic information\n"
		"  time            benchmark model execution time");
	// Run tool or show usage.
	caffe::GlobalInit(&argc, &argv);

	//test();
	//return 1;

	if (argc == 2) 
	{

#ifdef WITH_PYTHON_LAYER
		try {
#endif
			return GetBrewFunction(caffe::string(argv[1]))();
#ifdef WITH_PYTHON_LAYER
		}
		catch (bp::error_already_set) {
			PyErr_Print();
			return 1;
		}
#endif
	}
	else
	{
		//gflags::ShowUsageWithFlagsRestrict(argv[0], "tools/caffe");
		gflags::ShowUsageWithFlagsRestrict(argv[0], "tools/caffe");
	}
}
