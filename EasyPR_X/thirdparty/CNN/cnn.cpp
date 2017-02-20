#include "cnn.hpp"
#include <memory.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define LENGTH_KERNEL0	3
#define LENGTH_KERNEL1	4

#define LENGTH_FEATURE0	20
#define LENGTH_FEATURE1	18//(LENGTH_INPUT - LENGTH_KERNEL + 1)
#define LENGTH_FEATURE2	9//(LENGTH_INPUT_1 >> 1)
#define LENGTH_FEATURE3	6//(LENGTH_INPUT_2 - LENGTH_KERNEL + 1)
#define	LENGTH_FEATURE4	3//(LENGTH_INPUT_3 >> 1)
#define LENGTH_FEATURE5	1//(LENGTH_INPUT_4 - LENGTH_KERNEL + 1)
#define LENGTH_FEATURE6	1

#define LAYER0			1
#define LAYER1			6
#define LAYER2			6
#define LAYER3			16
#define LAYER4			16
#define LAYER5			120
#define OUTPUT          OUTPUT_SIZE

#define ALPHA 0.05

typedef unsigned char uint8;
typedef uint8 image[LENGTH_FEATURE0][LENGTH_FEATURE0];


typedef struct CNN
{
	double weight0_1[LAYER0][LAYER1][LENGTH_KERNEL0][LENGTH_KERNEL0];
	double weight2_3[LAYER2][LAYER3][LENGTH_KERNEL1][LENGTH_KERNEL1];
	double weight4_5[LAYER4][LAYER5][LENGTH_KERNEL0][LENGTH_KERNEL0];
	double weight5_6[LAYER5 * LENGTH_FEATURE6 * LENGTH_FEATURE6][OUTPUT];

	double bias0_1[LAYER1];
	double bias2_3[LAYER3];
	double bias4_5[LAYER5];
	double bias5_6[OUTPUT];

}CNN;

typedef struct Feature
{
	double layer0[LAYER0][LENGTH_FEATURE0][LENGTH_FEATURE0];
	double layer1[LAYER1][LENGTH_FEATURE1][LENGTH_FEATURE1];
	double layer2[LAYER2][LENGTH_FEATURE2][LENGTH_FEATURE2];
	double layer3[LAYER3][LENGTH_FEATURE3][LENGTH_FEATURE3];
	double layer4[LAYER4][LENGTH_FEATURE4][LENGTH_FEATURE4];
	double layer5[LAYER5][LENGTH_FEATURE5][LENGTH_FEATURE5];
	double output[OUTPUT];
}Feature;

void TrainBatch(CNN *cnn, image *inputs, const char(*resMat)[OUTPUT], uint8 *labels, int batchSize);

void Train(CNN *cnn, image input, const char(*resMat)[OUTPUT], uint8 label);

uint8 Predict(CNN *cnn, image input, const char(*resMat)[OUTPUT], uint8 count);

void Initial(CNN *cnn);

#define GETLENGTH(array) (sizeof(array)/sizeof(*(array)))

#define GETCOUNT(array)  (sizeof(array)/sizeof(double))

#define FOREACH(i,count) for (int i = 0; i < count; ++i)

#define CONVOLUTE_VALID(input,output,weight)											\
{																						\
	FOREACH(o0,GETLENGTH(output))														\
		FOREACH(o1,GETLENGTH(*(output)))												\
			FOREACH(w0,GETLENGTH(weight))												\
				FOREACH(w1,GETLENGTH(*(weight)))										\
					(output)[o0][o1] += (input)[o0 + w0][o1 + w1] * (weight)[w0][w1];	\
}

#define CONVOLUTE_FULL(input,output,weight)												\
{																						\
	FOREACH(i0,GETLENGTH(input))														\
		FOREACH(i1,GETLENGTH(*(input)))													\
			FOREACH(w0,GETLENGTH(weight))												\
				FOREACH(w1,GETLENGTH(*(weight)))										\
					(output)[i0 + w0][i1 + w1] += (input)[i0][i1] * (weight)[w0][w1];	\
}

#define CONVOLUTION_FORWARD(input,output,weight,bias,action)					\
{																				\
	for (int x = 0; x < GETLENGTH(weight); ++x)									\
		for (int y = 0; y < GETLENGTH(*weight); ++y)							\
			CONVOLUTE_VALID(input[x], output[y], weight[x][y]);					\
	FOREACH(j, GETLENGTH(output))												\
		FOREACH(i, GETCOUNT(output[j]))											\
		((double *)output[j])[i] = action(((double *)output[j])[i] + bias[j]);	\
}

