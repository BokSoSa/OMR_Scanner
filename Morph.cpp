// Morph.cpp: implementation of the CMorphology class.
//
//////////////////////////////////////////////////////////////////////


#include "Morph.h"
#include "Pixel.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMorph::CMorph()
{

}

CMorph::~CMorph()
{

}

// 모폴로지 스무딩 연산
cv::Mat CMorph::GS_smoothing(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;

	if (src_image.channels() != 1) return cv::Mat();
	
	// 열림 연산 수행
	cv::Mat opening_image = GS_opening(src_image, type, method);
	
	// 닫힘 연산 수행
	cv::Mat dst_image = GS_closing(opening_image, type, method);

	// 할당한 메모리 해제
	opening_image.release();

	return dst_image;
}

// 모폴로지 그래디언트 연산
cv::Mat CMorph::GS_gradient(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;
	CUtil cUtil;
	cv::Mat tmp_image;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;
	cv::Mat dilate_image = cUtil.GS_createImage(src_image.size(), src_image.channels());

	// 이진 영상으로 변환
	if (method == 0)
	{
		double threshold = 255.0;
		double max_value = 255.0;
		int threshold_type = cv::THRESH_OTSU;

		// otsu 알고리즘을 이용한 이진 영상 변환
		cv::Mat binary_image = cPixel.GS_threshold(src_image, threshold,
			max_value, threshold_type);
		tmp_image = binary_image.clone();

		binary_image.release();
	}
	// 명암도 영상이면 복사
	else if (method == 1)
	{
		tmp_image = src_image.clone();
	}

	// 구조화 요소 생성
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));
	// 모폴로지 그래디언트 연산
	cv::morphologyEx(tmp_image, dst_image, cv::MORPH_GRADIENT, kernel);

	/*
	// cvMorphologyEx() 함수를 사용하지 않고 직접 구현.

	// 팽창 연산 수행
	IplImage *dilate_image = GS_dilate( src_image, type, method );

	// 침식 연산 수행
	IplImage *erode_image = GS_erode( src_image, type, method );

	// 팽창 연산과 침식 연산 수행 결과를 차 연산
	IplImage *dst_image = cPixel.GS_subtract_image( dilate_image, erode_image );

	// 할당한 메모리 해제
	cvReleaseImage( &erode_image );
	*/

	// 할당한 메모리 해제
	dilate_image.release();
	tmp_image.release();
	kernel.release();

	return dst_image;
}

// 웰 연산 후 명암대비 증가
cv::Mat CMorph::GS_well_contrast(cv::Mat src_image, int type)
{
	CPixel cPixel;
	CUtil cUtil;
	int i, j, ret_var;
	double var, tophat_var, well_var;

	// 초기화 
	int height = src_image.rows;
	int width = src_image.cols;
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	// 탑-햇 연산
	cv::Mat tophat_image = GS_topHat(src_image, type);

	// 웰 연산 
	cv::Mat well_image = GS_well(src_image, type);

	for (i = 0; i<height; i++)
	{
		for (j = 0; j<width; j++)
		{
			
			var = src_image.at<float>(i, j); // float? uchar? lets figure it out.
			tophat_var = tophat_image.at<float>(i, j);// float? uchar? lets figure it out.
			well_var = well_image.at<float>(i, j);// float? uchar? lets figure it out.

			ret_var = cUtil.GS_clamping((var + tophat_var) - well_var);

			//cvSetReal2D(dst_image, i, j, ret_var);
			dst_image.at<float>(i, j) = ret_var;
		}
	}

	// 할당한 메모리 해제
	tophat_image.release();

	return dst_image;
}

