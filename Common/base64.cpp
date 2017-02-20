#include "base64.h"
#include <iostream>
#include <png/png.h> 

using namespace cv;

bool g_bCodedInit = false;
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}
std::string GetBase64ByfileName(std::string fileName)
{
	std::string base64file;
	//Ч��
	FILE * file = fopen(fileName.c_str(),"rb");
	fseek(file,0L,SEEK_END); 
	int flen = ftell(file);
		fseek(file,0L,SEEK_SET);
	char * szBuf = new char[flen];
	fread(szBuf,1,flen,file);
	base64file = base64_encode((const unsigned char*)szBuf,flen);
	//base64file = base64_encode((const unsigned char*)szBuf,flen);
	fclose(file);
	delete[] szBuf;
	return base64file;
}
std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}


unsigned char szSrcEncode[256] ={ 
0,1,2,5,6,3,4,7,8,10,9,
11,20,13,14,15,16,17,18,19,12,
21,22,30,24,25,26,27,28,29,23,
31,35,33,34,32,36,37,38,39,40,
41,42,43,50,45,46,47,48,49,44,
51,52,53,54,55,56,57,58,59,100,
61,62,63,64,65,75,67,68,69,70,
71,72,73,74,66,76,77,98,79,80,
81,82,83,84,85,88,87,86,89,90,
94,95,93,91,92,96,97,78,99,60,

101,102,103,104,145,106,107,108,109,110,
111,123,193,114,115,118,117,116,119,129,
124,142,112,121,185,126,127,128,120,130,
131,182,192,134,135,136,137,138,139,140,
141,122,184,179,105,146,147,167,200,170,
151,172,153,154,132,156,157,158,159,160,
161,162,163,164,165,166,148,168,169,150,
171,152,173,174,177,176,175,199,144,180,
181,155,183,143,125,186,190,188,189,187,
191,198,113,194,195,196,197,133,178,149,

201,202,203,204,205,227,207,208,209,210,
211,223,213,216,215,214,217,218,238,220,
221,222,212,224,235,226,206,228,229,230,
231,232,244,234,225,236,237,219,239,240,
241,242,243,246,245,233,247,248,249,250,
251,252,253,254,255
};


unsigned  char szDstEncode[256];


void MatImgcodeInit()
{
	g_bCodedInit = true;
	for(int i=0;i < 256;++i)
	{
		szDstEncode[i] == 0;
	}
	for(int i=0;i<256;++i)
	{
		szDstEncode[szSrcEncode[i]] = i; 
	}
	int iCount = 0;
	for(int i=0;i<256;++i)
	{
		if(szDstEncode[i] == 0)
		{
			iCount++;
		}
	}
	int result = iCount;
}
void BufEncode(unsigned char * szBuf,int bufLen)
{
	for(int i = 0;i<bufLen;i+=3)
	{
		szBuf[i] = szSrcEncode[szBuf[i]];
	}
}
void BufDecode(unsigned char * szBuf,int bufLen)
{
	for(int i = 0;i<bufLen;i+=3)
	{
		szBuf[i] =  szDstEncode[szBuf[i]];
	}
}
void MatImgEncode(cv::Mat& mat)
{
	int depth = 3;
	if(mat.type() == CV_8UC4)
	{
		depth = 4;
	}
	for(int i=0;i<mat.rows;i+=3)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				mat.data[step+jump+k] = szSrcEncode[mat.data[step+jump+k]];
			}

			jump+=depth;
		}
	}

	//ÿ��8�Ե�һ��
	//�м�ÿ��5�Ե�һ��
	//���жԵ�
	int allPix = mat.step[0]*mat.rows;
	for(int i=0;i<mat.rows/2-2;i+=8)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		unsigned char tmpData;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				tmpData = mat.data[step+jump+k];
				mat.data[step+jump+k] =mat.data[allPix-step-jump-1+k];
				mat.data[allPix-step-jump-1+k] = tmpData;
			}

			jump+=depth;
		}
	}
	for(int i=0;i<mat.rows/2-2;i+=5)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		unsigned char tmpData;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				tmpData = mat.data[step+jump+k];
				mat.data[step+jump+k] =mat.data[allPix-step-jump-1+k];
				mat.data[allPix-step-jump-1+k] = tmpData;
			}

			jump+=depth;
		}
	}


}