#define CONVOLUTION_BACKWARD(input,inerror,outerror,weight,wd,bd,actiongrad)\
{																			\
	for (int x = 0; x < GETLENGTH(weight); ++x)								\
		for (int y = 0; y < GETLENGTH(*weight); ++y)						\
			CONVOLUTE_FULL(outerror[y], inerror[x], weight[x][y]);			\
	FOREACH(i, GETCOUNT(inerror))											\
		((double *)inerror)[i] *= actiongrad(((double *)input)[i]);			\
	FOREACH(j, GETLENGTH(outerror))											\
		FOREACH(i, GETCOUNT(outerror[j]))									\
		bd[j] += ((double *)outerror[j])[i];								\
	for (int x = 0; x < GETLENGTH(weight); ++x)								\
		for (int y = 0; y < GETLENGTH(*weight); ++y)						\
			CONVOLUTE_VALID(input[x], wd[x][y], outerror[y]);				\
}


#define SUBSAMP_MAX_FORWARD(input,output)														\
{																								\
	const int len0 = GETLENGTH(*(input)) / GETLENGTH(*(output));								\
	const int len1 = GETLENGTH(**(input)) / GETLENGTH(**(output));								\
	FOREACH(i, GETLENGTH(output))																\
	FOREACH(o0, GETLENGTH(*(output)))															\
	FOREACH(o1, GETLENGTH(**(output)))															\
	{																							\
		int x0 = 0, x1 = 0, ismax;																\
		FOREACH(l0, len0)																		\
			FOREACH(l1, len1)																	\
		{																						\
			ismax = input[i][o0*len0 + l0][o1*len1 + l1] > input[i][o0*len0 + x0][o1*len1 + x1];\
			x0 += ismax * (l0 - x0);															\
			x1 += ismax * (l1 - x1);															\
		}																						\
		output[i][o0][o1] = input[i][o0*len0 + x0][o1*len1 + x1];								\
	}																							\
}

#define SUBSAMP_MAX_BACKWARD(input,inerror,outerror)											\
{																								\
	const int len0 = GETLENGTH(*(inerror)) / GETLENGTH(*(outerror));							\
	const int len1 = GETLENGTH(**(inerror)) / GETLENGTH(**(outerror));							\
	FOREACH(i, GETLENGTH(outerror))																\
	FOREACH(o0, GETLENGTH(*(outerror)))															\
	FOREACH(o1, GETLENGTH(**(outerror)))														\
	{																							\
		int x0 = 0, x1 = 0, ismax;																\
		FOREACH(l0, len0)																		\
			FOREACH(l1, len1)																	\
		{																						\
			ismax = input[i][o0*len0 + l0][o1*len1 + l1] > input[i][o0*len0 + x0][o1*len1 + x1];\
			x0 += ismax * (l0 - x0);															\
			x1 += ismax * (l1 - x1);															\
		}																						\
		inerror[i][o0*len0 + x0][o1*len1 + x1] = outerror[i][o0][o1];							\
	}																							\
}

#define DOT_PRODUCT_FORWARD(input,output,weight,bias,action)				\
{																			\
	for (int x = 0; x < GETLENGTH(weight); ++x)								\
		for (int y = 0; y < GETLENGTH(*weight); ++y)						\
			((double *)output)[y] += ((double *)input)[x] * weight[x][y];	\
	FOREACH(j, GETLENGTH(bias))												\
		((double *)output)[j] = action(((double *)output)[j] + bias[j]);	\
}

#define DOT_PRODUCT_BACKWARD(input,inerror,outerror,weight,wd,bd,actiongrad)	\
{																				\
	for (int x = 0; x < GETLENGTH(weight); ++x)									\
		for (int y = 0; y < GETLENGTH(*weight); ++y)							\
			((double *)inerror)[x] += ((double *)outerror)[y] * weight[x][y];	\
	FOREACH(i, GETCOUNT(inerror))												\
		((double *)inerror)[i] *= actiongrad(((double *)input)[i]);				\
	FOREACH(j, GETLENGTH(outerror))												\
		bd[j] += ((double *)outerror)[j];										\
	for (int x = 0; x < GETLENGTH(weight); ++x)									\
		for (int y = 0; y < GETLENGTH(*weight); ++y)							\
			wd[x][y] += ((double *)input)[x] * ((double *)outerror)[y];			\
}