// 웰 연산
cv::Mat CMorph::GS_well(cv::Mat src_image, int type)
{
	CPixel cPixel;
	CUtil cUtil;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;
	cv::Mat closing_image = cUtil.GS_createImage(src_image.size(), src_image.channels());

	// 구조화 요소 생성
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));

	// 웰 연산
	cv::morphologyEx(src_image, dst_image, cv::MORPH_BLACKHAT, kernel);

	/*
	// cvMorphologyEx() 함수를 사용하지 않고 직접 구현.

	// 원 명암도 영상에 대한 닫힘 연산 수행
	IplImage *closing_image = GS_closing( src_image, type, 1 );

	// 초기화
	IplImage *dst_image = cUtil.GS_createImage( cvGetSize(src_image), src_image->nChannels );

	// 원 명암도 영상과 닫힘 연산 수행 영상을 차 연산
	double ret_var;
	for(int i=0; i<src_image->height; i++)
	{
	for(int j=0; j<src_image->width; j++)
	{
	// 차 연산
	ret_var = cvGetReal2D( src_image, i, j ) - cvGetReal2D( closing_image, i, j );

	// 절대값 구한 후 클램핑
	ret_var = cUtil.GS_clamping( fabs(ret_var) );

	cvSetReal2D( dst_image, i, j, ret_var );
	}
	}
	*/
	// 할당한 메모리 해제
	closing_image.release();
	kernel.release();

	return dst_image;
}

// 탑-햇 연산 후 명암대비 증가
cv::Mat CMorph::GS_topHat_contrast(cv::Mat src_image, int type, double contrast, int brightness)
{
	CPixel cPixel;

	if (src_image.channels() != 1) return cv::Mat(); // NULL을 어떻게 return 할것인가

	// 탑-햇 연산
	cv::Mat tophat_image = GS_topHat(src_image, type);

	// 밝기 조절 + 명암대비 조절
	cv::Mat dst_image
		= cPixel.GS_LUT_basic_contrast_brightness(tophat_image, contrast, brightness);

	// 할당한 메모리 해제
	tophat_image.release();

	return dst_image;
}

// 탑-햇 연산
cv::Mat CMorph::GS_topHat(cv::Mat src_image, int type)
{
	CUtil cUtil;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels()!= 1) return dst_image;
	cv::Mat opening_image = cUtil.GS_createImage(src_image.size(), src_image.channels());

	// 구조화 요소 생성
//    cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));
    cv::Mat kernel = cv::getStructuringElement(type, cv::Size(11, 11), cv::Point());

	// 탑-햇 연산
	cv::morphologyEx(src_image, dst_image, cv::MORPH_TOPHAT, kernel);

	/*
	// cvMorphologyEx() 함수를 사용하지 않고 직접 구현.

	CPixel cPixel;

	// 원 명암도 영상에 대한 열림 연산 수행
	IplImage *opening_image = GS_opening( src_image, type, 1 );

	// 원 명암도 영상과 열림 연산 수행 영상을 차 연산
	IplImage *dst_image = cPixel.GS_subtract_image( src_image, opening_image );

	*/

	// 할당한 메모리 해제
	opening_image.release();
	kernel.release();

	return dst_image;
}

// 닫힘 연산
cv::Mat CMorph::GS_closing(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;
	CUtil cUtil;
	cv::Mat tmp_image;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;
	cv::Mat dilate_image = cUtil.GS_createImage(src_image.size(), src_image.channels());

	// 이진 영상으로 변환
	if (method == 0)
	{
		double threshold = 255.0;
		double max_value = 255.0;
		int threshold_type = cv::THRESH_OTSU;

		// otsu 알고리즘을 이용한 이진 영상 변환
		cv::Mat binary_image = cPixel.GS_threshold(src_image, threshold,
			max_value, threshold_type);
		tmp_image = binary_image.clone();

		binary_image.release();
	}
	// 명암도 영상이면 복사
	else if (method == 1)
	{
		tmp_image = src_image.clone();
	}

	// 구조화 요소 생성
	/* Type  = cv::Morphshape
	   MORPH_RECT / MORPH_ELLIPSE / MORPH_CROSS */
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));
	
	// 닫힘 연산 src dst temp element operation
    cv::morphologyEx(tmp_image, dst_image, cv::MORPH_CLOSE, kernel);
	/*
	// cvMorphologyEx() 함수를 사용하지 않고 직접 구현.
	// 팽창 연산
	cvDilate( tmp_image, dilate_image, element, 1 );

	// 침식 연산
	cvErode( dilate_image, dst_image, element, 1 );
	*/

	// 할당한 메모리 해제
	dilate_image.release();
	tmp_image.release();
	kernel.release();

	return dst_image;
}