void MatImgDecode(cv::Mat& mat)
{
	int depth = 3;
	if(mat.type() == CV_8UC4)
	{
		depth = 4;
	}
	//ÿ��5�Ե�һ��
	//ÿ��8�Ե�һ��
	int allPix = mat.step[0]*mat.rows;
	for(int i=0;i<mat.rows/2-2;i+=5)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		unsigned char tmpData;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				tmpData = mat.data[step+jump+k];
				mat.data[step+jump+k] =mat.data[allPix-step-jump-1+k];
				mat.data[allPix-step-jump-1+k] = tmpData;
			}

			jump+=depth;
		}
	}


	for(int i=0;i<mat.rows/2-2;i+=8)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		unsigned char tmpData;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				tmpData = mat.data[step+jump+k];
				mat.data[step+jump+k] =mat.data[allPix-step-jump-1+k];
				mat.data[allPix-step-jump-1+k] = tmpData;
			}

			jump+=depth;
		}
	}
	//���н���
	for(int i=0;i<mat.rows;i+=3)
	{
		int step = mat.step[0]*i;
		int jump = 0;
		for(int j=0;j<mat.cols;++j)
		{
			for(int k = 0;k<depth;++k)
			{
				mat.data[step+jump+k] = szDstEncode[mat.data[step+jump+k]];
			}

			jump+=depth;
		}
	}
}

//��Դ�ļ����м��ܺ󣬱��浽Ŀ��·��
void copyfileAndEncode(char * szSrcName,char * dstName)
{
	if(!g_bCodedInit)
	{
		MatImgcodeInit();
	}
	FILE * fSrc,*fDst;
	//
	fSrc = fopen(szSrcName,"rb");
	if(fSrc)
	{
		//��ȡ�ļ���С
		fseek(fSrc,0,SEEK_END);
		int imageLen = ftell(fSrc);

		//��ȡ�ļ�
		fseek(fSrc,0,SEEK_SET);
		char * szBuf = new char[imageLen];
		fread(szBuf,1,imageLen,fSrc);
		if(imageLen > 8192)
		{
			BufEncode((unsigned char*)szBuf,imageLen*0.3);
		}
		//д��
		fDst = fopen(dstName,"wb");
		fwrite(szBuf,1,imageLen,fDst);
		//�ر�
		fclose(fSrc);
		fclose(fDst);
		delete[] szBuf;
		szBuf = NULL;

	}


}
void decodePngfileToMat(char * szSrcName,Mat& outMat)
{
	if(!g_bCodedInit)
	{
		MatImgcodeInit();
	}
	FILE * fSrc,*fDst;
	//
	fSrc = fopen(szSrcName,"rb");
	if(fSrc)
	{
		//��ȡ�ļ���С
		fseek(fSrc,0,SEEK_END);
		int imageLen = ftell(fSrc);

		//��ȡ�ļ�
		fseek(fSrc,0,SEEK_SET);
		char * szBuf = new char[imageLen];
		fread(szBuf,1,imageLen,fSrc);
		if(imageLen > 8192)
		{
			BufDecode((unsigned char*)szBuf,imageLen*0.3);
		}
		
		loadFromStream((unsigned char*)szBuf,imageLen,outMat);

		fclose(fSrc);
		delete[] szBuf;
	}

}

void copyfileAndDecode(char * szSrcName,char * dstName)
{
	if(!g_bCodedInit)
	{
		MatImgcodeInit();
	}
	FILE * fSrc,*fDst;
	//
	fSrc = fopen(szSrcName,"rb");
	if(fSrc)
	{
		//��ȡ�ļ���С
		fseek(fSrc,0,SEEK_END);
		int imageLen = ftell(fSrc);

		//��ȡ�ļ�
		fseek(fSrc,0,SEEK_SET);
		char * szBuf = new char[imageLen];
		fread(szBuf,1,imageLen,fSrc);
		if(imageLen > 8192)
		{
			BufDecode((unsigned char*)szBuf,imageLen*0.3);
		}
		//д��
		fDst = fopen(dstName,"wb");
		fwrite(szBuf,1,imageLen,fDst);
		//�ر�
		fclose(fSrc);
		fclose(fDst);
		delete[] szBuf;
		szBuf = NULL;

	}


}