double tanhgrad(double y)
{
	return 1 - y*y;
}

static void normalize(uint8 input[], double output[], int count)
{
	double mean = 0, std = 0;
	FOREACH(i, count)
	{
		mean += input[i];
		std += input[i] * input[i];
	}
	mean /= count;
	std = sqrt(std / count - mean*mean);
	FOREACH(i, count)
		output[i] = (input[i] - mean) / std;
}

static void forward(CNN *cnn, Feature *features, double(*action)(double))
{
	CONVOLUTION_FORWARD(features->layer0, features->layer1, cnn->weight0_1, cnn->bias0_1, action);
	SUBSAMP_MAX_FORWARD(features->layer1, features->layer2);
	CONVOLUTION_FORWARD(features->layer2, features->layer3, cnn->weight2_3, cnn->bias2_3, action);
	SUBSAMP_MAX_FORWARD(features->layer3, features->layer4);
	CONVOLUTION_FORWARD(features->layer4, features->layer5, cnn->weight4_5, cnn->bias4_5, action);
	DOT_PRODUCT_FORWARD(features->layer5, features->output, cnn->weight5_6, cnn->bias5_6, action);
}

static void backward(CNN *cnn, CNN *deltas, Feature *errors, Feature *features, double(*actiongrad)(double))
{
	DOT_PRODUCT_BACKWARD(features->layer5, errors->layer5, errors->output, cnn->weight5_6, deltas->weight5_6, deltas->bias5_6, actiongrad);
	CONVOLUTION_BACKWARD(features->layer4, errors->layer4, errors->layer5, cnn->weight4_5, deltas->weight4_5, deltas->bias4_5, actiongrad);
	SUBSAMP_MAX_BACKWARD(features->layer3, errors->layer3, errors->layer4);
	CONVOLUTION_BACKWARD(features->layer2, errors->layer2, errors->layer3, cnn->weight2_3, deltas->weight2_3, deltas->bias2_3, actiongrad);
	SUBSAMP_MAX_BACKWARD(features->layer1, errors->layer1, errors->layer2);
	CONVOLUTION_BACKWARD(features->layer0, errors->layer0, errors->layer1, cnn->weight0_1, deltas->weight0_1, deltas->bias0_1, actiongrad);
}

static void load_input(Feature *features, image input)
{
	normalize((uint8 *)input, (double *)features->layer0, sizeof(image) / sizeof(uint8));
}

static void load_target(Feature *features, Feature *errors, const char *label, double(*actiongrad)(double))
{
	double *output = (double *)features->output;
	double *error = (double *)errors->output;
	const int outlen = GETCOUNT(features->output);
	FOREACH(i, outlen)
		error[i] = (label[i] - output[i])*actiongrad(output[i]);
}

static uint8 get_result(Feature *features, const char(*labels)[OUTPUT], uint8 count)
{
	double *output = (double *)features->output;
	const int outlen = GETCOUNT(features->output);
	uint8 result = 0;
	double minvalue = DBL_MAX;
	for (uint8 i = 0; i < count; ++i)
	{
		double sum = 0;
		FOREACH(j, outlen)
			sum += (output[j] - labels[i][j])*(output[j] - labels[i][j]);
		if (sum < minvalue)
		{
			minvalue = sum;
			result = i;
		}
	}
	return result;
}

static double f64rand()
{
	static int randbit = 0;
	if (!randbit)
	{
		srand((unsigned)time(0));
		for (int i = RAND_MAX; i; i >>= 1, ++randbit);
	}
	unsigned long long lvalue = 0x4000000000000000L;
	int i = 52 - randbit;
	for (; i > 0; i -= randbit)
		lvalue |= (unsigned long long)rand() << i;
	lvalue |= (unsigned long long)rand() >> -i;
	return *(double *)&lvalue - 3;
}


void TrainBatch(CNN *cnn, image *inputs, const char(*resMat)[OUTPUT], uint8 *labels, int batchSize)
{
	double buffer[GETCOUNT(CNN)] = { 0 };
	int i = 0;
#pragma omp parallel for
	for (i = 0; i < batchSize; ++i)
	{
		Feature features = { 0 };
		Feature errors = { 0 };
		CNN	deltas = { 0 };
		load_input(&features, inputs[i]);
		forward(cnn, &features, tanh);
		load_target(&features, &errors, resMat[labels[i]], tanhgrad);
		backward(cnn, &deltas, &errors, &features, tanhgrad);
#pragma omp critical
		{
			FOREACH(j, GETCOUNT(CNN))
				buffer[j] += ((double *)&deltas)[j];
		}
	}
	double k = ALPHA / batchSize;
	FOREACH(i, GETCOUNT(CNN))
		((double *)cnn)[i] += k * buffer[i];
}