// 열림 연산
cv::Mat CMorph::GS_opening(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;
	CUtil cUtil;
	cv::Mat tmp_image;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;
	cv::Mat erode_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	
	// 이진 영상으로 변환
	if (method == 0)
	{
		double threshold = 255.0;
		double max_value = 255.0;
		int threshold_type = cv::THRESH_OTSU;

		// otsu 알고리즘을 이용한 이진 영상 변환
		cv::Mat binary_image = cPixel.GS_threshold(src_image, threshold,
			max_value, threshold_type);
		tmp_image = binary_image.clone();

		binary_image.release();
	}
	// 명암도 영상이면 복사
	else if (method == 1)
	{
		tmp_image = src_image.clone();
	}

	// 구조화 요소 생성
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));

	// 열림 연산 morphologyEX(source, destination, operation, kernel)
    cv::morphologyEx(tmp_image, dst_image, cv::MORPH_OPEN, kernel);

	/*
	// cvMorphologyEx() 함수를 사용하지 않고 직접 구현.
	// 침식 연산
	cvErode( tmp_image, erode_image, element );

	// 팽창 연산
	cvDilate( erode_image, dst_image, element );
	*/

	// 할당한 메모리 해제
	erode_image.release();
	tmp_image.release();
	kernel.release();

	return dst_image;
}

// 팽창 연산

cv::Mat CMorph::GS_dilate(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;
	CUtil cUtil;
	cv::Mat tmp_image;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	// 이진 영상으로 변환
	if (method == 0)
	{
		double threshold = 255.0;
		double max_value = 255.0;
		int threshold_type = cv::THRESH_OTSU;

		// otsu 알고리즘을 이용한 이진 영상 변환
		cv::Mat binary_image = cPixel.GS_threshold(src_image, threshold,
			max_value, threshold_type);
		// 이진 영상 변환 결과 복사 
		tmp_image = binary_image.clone();

		binary_image.release();
	}
	// 명암도 영상이면 복사
	else if (method == 1)
	{
		tmp_image = src_image.clone();
	}

	// 구조화 요소 생성
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));

	// 팽창 연산
	cv::dilate(tmp_image, dst_image, kernel);

	// 할당한 메모리 해제
	tmp_image.release();
	kernel.release();

	return dst_image;
}

// 침식 연산
cv::Mat CMorph::GS_erode(cv::Mat src_image, int type, int method)
{
	CPixel cPixel;
	CUtil cUtil;
	cv::Mat tmp_image;

	// 초기화 
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	// 이진 영상으로 변환
	if (method == 0)
	{
		double threshold = 255.0;
		double max_value = 255.0;
		int threshold_type = cv::THRESH_OTSU;

		// otsu 알고리즘을 이용한 이진 영상 변환
		cv::Mat binary_image = cPixel.GS_threshold(src_image, threshold,
			max_value, threshold_type);

		// 이진 영상 변환 결과 복사 
		tmp_image = binary_image.clone();

		binary_image.release();
	}
	// 명암도 영상이면 복사
	else if (method == 1)
	{
		tmp_image = src_image.clone();
	}

	// 구조화 요소 생성
	cv::Mat kernel = cv::getStructuringElement(type, cv::Size(3, 3), cv::Point(1, 1));

	// 침식 연산
	cv::erode(tmp_image, dst_image, kernel);

	// 할당한 메모리 해제
	tmp_image.release();
	kernel.release();

	return dst_image;
}
