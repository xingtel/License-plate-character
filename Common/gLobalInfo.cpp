#include "globalInfo.h"
#include <vector>

#ifdef WIN32
char g_rootpath[512] = "./";
#else 
#ifdef ANDROID
std::string  g_rootpath = "./sdcard/ShowO/data/modle/CLM/";
#else
std::string g_rootpath="./"
#endif
#endif

bool  ISFileExist(char * szFileName)
{
	FILE * pfile = fopen(szFileName,"r");
	if(pfile != NULL)
	{
		fclose(pfile);
		return true;
	}
	return false;
}


int min_int(int x, int y) { return (x <= y ? x : y); }
int max_int(int x, int y) { return (x <= y ? y : x); }
void ResizeImage(unsigned char *src, int nSrcWidth, int nSrcHeight, unsigned char *dst, int nDstWidth, int nDstHeight, int nChannel)
{

	int step1 = nSrcWidth*nChannel;
	int step2 = nDstWidth*nChannel;
	int j;
	int i;
	int k;

	unsigned char* data1 = (unsigned char*)src;
	unsigned char* data2 = (unsigned char*)dst;

	long xrIntFloat_16 = ((nSrcWidth) << 16) / nDstWidth + 1;
	long yrIntFloat_16 = ((nSrcHeight) << 16) / nDstHeight + 1;
	const long csDErrorX = -(1 << 15) + (xrIntFloat_16 >> 1);
	const long csDErrorY = -(1 << 15) + (yrIntFloat_16 >> 1);

	long srcy_16 = csDErrorY;

	for (j = 0; j < nDstHeight; j++)
	{

		long srcx_16 = csDErrorX;
		unsigned long v_8 = (srcy_16 & 0xFFFF) >> 8;
		long kq = srcy_16 >> 16;
		kq = max_int(0, kq);
		kq = min_int(kq, nSrcHeight - 2);

		for (i = 0; i < nDstWidth; i++)
		{

			unsigned long u_8 = (srcx_16 & 0xFFFF) >> 8;
			unsigned long pm3_16 = (u_8*v_8);
			unsigned long pm2_16 = (u_8*(unsigned long)(256 - v_8));
			unsigned long pm1_16 = (v_8*(unsigned long)(256 - u_8));
			unsigned long pm0_16 = ((256 - u_8)*(256 - v_8));

			long kp = srcx_16 >> 16;
			kp = max_int(0, kp);
			kp = min_int(kp, nSrcWidth - 2);

			for (k = 0; k<3; k++)
			{
				data2[j*step2 + i * 3 + k] = (pm0_16*data1[kq*step1 + kp * 3 + k] + pm1_16*data1[(kq + 1)*step1 + kp * 3 + k] +
					pm2_16*data1[kq*step1 + (kp + 1) * 3 + k] + pm3_16*data1[(kq + 1)*step1 + (kp + 1) * 3 + k]) >> 16;

			}
			srcx_16 += xrIntFloat_16;

		}
		srcy_16 += yrIntFloat_16;

	}

}

bool  ResizeImageGray(const unsigned char *src_im, int src_width, int src_height,
	unsigned char* dst_im, int dst_width, int dst_height)
{

	double	lfx_scl, lfy_scl;
	if (src_width == dst_width && src_height == dst_height) {
		memcpy(dst_im, src_im, src_width * src_height * sizeof(unsigned char));
		return true;
	}

	lfx_scl = double(src_width + 0.0) / dst_width;
	lfy_scl = double(src_height + 0.0) / dst_height;

	for (int n_y_d = 0; n_y_d < dst_height; n_y_d++) {
		for (int n_x_d = 0; n_x_d < dst_width; n_x_d++) {
			double lf_x_s = lfx_scl * n_x_d;
			double lf_y_s = lfy_scl * n_y_d;

			int n_x_s = int(lf_x_s);
			n_x_s = (n_x_s <= (src_width - 2) ? n_x_s : (src_width - 2));
			int n_y_s = int(lf_y_s);
			n_y_s = (n_y_s <= (src_height - 2) ? n_y_s : (src_height - 2));

			double lf_weight_x = lf_x_s - n_x_s;
			double lf_weight_y = lf_y_s - n_y_s;

			double lf_new_gray = (1 - lf_weight_y) * ((1 - lf_weight_x) * src_im[n_y_s * src_width + n_x_s] +
				lf_weight_x * src_im[n_y_s * src_width + n_x_s + 1]) +
				lf_weight_y * ((1 - lf_weight_x) * src_im[(n_y_s + 1) * src_width + n_x_s] +
					lf_weight_x * src_im[(n_y_s + 1) * src_width + n_x_s + 1]);

			dst_im[n_y_d * dst_width + n_x_d] = (unsigned char)(lf_new_gray);
		}
	}
	return true;
}
