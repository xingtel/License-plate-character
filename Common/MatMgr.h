/********************************************
	mat ����,ȡֵ�ʹ�ֵ�����������֤���ݵĽ�����

	
************************************************************/
#pragma once
#include <afxwin.h>
#include <vxHelp/vxComm/xthreadpool.h>
#include <opencv2\core.hpp>



class MatMgr
{
private:
	cv::Mat m_matSrc;
	xsynchron m_lock;
	xsynchron m_lockFilter;
	cv::Mat m_matFilter;//�˲��󱣴�Ķ���
public:
	void SetMat(cv::Mat& mat);
	
	void SetFilterMat(cv::Mat& mat)
	{
		m_lockFilter.lock();
		m_matFilter = mat;
		m_lockFilter.unlock();
	}
	cv::Mat GetFilterMat()
	{
		cv::Mat tmpMat;

		m_lockFilter.lock();
		tmpMat = m_matFilter.clone();
		m_lockFilter.unlock();

		return tmpMat;
	}

	cv::Mat GetMat()
	{
		cv::Mat tmpMat;

		m_lock.lock();
		tmpMat = m_matSrc.clone();
		m_lock.unlock();

		return tmpMat;
	}
};
