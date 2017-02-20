

#include <afxwin.h>
#include "localContrastGlobal.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
void localContrastGlobal(Mat dataMat, int maskSize)
{
    int width =dataMat.cols;
    int height=dataMat.rows;
    Size size=dataMat.size();
    Mat pixelData ;
    Mat pixelDataTmp ;
    dataMat.copyTo(pixelData);

	for(int y = maskSize; y < height - maskSize; y++)
    {
        for(int x = maskSize; x < width - maskSize; x++)
        {
            int sum = 0;
            int hist [256]={0};
            int grayTmp;
            double grayNow;


			
            for(int j = y - maskSize; j <= y + maskSize; j++)
            {
                for(int i = x - maskSize; i <= x + maskSize; i++)
                    hist[pixelData.data[j*width+i]]++;
            }

            grayTmp = pixelData.data[y*width+x];

            for(int i = 0; i <= grayTmp; i++)
                sum += hist[i];//cdf
            grayNow = 255 * sum/((2*maskSize+1)*(2*maskSize+1));//255*(總和/mask_size)

            if((int)grayNow > 255)
                grayNow = 255;
            else if((int)grayNow < 0)
                grayNow = 0;
            dataMat.data[y*width+x] = (int)grayNow;
        }
    }



}


int localFangchaContrasMy::getBlockSum(int x, int y, int m, int n) 
{
	int swx = x + n / 2;
	int swy = y + m / 2;
	int nex = x - n / 2 - 1;
	int ney = y - m / 2 - 1;
	int sum1, sum2, sum3, sum4;
	if (swx >= width) {
		swx = width - 1;
	}
	if (swy >= height) {
		swy = height - 1;
	}
	if (nex < 0) {
		nex = 0;
	}
	if (ney < 0) {
		ney = 0;
	}
	sum1 = sum[ney*width + nex];
	sum4 = sum[swy*width + swx];
	sum2 = sum[swy*width + nex];
	sum3 = sum[ney*width + swx];
	return ((sum1 + sum4) - sum2 - sum3);
}

int64 localFangchaContrasMy::getBlockSquareSum(int x, int y, int m, int n) 
{
	int64 swx = x + n / 2;
	int64 swy = y + m / 2;
	int64 nex = x - n / 2 - 1;
	int64 ney = y - m / 2 - 1;
	int64 sum1, sum2, sum3, sum4;
	if (swx >= width) {
		swx = width - 1;
	}
	if (swy >= height) {
		swy = height - 1;
	}
	if (nex < 0) {
		nex = 0;
	}
	if (ney < 0) {
		ney = 0;
	}
	sum1 = squaresum[ney*width + nex];
	sum4 = squaresum[swy*width + swx];
	sum2 = squaresum[swy*width + nex];
	sum3 = squaresum[ney*width + swx];
	return ((sum1 + sum4) - sum2 - sum3);
} 

//http://blog.csdn.net/jia20003/article/details/52744157
//cal 积分图
void localFangchaContrasMy::ProcessJuanji(Mat& dataMat)
{

	int p1 = 0, p2 = 0, p3 = 0, p4;
	int offset = 0, uprow = 0, leftcol = 0;
	int s = 0;
	for (int row = 0; row<height; row++) {
		offset = row*width;
		uprow = row - 1;
		for (int col = 0; col<width; col++)
		{
			leftcol = col - 1;
			p1 = dataMat.data[offset];// p(x, y)  
			p2 = (leftcol<0) ? 0 : sum[offset - 1]; // p(x-1, y)  
			p3 = (uprow<0) ? 0 : sum[offset - width]; // p(x, y-1);  
			p4 = (uprow<0 || leftcol<0) ? 0 : sum[offset - width - 1]; // p(x-1, y-1);  
			s = sum[offset] = p1 + p2 + p3 - p4;

			offset++;
		}
		// System.out.println();  
	}


}
//http://blog.csdn.net/jia20003/article/details/52744157
//cal 积分图
void localFangchaContrasMy::ProcessJuanjiSum(Mat& dataMat)
{
	
	int p1 = 0, p2 = 0, p3 = 0, p4;
	int offset = 0, uprow = 0, leftcol = 0;
	int s = 0;
	for (int row = 0; row<height; row++) {
		offset = row*width;
		uprow = row - 1;
		for (int col = 0; col<width; col++)
		{
			leftcol = col - 1;
			p1 = (dataMat.data[offset]>120)?1:0;// p(x, y)  
			p2 = (leftcol<0) ? 0 : sum[offset - 1]; // p(x-1, y)  
			p3 = (uprow<0) ? 0 : sum[offset - width]; // p(x, y-1);  
			p4 = (uprow<0 || leftcol<0) ? 0 : sum[offset - width - 1]; // p(x-1, y-1);  
			s = sum[offset] = p1 + p2 + p3 - p4;
			offset++;
		}
	}
}

