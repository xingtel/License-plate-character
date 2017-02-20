#include "leaky_layer.hpp"
namespace caffe {
	//add by yang
	template <typename Dtype>
	void LeakyLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top) {
		NeuronLayer<Dtype>::LayerSetUp(bottom, top);
		CHECK_NE(top[0], bottom[0]) << this->type() << " Layer does not "
			"allow in-place computation.";
	}

	template <typename Dtype>
	void LeakyLayer<Dtype>::Forward_cpu(
		const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
		const int count = top[0]->count();
		Dtype* top_data = top[0]->mutable_cpu_data();
		const Dtype* bottom_data = bottom[0]->cpu_data();
		for (int i = 0; i < count; ++i) {
			if (bottom_data[i] > 0)
				top_data[i] = bottom_data[i];
			else
				top_data[i] = 0.1*bottom_data[i];
			//top_data[i] = bottom_data[i] > 0£¿ bottom_data[i]: (Dtype(0.1)*bottom_data[i]);
		}
	}

#ifdef CPU_ONLY
	STUB_GPU(LeakyLayer);
#endif
	INSTANTIATE_CLASS(LeakyLayer);
	REGISTER_LAYER_CLASS(Leaky);
}