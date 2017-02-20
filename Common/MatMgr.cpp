
#include "MatMgr.h"
#include <opencv2\imgproc.hpp>	

void MatMgr::SetMat(cv::Mat& mat)
{
		m_lock.lock();
		if(mat.type() == CV_8UC4)
		{
			cv::cvtColor(mat,m_matSrc,CV_BGRA2BGR);
		}
		else m_matSrc = mat;
		m_lock.unlock();
}