// Color.h: interface for the CColor class.
//
//////////////////////////////////////////////////////////////////////


#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColor::CColor()
{

}

CColor::~CColor()
{

}

// 컬러 공간을 명암도 영상으로 변환  (conversion color space to gray scale image)
cv::Mat CColor::GS_rgb2gray(cv::Mat rgb_image)
{
	cv::Mat gray_image = cv::Mat(rgb_image.size(), CV_8UC1);

	cv::cvtColor(rgb_image, gray_image, cv::COLOR_BGR2GRAY);

	return gray_image;
}



// RGB 컬러 공간을 HSV 컬러 공간으로 변환한다. (convert RGB color space to HSV color space)
void CColor::GS_rgb2hsv(cv::Mat *rgb, cv::Mat *hsv)
{
	GS_cvtColor(rgb, hsv, cv::COLOR_BGR2HSV);

	return;
}

// 주어진 code에 따라 컬러 공간 변환한 후, 각 채널을 분리하여 반환한다. 
//(split each channel and return after converting color space according to the given code) 
void CColor::GS_cvtColor(cv::Mat * src, cv::Mat * dst, int code)
{
	cv::Mat src_image = cv::Mat(src[0].size(), src[0].type());
	cv::Mat dst_image = cv::Mat(src[0].size(), src[0].type());

	for (int i = 0; i < 3; i++)
	{
		dst[i] = cv::Mat(src[0].size(), src[0].type());
	}
	
	// STEP 2 : 각 채널을 src_image로 조합한다.  (composite each channel to src_image)
	cv::merge(src, 3, src_image);

	// STEP 3 : 해당 컬러 공간을 주어진 인자에 맞는 컬러 공간으로 변환. (convert color space to the new one which meets given argument)
	cv::cvtColor(src_image, dst_image, code);

	// STEP 4 : 변환된 컬러 공간을 분리(split the converted color space)
	cv::split(dst_image, dst);

	// STEP 5 : 메모리 해제 (memory deallocation)
	src_image.release();
	dst_image.release();

	return;

}

// HSV 컬러 공간을 RGB 컬러 공간으로 변환한다.  (convert RGB color space to HSV color space)
void CColor::GS_hsv2rgb(cv::Mat * hsv, cv::Mat * rgb)
{
	GS_cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
}

// RGB 컬러 공간을 YCbCr 컬러 공간으로 변환한다.  (convert RGB color space to YCbCr color space)
void CColor::GS_rgb2ycbcr(cv::Mat *rgb, cv::Mat *yCbCr)
{
	GS_cvtColor(rgb, yCbCr, cv::COLOR_BGR2YCrCb);
}

// YCbCr 컬러 공간을 RGB 컬러 공간으로 변환한다.  (convert RGB color space to YCbCr color space)
void CColor::GS_ycbcr2rgb(cv::Mat * YCbCr, cv::Mat * rgb)
{
	GS_cvtColor(YCbCr, rgb, cv::COLOR_YCrCb2BGR);
}

// RGB 컬러 공간 분리, 만약에 명암도 영상이면 각 채널에 할당한다. (split RGB color space/ if the image is gray scale, assign it to each channel) 
void CColor::GS_splitRGB(cv::Mat rgb_image, cv::Mat * rgb)
{
	// STEP 1 : 메모리 할당 (memory allocation)
	for (int i = 0; i < 3; i++) {
		rgb[i] = cv::Mat(rgb_image.size(), rgb_image.type());
	}

	// STEP 2 : cvCvtPixToPlane() 함수 호출 (call cvCvtPixToPlne() mothod)
	// 컬러 영상 (color image)
	if (rgb_image.channels() == 3)
	{
		// 주의 : BGR 채널 순서로 가져옴. (caution: B->G->R channel order)
		cv::split(rgb_image, rgb);
	}
	// 명암도 영상 (gray scale image)
	else if (rgb_image.channels() == 1)
	{
		cv::Mat tmp_image = cv::Mat(rgb_image.size(), CV_8UC3);

		cv::cvtColor(rgb_image, tmp_image, cv::COLOR_GRAY2BGR);

		cv::split(tmp_image, rgb);

		tmp_image.release();
	}

	return;
}


// RGB 채널을 조합하여 RGB 컬러 공간으로 반환한다. (composite RGB channels and convert RGB color space)
cv::Mat CColor::GS_compositeRGB(cv::Mat *rgb)
{
	cv::Mat rgb_image = cv::Mat(rgb[0].size(), CV_8UC3);

	cv::merge(rgb, 3, rgb_image);

	return rgb_image;
}

// RGB 컬러 공간 분리 - 5장 예제 (Split RGB color space)
void CColor::SplitRGB(cv::Mat color_image, cv::Mat *rgb)
{
	rgb[0] = cv::Mat(color_image.size(), CV_8UC1);
	rgb[1] = cv::Mat(color_image.size(), CV_8UC1);
	rgb[2] = cv::Mat(color_image.size(), CV_8UC1);

	cv::split(color_image, rgb);
}

