
#include "vcshow.h"
#include <afxwin.h>
void *  gShowWnd = NULL;
#include <atlimage.h>
#include <opencv2\imgproc.hpp>

void DrawThePicPoint(void  * pWnd,cv::Mat clmResult, int posX, int posY, int dstW)
{
	if (pWnd == NULL) return;
	//若使用前不想把原来绘制的图片去掉，可以删去上面那三段
	CDC *pDC =((CWnd*) pWnd)->GetDC();
	if (pDC)
	{
		int biaoWidht = dstW;
		cv::Mat dst;
		if (clmResult.cols > dstW)
		{
			float ratio = clmResult.cols*1.0 / biaoWidht;
			int height = clmResult.rows / ratio;
			cv::resize(clmResult, dst, cv::Size(biaoWidht, height));
		}
		else
		{
			dst = clmResult;
		}


		CImage Image;
		Image.Create(dst.cols, dst.rows, 24);
		int i;
		int j;
		for (i = 0; i < Image.GetHeight(); i++)
		{
			int step = dst.step[0] * i;
			int jump = 0;
			for (j = 0; j < Image.GetWidth(); j++)
			{
				byte *pByte = (byte *)Image.GetPixelAddress(j, i);

				if (dst.channels() != 1)
				{
					pByte[0] = (unsigned char)dst.data[step + jump + 0];
					pByte[1] = (unsigned char)dst.data[step + jump + 1];
					pByte[2] = (unsigned char)dst.data[step + jump + 2];

					jump += 3;
				}
				else
				{
					pByte[0] = (unsigned char)dst.data[step + jump + 0];
					pByte[1] = (unsigned char)dst.data[step + jump + 0];
					pByte[2] = (unsigned char)dst.data[step + jump + 0];

					jump += 1;
				}

			}
		}

		Image.Draw(pDC->m_hDC, posX, posY);
		Image.Destroy();

		((CWnd*)pWnd)->ReleaseDC(pDC);
	}

	
}
