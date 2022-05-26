// Color.h: interface for the CColor class.
//
//////////////////////////////////////////////////////////////////////

#include<opencv2/opencv.hpp>

#if !defined(AFX_COLOR_H__51E6F4C5_45B6_440E_8240_FCFF2CFD7CAC__INCLUDED_)
#define AFX_COLOR_H__51E6F4C5_45B6_440E_8240_FCFF2CFD7CAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColor  
{
public:
	CColor();
	virtual ~CColor();

	// RGB 컬러 공간 분리, 만약에 명암도 영상이면 각 채널에 할당함.
	void GS_splitRGB(cv::Mat rgb_image, cv::Mat * rgb);

	// RGB 채널을 조합하여 하나의 RGB 컬러 공간으로 반환한다.
	cv::Mat GS_compositeRGB(cv::Mat *rgb);

	// RGB 컬러 공간을 HSV 컬러 공간으로 변환한다.
	void GS_rgb2hsv(cv::Mat *rgb, cv::Mat *hsv);

	// HSV 컬러 공간을 RGB 컬러 공간으로 변환한다.
	void GS_hsv2rgb(cv::Mat *hsv, cv::Mat *rgb);

	// RGB 컬러 공간을 YCbCr 컬러 공간으로 변환한다.
	void GS_rgb2ycbcr(cv::Mat *rgb, cv::Mat *yCbCr);

	// YCbCr 컬러 공간을 RGB 컬러 공간으로 변환한다.
	void GS_ycbcr2rgb(cv::Mat *YCbCr, cv::Mat *rgb);

	// 주어진 code에 따라 컬러 공간 변환한 후, 각 채널을 분리하여 반환한다.
	void GS_cvtColor(cv::Mat * src, cv::Mat * dst, int codes);
	
	// 컬러 공간을 명암도 영상으로 변환 
	cv::Mat GS_rgb2gray(cv::Mat rgb_image);

	// 컬러 영상 -> 채널 분리
	void SplitRGB(cv::Mat color_image,	/* 원본 RGB 영상 */
				  cv::Mat *rgb);		/* bgr 채널 배열 */
};

#endif // !defined(AFX_COLOR_H__51E6F4C5_45B6_440E_8240_FCFF2CFD7CAC__INCLUDED_)