void Train(CNN *cnn, image input, const char(*resMat)[OUTPUT], uint8 label)
{
	Feature features = { 0 };
	Feature errors = { 0 };
	CNN deltas = { 0 };
	load_input(&features, input);
	forward(cnn, &features, tanh);
	load_target(&features, &errors, resMat[label], tanhgrad);
	backward(cnn, &deltas, &errors, &features, tanhgrad);
	FOREACH(i, GETCOUNT(CNN))
		((double *)cnn)[i] += ALPHA * ((double *)&deltas)[i];
}

uint8 Predict(CNN *cnn, image input, const char(*resMat)[OUTPUT], uint8 count)
{
	Feature features = { 0 };
	load_input(&features, input);
	forward(cnn, &features, tanh);
	return get_result(&features, resMat, count);
}

void Initial(CNN *cnn)
{
	for (double *pos = (double *)cnn->weight0_1; pos < (double *)cnn->bias0_1; *pos++ = f64rand());
	for (double *pos = (double *)cnn->weight0_1; pos < (double *)cnn->weight2_3; *pos++ *= sqrt(6.0 / (LENGTH_KERNEL0 * LENGTH_KERNEL0 * (LAYER0 + LAYER1))));
	for (double *pos = (double *)cnn->weight2_3; pos < (double *)cnn->weight4_5; *pos++ *= sqrt(6.0 / (LENGTH_KERNEL0 * LENGTH_KERNEL0 * (LAYER2 + LAYER3))));
	for (double *pos = (double *)cnn->weight4_5; pos < (double *)cnn->weight5_6; *pos++ *= sqrt(6.0 / (LENGTH_KERNEL1 * LENGTH_KERNEL1 * (LAYER4 + LAYER5))));
	for (double *pos = (double *)cnn->weight5_6; pos < (double *)cnn->bias0_1; *pos++ *= sqrt(6.0 / (LAYER5 + OUTPUT)));
	for (int *pos = (int *)cnn->bias0_1; pos < (int *)(cnn + 1); *pos++ = 0);
}

namespace cv {
	namespace ml {

		CNN_MLP::CNN_MLP()
		{

		}

		CNN_MLP::~CNN_MLP()
		{
			free(this->cnn);
			free(this->resMat);
		}

		CV_WRAP int CNN_MLP::getVarCount() const
		{
			return 1;
		}

		CV_WRAP bool CNN_MLP::empty() const
		{
			return true;
		}

		CV_WRAP bool CNN_MLP::isTrained() const
		{
			return false;
		}

		CV_WRAP bool CNN_MLP::isClassifier() const
		{
			return true;
		}

		CV_WRAP bool CNN_MLP::train(const Ptr<TrainData>& trainData, int flags)
		{
			return CV_WRAP bool();
		}

		CV_WRAP bool CNN_MLP::train(InputArray samples, int layout, InputArray responses)
		{
			return CV_WRAP bool();
		}

		CV_WRAP float CNN_MLP::calcError(const Ptr<TrainData>& data, bool test, OutputArray resp) const
		{
			return CV_WRAP float();
		}

		CV_WRAP float CNN_MLP::predict(InputArray samples, OutputArray results, int flags) const
		{
			int count = results.rows();
			for (int i = 0; i < count; ++i)
			{
				image img = { 0 };
				Mat &result = results.getMat(i);
				Mat &sample = samples.getMat(i);
				memcpy(img, sample.data, sizeof(img));
			
				uint8 res = ::Predict(this->cnn, img, this->resMat, this->resCount);
				for (int i = 0; i < result.cols; ++i)
					result.at<float>(i) = i == res;
			}
			return CV_WRAP float();
		}


		CV_WRAP void CNN_MLP::read(FILE * fp)
		{
			fseek(fp, 0, SEEK_END);
			long len = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			this->resCount = (uint8)((len - sizeof(::CNN)) / sizeof(*resMat));
			this->cnn = (::CNN *)malloc(sizeof(::CNN));
			this->resMat = (ResVec *)calloc(this->resCount, sizeof(ResVec));
			fread(this->cnn, 1, sizeof(::CNN), fp);
			fread(this->resMat, sizeof(ResVec), this->resCount, fp);
		}
	}
}