/******************************ͼƬ����*********************************/
typedef struct _pic_data pic_data;
struct _pic_data
{
	int	width, height;	/* �ߴ� */
	int	bit_depth;		/* λ�� */
	int	flag;			/* һ����־����ʾ�Ƿ���alphaͨ�� */

	unsigned char	**rgba;	/* ͼƬ���� */
};
/**********************************************************************/
#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA	1
#define NO_ALPHA	0


int detect_png(char *filepath, pic_data *out)
/* ���ڽ���pngͼƬ */
{
	FILE *pic_fp;
	pic_fp = fopen(filepath, "rb");
	if(pic_fp == NULL) /* �ļ���ʧ�� */
		return -1;
	
	/* ��ʼ�����ֽṹ */
	png_structp png_ptr;
	png_infop   info_ptr;
	char        buf[PNG_BYTES_TO_CHECK];
	int         temp;
	
	png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	info_ptr = png_create_info_struct(png_ptr);
	
	setjmp(png_jmpbuf(png_ptr)); // ������Ҫ
	
	temp = fread(buf,1,PNG_BYTES_TO_CHECK,pic_fp);
	temp = png_sig_cmp((unsigned char*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
	
	/*����Ƿ�Ϊpng�ļ�*/
	if (temp!=0) return 1;
	
	rewind(pic_fp);
	/*��ʼ���ļ�*/
	png_init_io(png_ptr, pic_fp);
	// ���ļ���
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	int color_type,channels;
	
	/*��ȡ��ȣ��߶ȣ�λ���ɫ����*/
	channels       = png_get_channels(png_ptr, info_ptr);	/*��ȡͨ����*/
	out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);	/* ��ȡλ�� */
	color_type     = png_get_color_type(png_ptr, info_ptr);	/*��ɫ����*/
	
	int i,j;
	int size, pos = 0;
	/* row_pointers��߾���rgba���� */
	png_bytep* row_pointers;
	row_pointers = png_get_rows(png_ptr, info_ptr);
	out->width = png_get_image_width(png_ptr, info_ptr);
	out->height = png_get_image_height(png_ptr, info_ptr);
	
	size = out->width * out->height; /* ����ͼƬ�������ص����� */ 

	if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{/*�����RGB+alphaͨ��������RGB+�����ֽ�*/ 
		size *= (4*sizeof(unsigned char));	/* ÿ�����ص�ռ4���ֽ��ڴ� */
		out->flag = HAVE_ALPHA;				/* ��� */
		out->rgba = (unsigned char**) malloc(size);
		if(out->rgba == NULL)
		{/* ��������ڴ�ʧ�� */
			fclose(pic_fp);
			puts("����(png):�޷������㹻���ڴ湩�洢����!");
			return 1;
		}

		temp = (4 * out->width);/* ÿ����4 * out->width���ֽ� */
		for(i = 0; i < out->height; i++)
		{
			for(j = 0; j < temp; j += 4)
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */
				out->rgba[0][pos] = row_pointers[i][j]; // red
				out->rgba[1][pos] = row_pointers[i][j+1]; // green
				out->rgba[2][pos] = row_pointers[i][j+2];   // blue
				out->rgba[3][pos] = row_pointers[i][j+3]; // alpha
				++pos;
			}
		}
	}
	else if(channels == 3 || color_type == PNG_COLOR_TYPE_RGB)
	{/* �����RGBͨ�� */
		size *= (3*sizeof(unsigned char));	/* ÿ�����ص�ռ3���ֽ��ڴ� */
		out->flag = NO_ALPHA;				/* ��� */
		out->rgba = (unsigned char**) malloc(size);
		if(out->rgba == NULL)
		{/* ��������ڴ�ʧ�� */
			fclose(pic_fp);
			puts("����(png):�޷������㹻���ڴ湩�洢����!");
			return 1;
		}

		temp = (3 * out->width);/* ÿ����3 * out->width���ֽ� */
		for(i = 0; i < out->height; i++)
		{
			for(j = 0; j < temp; j += 3)
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */
				out->rgba[0][pos] = row_pointers[i][j]; // red
				out->rgba[1][pos] = row_pointers[i][j+1]; // green
				out->rgba[2][pos] = row_pointers[i][j+2];   // blue
				++pos;
			}
		}
	}
	else return 1;
	
	/* ��������ռ�õ��ڴ� */
	png_destroy_read_struct(&png_ptr, &info_ptr, 0); 
	return 0;
}
typedef struct  
{
 unsigned char* data;
 int size;
 int offset;
}ImageSource;
//���ڴ��ȡPNGͼƬ�Ļص�����
static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
 ImageSource* isource = (ImageSource*)png_get_io_ptr(png_ptr);
 if(isource->offset + length <= isource->size)
 {
  memcpy(data, isource->data+isource->offset, length);
  isource->offset += length;
 }
 else
  png_error(png_ptr, "pngReaderCallback failed");
}
//���ڴ��ȡ
bool loadFromStream(unsigned char* data, const unsigned int dataSize,Mat& outMat)
{
 
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if(png_ptr == 0)
   return false;
  
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if(info_ptr == 0)
  {
   png_destroy_read_struct(&png_ptr, 0, 0);
   return false;
  }
  if(setjmp(png_jmpbuf(png_ptr)))
  {
   png_destroy_read_struct(&png_ptr, &info_ptr,0);
  }
  ImageSource imgsource;
  imgsource.data = data;
  imgsource.size = dataSize;
  imgsource.offset = 0;
  png_set_read_fn(png_ptr, &imgsource,pngReadCallback);
 // png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);
  // ���ļ���
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

  	 int width = png_get_image_width(png_ptr, info_ptr);
	int height = png_get_image_height(png_ptr, info_ptr);

  int color_type = png_get_color_type(png_ptr, info_ptr);	/*��ɫ����*/
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);	/* ��ȡλ�� */
  int channels       = png_get_channels(png_ptr, info_ptr);	/*��ȡͨ����*/

  png_bytep* row_pointers = png_get_rows(png_ptr,info_ptr);
  if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
	   int pos=0;
  
	   unsigned char * m_bgra = new unsigned char[width*height*4];
	   memset(m_bgra,0,width*height*4);
	   for(int i=0;i<height;i++)
	   {
			for(int j=0;j<4*width;j+=4)
			{
				 m_bgra[pos++] = row_pointers[i][j+2];//BLUE
				 m_bgra[pos++] = row_pointers[i][j+1];//GREEN
				 m_bgra[pos++] = row_pointers[i][j];//RED

				 m_bgra[pos++] = row_pointers[i][j+3];//RED
			}
	   }

	   outMat = Mat(height,width,CV_8UC4);
	   memcpy(outMat.data,m_bgra,width*height*4);

	 // free memory
	 png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	 delete[] m_bgra;
  }
  else  if(channels == 3 || color_type == PNG_COLOR_TYPE_RGB)
  {
	   int pos=0;
  
	   unsigned char * m_bgra = new unsigned char[width*height*3];
	   memset(m_bgra,0,width*height*3);
	   for(int i=0;i<height;i++)
	   {
			for(int j=0;j<3*width;j+=3)
			{
				 m_bgra[pos++] = row_pointers[i][j+2];//BLUE
				 m_bgra[pos++] = row_pointers[i][j+1];//GREEN
				 m_bgra[pos++] = row_pointers[i][j];//RED

				
			}
	   }

	   outMat = Mat(height,width,CV_8UC3);
	   memcpy(outMat.data,m_bgra,width*height*3);

	 // free memory
	 png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	 delete[] m_bgra;
  }
 
 
 return /*(m_good = true)*/true;
}