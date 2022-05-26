// Morph.h: interface for the CMorphology class.
//
//////////////////////////////////////////////////////////////////////
//#include<opencv\cv.h>
//#include<opencv\cxcore.h>
//#include<opencv\cvaux.h>
//#include<opencv\highgui.h>
#include <opencv2/opencv.hpp>

#if !defined(AFX_MORPHOLOGY_H__31E0FBD3_42F5_40A0_A23F_FCB82C21E80E__INCLUDED_)
#define AFX_MORPHOLOGY_H__31E0FBD3_42F5_40A0_A23F_FCB82C21E80E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMorph
{
public:
	CMorph();
	virtual ~CMorph();

	// 침식 연산
	cv::Mat GS_erode(cv::Mat src_image, int type, int method);

	// 팽창 연산
	cv::Mat GS_dilate(cv::Mat src_image, int type, int method);

	// 열림 연산
	cv::Mat GS_opening(cv::Mat src_image, int type, int method);

	// 닫힘 연산
	cv::Mat GS_closing(cv::Mat src_image, int type, int method);

	// 탑-햇 연산
	cv::Mat GS_topHat(cv::Mat src_image, int type);

	// 탑-햇 연산 수행 결과 명암대비 증가 
	cv::Mat GS_topHat_contrast(cv::Mat src_image, int type, double contrast, int brightness);

	// 웰 연산
	cv::Mat GS_well(cv::Mat src_image, int type);

	// 웰 연산 수행 결과 명암대비 증가 
	cv::Mat GS_well_contrast(cv::Mat src_image, int type);

	// 모폴로지 그래디언트 연산
	cv::Mat GS_gradient(cv::Mat src_image, int type, int method);

	// 모폴로지 스무딩 연산
	cv::Mat GS_smoothing(cv::Mat src_image, int type, int method);

};

#endif // !defined(AFX_MORPHOLOGY_H__31E0FBD3_42F5_40A0_A23F_FCB82C21E80E__INCLUDED_)
