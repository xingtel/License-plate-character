#include "align.h"




void Align::Alignment(unsigned char * src_img,int srw,int srh,
	const float* const feat_points,
	unsigned char * dst_img)
{
	float tform[6] = { 0 };
	//计算旋转缩放平移系数
	{
		double sum_x = 0, sum_y = 0;
		double sum_u = 0, sum_v = 0;
		double sum_xx_yy = 0;
		double sum_ux_vy = 0;
		double sum_vx__uy = 0;
		for (int c = 0; c < points_num_; ++c) {
			int x_off = c * 2;
			int y_off = x_off + 1;
			sum_x += std_points[c * 2];
			sum_y += std_points[c * 2 + 1];
			sum_u += feat_points[x_off];
			sum_v += feat_points[y_off];
			sum_xx_yy += std_points[c * 2] * std_points[c * 2] +
				std_points[c * 2 + 1] * std_points[c * 2 + 1];
			sum_ux_vy += std_points[c * 2] * feat_points[x_off] +
				std_points[c * 2 + 1] * feat_points[y_off];
			sum_vx__uy += feat_points[y_off] * std_points[c * 2] -
				feat_points[x_off] * std_points[c * 2 + 1];
		}
		double q = sum_u - sum_x * sum_ux_vy / sum_xx_yy
			+ sum_y * sum_vx__uy / sum_xx_yy;
		double p = sum_v - sum_y * sum_ux_vy / sum_xx_yy
			- sum_x * sum_vx__uy / sum_xx_yy;

		double r = points_num_ - (sum_x * sum_x + sum_y * sum_y) / sum_xx_yy;
		double a = (sum_ux_vy - sum_x * q / r - sum_y * p / r) / sum_xx_yy;
		double b = (sum_vx__uy + sum_y * q / r - sum_x * p / r) / sum_xx_yy;
		double c = q / r;
		double d = p / r;
		tform[0] = tform[4] = a;
		tform[1] = -b;
		tform[3] = b;
		tform[2] = c;
		tform[5] = d;
	}

	int channels = 3;
	int src_h = srh;
	int src_w = srw;


	
	//uchar* src_img_ptr = (uchar*)src_img.ptr<uchar>(0);

	float* theta_data = tform;
	int dst_h = crop_height();
	int dst_w = crop_width();

	//scale在cubic插值的时候用到，在双线性插值用不到。
	float scale = sqrt(theta_data[0] * theta_data[0] + theta_data[3] * theta_data[3]);
	scale = 1.f / scale;
	float padding_ex = (padding_ * 2 + 1);
	float pad_h = dst_h *padding_;
	float pad_w = dst_w *padding_;
	//get 4CornerPts in srcImg
	//左上角 右上角 左下角 右下角
	{
		float sC[4][2];
		sC[0][0] = 0;sC[0][1] = 0;
		sC[1][0] = 0;sC[1][1] = dst_w - 1;
		sC[2][0] = dst_h - 1; sC[2][1] = 0;
		sC[3][0] = dst_h - 1; sC[3][1] = dst_w - 1;


		/*sC.push_back(Vec2f(0, 0)); sC.push_back(Vec2f(0, dst_w - 1));
		sC.push_back(Vec2f(dst_h - 1, 0)); sC.push_back(Vec2f(dst_h - 1, dst_w - 1));*/

		for (size_t i = 0; i < 4; i++)
		{
			int x = sC[i][0], y = sC[i][1];
			int tx = padding_ex * x - pad_h;
			int ty = padding_ex * y - pad_w;
			float src_y = theta_data[0] * ty + theta_data[1] * tx + theta_data[2];
			float src_x = theta_data[3] * ty + theta_data[4] * tx + theta_data[5];
			corners[2 * i] = src_y; 			corners[2 * i + 1] = src_x;
		}
	}
	if (dst_h*dst_w < 16)
	{
		return;
	}
	//#pragma omp parallel for num_threads(4)
	for (int x = 0; x < dst_h; ++x)
	{
		float tx = padding_ex * x - pad_h;
		float temp1 = theta_data[1] * tx + theta_data[2];
		float temp2 = theta_data[4] * tx + theta_data[5];
	

		int step = dst_w*x*channels;
		int jump = 0;
		for (int y = 0; y < dst_w; ++y)
		{
			float ty = padding_ex * y - pad_w;
			//float src_y = theta_data[0] * ty + theta_data[1] * tx + theta_data[2];
			float src_y = theta_data[0] * ty + temp1;
			//float src_x = theta_data[3] * ty + theta_data[4] * tx + theta_data[5];
			float src_x = theta_data[3] * ty + temp2;

			//double src_y = theta_data[0] * y + theta_data[1] * x + theta_data[2];
			//double src_x = theta_data[3] * y + theta_data[4] * x + theta_data[5];
			
			
				dst_img[step + jump + 0]=	(unsigned char)Sampling(src_img,
						0, src_h, src_w,
						channels, src_x, src_y, scale);

				dst_img[step + jump + 1] = (unsigned char)Sampling(src_img,
					1, src_h, src_w,
					channels, src_x, src_y, scale);

				dst_img[step + jump + 2] = (unsigned char)Sampling(src_img,
					2, src_h, src_w,
					channels, src_x, src_y, scale);
			

			jump += channels;
		}
	}
}
float Align::Sampling(const unsigned char* const feat_map,
	int c, int H, int W, int C, float x, float y, float scale)
{
	//if (type_ == "linear") 
	{
		// bilinear subsampling
		int ux = floor(x), uy = floor(y);
		float ans = 0;
		int ansI = 0;
		if (ux >= 0 && ux < H - 1 && uy >= 0 && uy < W - 1) {
			int offset = (ux * W + uy) * C + c;
			float cof_x = x - ux;
			float cof_y = y - uy;
#define SHIFTL
			//整数乘和移位代替浮点乘法。在windows上差别不大，在arm上应该有收益吧。
#ifdef SHIFTL
			int cof_xI = cof_x * (1 << 16);
			int cof_xI_inv = (1 << 16) - cof_xI;
			int cof_yI = cof_y * (1 << 16);
			int cof_yI_inv = (1 << 16) - cof_yI;
#define SHIFT(a) (((a) /*+ (1 << 15)*/) >> 16)
			ansI = SHIFT(cof_yI_inv * feat_map[offset]) + SHIFT(cof_yI * feat_map[offset + C]);
			//int tx = SHIFT(cof_yI_inv * feat_map[offset + W * C]) + SHIFT(cof_yI * feat_map[offset + W * C + C]);
			ansI = SHIFT(cof_xI_inv * ansI)
				+ SHIFT(cof_xI * (SHIFT(cof_yI_inv * feat_map[offset + W * C])
					+ SHIFT(cof_yI * feat_map[offset + W * C + C])));
#undef SHIFT
#else
			ans = (1 - cof_y) * feat_map[offset] + cof_y * feat_map[offset + C];
			ans = (1 - cof_x) * ans + cof_x * ((1 - cof_y) * feat_map[offset + W * C]
				+ cof_y * feat_map[offset + W * C + C]);
#endif
		}
		return ansI;
		return ans;
	}
	//int ux = floor(x), uy = floor(y);
	//return(feat_map[(ux * W + uy) * C + c]);
}
//Alignment校准
//输入原图像，和68点shape，返回截取后的脸dst_img
//void Align::Alignment(unsigned char *src_img,int sw,int sh,
//	const vector<Vec2f> shape,
//	unsigned char * dst_img)
//{
//	float* points;
//	points = new float[10];
//	Vec2f l(0, 0), r(0, 0);
//	float cnt = 0.f;
//	for (unsigned long i = 36; i <= 41; ++i)
//	{
//		l += shape[i];
//		++cnt;
//	}
//	l /= cnt;
//	cnt = 0.f;
//	for (unsigned long i = 42; i <= 47; ++i)
//	{
//		r += shape[i];
//		++cnt;
//	}
//	r /= cnt;
//	points[0] = l[0]; points[1] = l[1];
//	points[2] = r[0]; points[3] = r[1];
//	points[4] = shape[30][0]; points[5] = shape[30][1];
//	points[6] = shape[48][0]; points[7] = shape[48][1];
//	points[8] = shape[54][0]; points[9] = shape[54][1];
//	Alignment(src_img,sw,sh, points, dst_img);
//	delete points;
//}