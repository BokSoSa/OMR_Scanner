// Geometry.h: interface for the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

//#include<opencv\cv.h>
//#include<opencv\cxcore.h>
//#include<opencv\cvaux.h>
//#include<opencv\highgui.h>
#include <opencv2/opencv.hpp>


#if !defined(AFX_GEOMETRY_H__1E8A5F13_A043_489A_A703_835E78F64E03__INCLUDED_)
#define AFX_GEOMETRY_H__1E8A5F13_A043_489A_A703_835E78F64E03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGeometry
{
public:
	CGeometry();
	virtual ~CGeometry();

/*
	// 단순 확대/축소
	IplImage *GS_enlarge(IplImage *src_image, int resize_height, int resize_width);

	// 최근접 이웃 보간법
	IplImage *GS_neighbor_interp(IplImage *src_image, int resize_height, int reisize_width);

	// 양선형 보간법
	IplImage *GS_bilinear_interp(IplImage *src_image, int resize_height, int reisize_width);

	// 감축 : 중간값/평균값
	IplImage *GS_reduction(IplImage *src_image, double h_scale, double w_scale, int method);

	// 회전
	IplImage *GS_rotate(IplImage *src_image, int angle);
*/

	// 어파인 변환
	cv::Mat GS_affine(cv::Mat src_image, cv::Mat map_matrix);

	// 어파인 변환 - 회전
	cv::Mat GS_affineRotate(cv::Mat src_image, double angle, double scale);

	// 원근 변환
	cv::Mat GS_perspective(cv::Mat src_image, cv::Point2f src_point, cv::Point2f dst_point);

	// 반사 또는 대칭
	cv::Mat GS_reflect(cv::Mat src_image, int flip_mode);

	// 관심 영역 추출
	cv::Mat GS_getROI(cv::Mat src_image, int startH, int startW, int height, int width);

	// 관심 영역 추출된 것을 원래대로 복원 
	// cv::Mat 자료형은 해당 기능 없음.
//    void GS_resetROI(IplImage *roi_image);
};

#endif // !defined(AFX_GEOMETRY_H__1E8A5F13_A043_489A_A703_835E78F64E03__INCLUDED_)
