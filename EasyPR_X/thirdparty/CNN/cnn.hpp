#pragma once

#include <opencv2/ml.hpp>

#define OUTPUT_SIZE 84

typedef char ResVec[OUTPUT_SIZE];

struct CNN;

namespace cv
{
	namespace ml
	{
		class CNN_MLP : public StatModel
		{
		public:

			CV_WRAP int getVarCount() const override;

			CV_WRAP bool empty() const override;

			CV_WRAP bool isTrained() const override;

			CV_WRAP bool isClassifier() const override;

			CV_WRAP bool train(const Ptr<TrainData>& trainData, int flags = 0) override;

			CV_WRAP bool train(InputArray samples, int layout, InputArray responses) override;

			CV_WRAP float calcError(const Ptr<TrainData>& data, bool test, OutputArray resp) const override;

			CV_WRAP float predict(InputArray samples, OutputArray results = noArray(), int flags = 0) const override;

			CV_WRAP void read(FILE *fp);

			template<typename _Tp> static Ptr<_Tp> load(const String& filename, const String& objname = String())
			{
				Ptr<_Tp> obj = _Tp::create();
				FILE *fp = fopen(filename.c_str(), "rb");
				obj->read(fp);
				fclose(fp);
				return obj;
			}

			CV_WRAP static Ptr<CNN_MLP> create() 
			{
				return Ptr<CNN_MLP>(new CNN_MLP());
			}

			~CNN_MLP();

		private:
			explicit CNN_MLP();

			
			ResVec *resMat = 0;
			unsigned char resCount = 0;
			::CNN *cnn;

		};
	}
}