float faiValue(int inValue, int& Avg, int& fangcha)
{

	float fValue = 1.0 / (1 + exp((Avg - inValue)*1.0 / fangcha));
	char szBuf[512];
	sprintf(szBuf, " faiTalbe %.4f %d \n", fValue, inValue);
	return fValue;
}

//判断两个举行区域相互包含
BOOL Content2Rect(cv::Rect& r1, cv::Rect& r2)
{
	int cx1 = r1.x + r1.width / 2;
	int cy1 = r1.y + r1.height / 2;

	int cx2 = r2.x + r2.width / 2;
	int cy2 = r2.y + r2.height / 2;
	int span = 6;

	if (cx1 > (r2.x-span) && cx1 < (r2.x + r2.width) && cy1 > r2.y && cy1 < r2.y + r2.height)
	{
		return TRUE;
		//r2包含r1的中心点
	}
	if (cx2 > (r1.x-span) && cx2 < (r1.x + r1.width) && cy2> r1.y && cy2 < r1.y + r1.height)
	{
		return TRUE;
		//r1包含r2的中心点
	}

	span = r1.height*0.3;
	// 如果两者水平相交
	if (fabs(cy2 -cy1) < 20 &&
		(((r1.x < r2.x) && (r2.x-span < r1.x+r1.width))
			||(r2.x < r1.x && r1.x-span < r2.x+r2.width)))
	{
		return true;
	}

	return FALSE;

}

void CombineRect(std::vector<cv::Rect>& outRect)
{
	//消去重复
	int iCount = outRect.size();
	int index = 0;
	int myk = 0;
	for (int k = 0; k < iCount; ++k)
	{
		if (index == 20)
		{
			myk++;
		}

		if (index < outRect.size())
		{
			for (int j = outRect.size() - 1; j > index; j--)
			{
				char szBuf[256];
				sprintf(szBuf,"%d %d", index,j);
				if (Content2Rect(outRect[index], outRect[j]))
				{

					int minX = (outRect[index].x > outRect[j].x) ? outRect[j].x : outRect[index].x;
					int minY = (outRect[index].y > outRect[j].y) ? outRect[j].y : outRect[index].y;

					int x1 = outRect[index].x + outRect[index].width;
					int x2 = outRect[j].x + outRect[j].width;
					int maxX = (x1 > x2) ? x1 : x2;

					int y1 = outRect[index].y + outRect[index].height;
					int y2 = outRect[j].y + outRect[j].height;
					int maxY = (y1 > y2) ? y1 : y2;
					//
					outRect[index].x = minX; outRect[index].y = minY;
					outRect[index].width = maxX - minX; outRect[index].height = maxY - minY;
					outRect.erase(outRect.begin() + j);
				}
			}
		}

		index++;
	}
}
void localFangchaContrasMy::GetPlateRectsNew(Mat inMat, std::vector<cv::Rect>& outRect)
{
	outRect.clear();

	

	float scales[4] = { 1.0,0.75,0.5,0.25 };
	for (int k = 0; k < 4; ++k)
	{

		int srW = inMat.cols;
		int srH = inMat.rows;

		width = srW*scales[k];
		height = srH*scales[k];

		sum = new int[width*height];
	
		Mat dataMat;
		if (k == 0)
		{
			dataMat = inMat.clone();
		}
		else
		{
			resize(inMat, dataMat, Size(width, height));
		}

		
		

	

		
		ProcessJuanjiSum(dataMat);



		//int xrs[4] = { 22,22,22 };
		//int yrs[4] = { 12,25,44 };
		//int yuzhi1s[4] = { 550,1000,2200 };
		int yr = 22;
		int xr = 22;
		int margin = 3;

		int yuzhi1 = (yr - margin) * 2 * (xr) * 2 * 0.30;
		int syuzhi = 20;// (yr - margin) * 2 * (xr - margin) * 2 * 0.06;

		for (int row = yr; row < height - yr; row += 4)
		{
			for (int col = xr; col < width - xr; col += 4)
			{
				int sr = getBlockSum(col, row, (yr * 2 + 1), (xr * 2 + 1));
				int mr = getBlockSum(col, row, (yr - margin) * 2 + 1, (xr) * 2 + 1);

				if (col == 150 && row == 92)
				{
					int kk = 0;
				}

				if (mr > yuzhi1 && (sr - mr) < syuzhi)
				{
					Rect rect = cv::Rect(col - xr, row - yr + margin, 2 * (xr), 2 * (yr - margin));
					rect.x = rect.x / scales[k];
					rect.y = rect.y / scales[k];
					rect.width = rect.width / scales[k];
					rect.height = rect.height / scales[k];

					outRect.push_back(rect);

				}

			}
		}


		delete[] sum;
	}


	for (int m = 0; m < 4; ++m)
	{
		//合并重复区域
		CombineRect(outRect);
	}







}
void localFangchaContrasMy::GetPlateRects(Mat dataMat, std::vector<cv::Rect>& outRect)
{
	width = dataMat.cols;
	height = dataMat.rows;

	sum = new int[width*height];
	
	ProcessJuanjiSum(dataMat);

	int xrs[4] = { 22,22,22 };
	int yrs[4] = { 12,25,44 };
	int yuzhi1s[4] = { 550,1000,2200 };
	for (int k = 0; k < 3; ++k)
	{
		int yr = yrs[k];
		int xr = xrs[k];

		int margin = 3;

		int yuzhi1 = (yr - margin) * 2 * (xr) * 2 * 0.30;
		int syuzhi = 20;// (yr - margin) * 2 * (xr - margin) * 2 * 0.06;

		for (int row = yr; row < height - yr; row += 4)
		{
			for (int col = xr; col < width - xr; col += 4)
			{
				int sr = getBlockSum(col, row, (yr * 2 + 1), (xr * 2 + 1));
				int mr = getBlockSum(col, row, (yr - margin) * 2 + 1, (xr) * 2 + 1);

				if (col == 150 && row == 92)
				{
					int kk = 0;
				}

				if (mr > yuzhi1 && (sr - mr) < syuzhi)
				{
					outRect.push_back(cv::Rect(col - xr , row - yr + margin, 2 * (xr ), 2 * (yr - margin)));
					
				}

			}
		}

	}
	
	
	for (int m = 0; m < 4; ++m)
	{
		//合并重复区域
		CombineRect(outRect);
	}
		




	delete[] sum;
	
}

void localFangchaContrasMy::FilterSmallThinline(Mat dataMat, Mat& dataOut)
{
	width = dataMat.cols;
	height = dataMat.rows;

	sum = new int[width*height];
	squaresum = new  int64[width*height];
	
	dataOut = dataMat.clone();

	for (int i = 0; i < dataOut.rows; ++i)
	{
		int step = dataOut.step[0] * i;
		for (int j = 0; j < dataOut.cols; ++j)
		{
			dataOut.data[step + j] = 255 - dataOut.data[step + j];
		}
	}

	ProcessJuanji(dataMat);

	std::vector<Point> pts;
	int yr = 9;
	int xr = 9;

	int margin = 2;
	for (int row = yr; row < height - yr; row++)
	{
		for (int col = xr; col < width - xr; col++)
		{
			int sr = getBlockSum(col, row, (yr * 2 + 1), (xr * 2 + 1));
			int mr = getBlockSum(col, row, (yr-margin) * 2 + 1, (xr-margin) * 2 + 1);
	

			if (sr == mr && mr > 0)
			{
				pts.push_back(Point(col,row));
			}
			col += xr;
		}
	}
	//消去重复
	int iCount = pts.size();
	int index = 0;
	for (int k = 0; k < iCount; ++k)
	{
		if (index < pts.size())
		{
			for (int j = pts.size() - 1; j > index; j--)
			{
				int iValue = (pts[j].x - pts[index].x)*(pts[j].x - pts[index].x) +
					(pts[j].y - pts[index].y)*(pts[j].y - pts[index].y);
				if (iValue < yr*yr / 4)
				{
					pts.erase(pts.begin()+j);
				}
			}
		}
		
		index++;
	}


	for (int k = 0; k < pts.size(); ++k)
	{
		for (int i = pts[k].y - yr; i < pts[k].y + yr; ++i)
		{
			int step = dataOut.step[0] * i;
			for (int j = pts[k].x - xr; j < pts[k].x + xr; ++j)
			{
				dataOut.data[step + j] = 255;
			}
		}
	}


	delete[] sum;
	delete[] squaresum;
	
}


void localFangchaContrasMy::BinaryCalContrasMy(Mat dataMat, Mat& dataOut, int maskSize)
{
	int wMax = 0;
	int wMin = 1000;
	for (int i = 0; i < dataMat.rows; ++i)
	{
		int step = i*dataMat.step[0];
		for (int j = 0; j < dataMat.cols; ++j)
		{
			if (dataMat.data[step + j] > wMax)
			{
				wMax = dataMat.data[step + j];
			}
			if (dataMat.data[step + j] < wMin)
			{
				wMin = dataMat.data[step + j];
			}
		}
	}


	width = dataMat.cols;
	height = dataMat.rows;
	sum = new int[width*height];
	squaresum = new  int64[width*height];
	dataOut = dataMat.clone();
	ProcessJuanji(dataMat);
	CString strInfo;
	int index = 0;
	int xr = maskSize; int yr = maskSize;
	int size = (xr * 2 + 1) * (yr * 2 + 1);


	for (int row = 1; row < height - 1; row++)
	{
		if (row < maskSize)
		{
			yr = row;
		}
		else if (row >= height - maskSize)
		{
			yr = height - row;
		}
		else
		{
			yr = maskSize;
		}


		int index = row*width;
		for (int col = 1; col < width - 1; col++)
		{

			if (col < maskSize)
			{
				xr = col;
			}
			else if (col >= width - maskSize)
			{
				xr = width - col;
			}
			else
			{
				xr = maskSize;
			}
			size = (yr * 2 + 1)*(xr * 2 + 1);

			int r = dataMat.data[index];

			int sr = getBlockSum(col, row, (yr * 2 + 1), (xr * 2 + 1));
			int64 vr = getBlockSquareSum(col, row, (yr * 2 + 1), xr * 2 + 1);


			int mr = sr / size;
			int dr = sqrt(vr / size - mr*mr) + 1;
			//int dr1 = sqrt((vr - (sr*sr) / size) / size);
			char szBuf[128];
			sprintf(szBuf, "%d ", dr);
			strInfo += szBuf;

			/*	float fminValue = faiValue(wMin, mr, dr);
			float fmaxValue = faiValue(wMax, mr, dr);
			float fValue = faiValue(r, mr, dr);
			int iValue = 255 * (fValue - fminValue) / (fmaxValue - fminValue);
			*/
			int iValue = mr + 44 * (r - mr) / dr;
			if (r > (mr - 0.1*dr))
			{
				dataOut.data[index] = 255;
			}
			else
			{
				dataOut.data[index] = 0;		
			}

	

			index++;

		}
		//fileW.WriteString(strInfo);
		//fileW.WriteString("\n");

	}
	//TRACE0(strInfo.GetBuffer(0));


	delete[] sum;
	delete[] squaresum;
}


void localFangchaContrasMy::CalContrasMy(Mat dataMat, Mat& dataOut, int maskSize)
{
		int wMax = 0;
		int wMin = 1000;
		for (int i = 0; i < dataMat.rows; ++i)
		{
			int step = i*dataMat.step[0];
			for (int j = 0; j < dataMat.cols; ++j)
			{
				if (dataMat.data[step + j] > wMax)
				{
					wMax = dataMat.data[step + j];
				}
				if (dataMat.data[step + j] < wMin)
				{
					wMin = dataMat.data[step + j];
				}
			}
		}


	width = dataMat.cols;
	height = dataMat.rows;
	sum = new int[width*height];
	squaresum = new  int64[width*height];
	dataOut = dataMat.clone();
	ProcessJuanji(dataMat);
	CString strInfo;
	int index = 0;
	int xr = maskSize; int yr = maskSize;
	int size = (xr * 2 + 1) * (yr * 2 + 1);

	
	for (int row = 1; row < height - 1; row++)
	{
		if (row < maskSize)
		{
			yr = row;
		}
		else if (row >= height - maskSize)
		{
			yr = height - row;
		}
		else
		{
			yr = maskSize;
		}


		int index = row*width;
		for (int col = 1; col < width - 1; col++)
		{
			
			if (col < maskSize)
			{
				xr = col;
			}
			else if (col >= width - maskSize)
			{
				xr = width - col;
			}
			else
			{
				xr = maskSize;
			}
			size = (yr * 2 + 1)*(xr * 2 + 1);

			int r = dataMat.data[index];

			int sr = getBlockSum(col,row,(yr*2+1),(xr*2+1));
			int64 vr = getBlockSquareSum(col,row,(yr*2+1),xr*2+1);

			
			int mr = sr/ size;
			int dr = sqrt(vr/size - mr*mr)+1;
			//int dr1 = sqrt((vr - (sr*sr) / size) / size);
			char szBuf[128];
			sprintf(szBuf,"%d ",dr);
			strInfo += szBuf;

		/*	float fminValue = faiValue(wMin, mr, dr);
			float fmaxValue = faiValue(wMax, mr, dr);
			float fValue = faiValue(r, mr, dr);
			int iValue = 255 * (fValue - fminValue) / (fmaxValue - fminValue);
			*/
			int iValue = mr + 44*(r - mr) / dr;
			if (iValue > 255) iValue = 255;

			if (iValue < 0) iValue = 0;

			dataOut.data[index] = iValue;

			index++;

		}
		//fileW.WriteString(strInfo);
		//fileW.WriteString("\n");
	
	}
	//TRACE0(strInfo.GetBuffer(0));
	

	delete[] sum;
	delete[] squaresum;
}