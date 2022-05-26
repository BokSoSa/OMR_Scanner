/*
* Filter.cpp : 공간 영역 필터링 관련 클래스
* Modified by Lee, Moon-Ho (conv2@korea.com)
* Last Modified : 2006/11/15
*/

// Filter.cpp: implementation of the CFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "Filter.h"
#include "Util.h"
#include "Color.h"
#include "Pixel.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilter::CFilter()
{

}

CFilter::~CFilter()
{

}

// 가우시안 스무딩 필터링
cv::Mat CFilter::GS_gaussian_smoothing_filtering(cv::Mat src_image, double sigma)
{
	int i, j, mask_height, mask_width;
	double var;
	CUtil cUtil;

	// 초기화 
	int height = src_image.rows;
	int width = src_image.cols;
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (src_image.channels() != 1) return dst_image;

	// sigma 범위를 충족하지 않을 때
	if (sigma < 0.0)
	{
		//cUtil.GS_errMsg("sigma 값은 0.0 이상이어야 합니다.");
		return dst_image;
	}

	// 마스크를 구한다.
	// Gaussian 마스크의 크기를 결정한다.
	// length = 5 이면 2D의 경우 5x5 마스크이다.
	int length = (int)(6 * sigma) + 1;
	int center = length / 2;

	mask_height = mask_width = length;
	float **mask = cUtil.GS_floatAlloc2D(mask_height, mask_width);
	
	for (i = 0; i<mask_height; i++)
	{
		for (j = 0; j<mask_width; j++)
		{
			var = (1.0 / (2.0*M_PI*sigma*sigma))
				* exp(-((i - center)*(i - center) + (j - center)*(j - center))
					*(1.0 / (2.0*sigma*sigma)));

			mask[i][j] = (float)var;
		}
	}
	
	// 2차원 마스크를 1차원으로 변환
	float *single_mask = cUtil.GS_toSingleArray(mask, mask_height, mask_width);

	// CvMat으로 변환
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, single_mask);

	// 회선 후 dst_image에 복사
	cv::Mat tmp_image = GS_conv2(src_image, mat_kernel);
	tmp_image.copyTo(dst_image);

	free(single_mask);
	tmp_image.release();
	cUtil.GS_free2D(mask, mask_height);

	return dst_image;
}

// 최대/최소값 필터링

// α-trimmed 평균값 필터링
cv::Mat CFilter::GS_mean_filtering(cv::Mat src_image, int mask_height, int mask_width)
{
	int i, j, m, n;
	CUtil cUtil;
	double sum, var;

	// 초기화 
	int height = src_image.rows;
	int width = src_image.cols;
	cv::Mat dst_image = cUtil.GS_createImage(src_image.size(), src_image.channels());
	if (dst_image.channels() != 1) return dst_image;

	for (i = 0; i<height - mask_height + ((mask_height - 1) / 2); i++)
	{
		for (j = 0; j<width - mask_width + ((mask_width - 1) / 2); j++)
		{
			sum = 0.0;

			if ((i + mask_height > height) || (j + mask_width > width))
				continue;

			// mask_height x mask_width의 블록을 잡아 mask에 할당.
			for (m = 0; m<mask_height; m++)
			{
				for (n = 0; n<mask_width; n++)
				{
					sum += src_image.at<uchar>(m + i, n + j);
				}
			}

			// 평균값을 구한다.
			var = sum / (mask_height*mask_width);
			dst_image.at<uchar>(i + (mask_height - 1) / 2, j + (mask_width - 1)) = var;
			
		}
	}

	return dst_image;
}

// 미디언 필터링



// 블러 라이트/블랜딩 필터 
cv::Mat CFilter::GS_blue_lb_filter(cv::Mat src_image, int method)
{
	CUtil cUtil;

	int mask_height = 3;
	int mask_width = 3;

	float **mask = cUtil.GS_floatAlloc2D(mask_height, mask_width);

	// 블러 라이트 필터
	if (method == 0)
	{
		mask[0][0] = 1.0f; mask[0][1] = 2.0f; mask[0][2] = 1.0f;
		mask[1][0] = 2.0f; mask[1][1] = 2.0f; mask[1][2] = 2.0f;
		mask[2][0] = 1.0f; mask[2][1] = 2.0f; mask[2][2] = 1.0f;
	}
	// 블러 블랜딩 필터 
	else if (method == 1)
	{
		mask[0][0] = 1.0f; mask[0][1] = 2.0f; mask[0][2] = 1.0f;
		mask[1][0] = 2.0f; mask[1][1] = 4.0f; mask[1][2] = 2.0f;
		mask[2][0] = 1.0f; mask[2][1] = 2.0f; mask[2][2] = 1.0f;
	}

	// 2차원 마스크를 1차원으로 변환
	float *single_mask = cUtil.GS_toSingleArray(mask, mask_height, mask_width);

	// cv::Mat으로 변환
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, single_mask);

	float tmp;
	for (int i = 0; i<mask_height; i++)
	{
		for (int j = 0; j<mask_width; j++)
		{
			tmp = (float)mat_kernel.at<float>(i, j);

			if (method == 0) //cvmSet(&mat_kernel, i, j, tmp / 14.0f);
				mat_kernel.at<float>(i, j) = tmp / 14.0f;
			else if (method == 1) //cvmSet(&mat_kernel, i, j, tmp / 16.0f);
				mat_kernel.at<float>(i, j) = tmp / 16.0f;
		}
	}

	// 회선 
	cv::Mat dst_image = GS_conv2(src_image, mat_kernel);

	// 할당한 메모리 해제
	cUtil.GS_free1D(single_mask);
	cUtil.GS_free2D(mask, mask_height);

	return dst_image;
}

// 인핸스 포커스/디테일 필터
cv::Mat CFilter::GS_enhance_filter(cv::Mat src_image, int method)
{
	CUtil cUtil;

	int mask_height = 3;
	int mask_width = 3;

	float **mask = cUtil.GS_floatAlloc2D(mask_height, mask_width);

	// 인핸스 포커스 필터
	if (method == 0)
	{
		mask[0][0] = -1.0f; mask[0][1] = 0.0f; mask[0][2] = -1.0f;
		mask[1][0] = 0.0f; mask[1][1] = 7.0f; mask[1][2] = 0.0f;
		mask[2][0] = -1.0f; mask[2][1] = 0.0f; mask[2][2] = -1.0f;
	}
	// 인핸스 디테일 필터 
	else if (method == 1)
	{
		mask[0][0] = 0.0f; mask[0][1] = -1.0f; mask[0][2] = 0.0f;
		mask[1][0] = -1.0f; mask[1][1] = 9.0f; mask[1][2] = -1.0f;
		mask[2][0] = 0.0f; mask[2][1] = -1.0f; mask[2][2] = 0.0f;
	}

	// 2차원 마스크를 1차원으로 변환
	float *single_mask = cUtil.GS_toSingleArray(mask, mask_height, mask_width);

	// CvMat으로 변환
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, single_mask);
	

	float tmp;
	for (int i = 0; i<mask_height; i++)
	{
		for (int j = 0; j<mask_width; j++)
		{
			tmp = (float)mat_kernel.at<float>(i, j);

			if (method == 0) //cvmSet(&mat_kernel, i, j, tmp / 3.0f);
				mat_kernel.at<float>(i, j) = tmp / 3.0f;
			else if (method == 1) //cvmSet(&mat_kernel, i, j, tmp / 5.0f);
				mat_kernel.at<float>(i, j) = tmp / 5.0f;
		}
	}

	// 회선 
	cv::Mat dst_image = GS_conv2(src_image, mat_kernel);

	// 할당한 메모리 해제
	cUtil.GS_free1D(single_mask);
	cUtil.GS_free2D(mask, mask_height);

	return dst_image;
}

// 소프턴 필터
cv::Mat CFilter::GS_soften_filter(cv::Mat src_image, int method)
{
	CUtil cUtil;

	int mask_height = 3;
	int mask_width = 3;

	float **mask = cUtil.GS_floatAlloc2D(mask_height, mask_width);

	if (method == 0)
	{
		mask[0][0] = 11.0f; mask[0][1] = 11.0f; mask[0][2] = 11.0f;
		mask[1][0] = 11.0f; mask[1][1] = 11.0f; mask[1][2] = 11.0f;
		mask[2][0] = 11.0f; mask[2][1] = 11.0f; mask[2][2] = 11.0f;
	}
	else if (method == 1)
	{
		mask[0][0] = 1.0f; mask[0][1] = 1.0f; mask[0][2] = 1.0f;
		mask[1][0] = 1.0f; mask[1][1] = 10.0f; mask[1][2] = 1.0f;
		mask[2][0] = 1.0f; mask[2][1] = 1.0f; mask[2][2] = 1.0f;
	}
	else if (method == 2)
	{
		mask[0][0] = 1.0f; mask[0][1] = 1.0f; mask[0][2] = 1.0f;
		mask[1][0] = 1.0f; mask[1][1] = 4.0f; mask[1][2] = 1.0f;
		mask[2][0] = 1.0f; mask[2][1] = 1.0f; mask[2][2] = 1.0f;
	}

	// 2차원 마스크를 1차원으로 변환
	float *single_mask = cUtil.GS_toSingleArray(mask, mask_height, mask_width);

	// CvMat으로 변환
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, single_mask);

	float tmp;
	for (int i = 0; i<mask_height; i++)
	{
		for (int j = 0; j<mask_width; j++)
		{
			tmp = mat_kernel.at<float>(i, j);

			if (method == 0) // cvmSet(&mat_kernel, i, j, tmp / 99.0f);
				mat_kernel.at<float>(i, j) = tmp / 99.0f;
			else if (method == 1) // cvmSet(&mat_kernel, i, j, tmp / 18.0f);
				mat_kernel.at<float>(i, j) = tmp / 18.0f;
			else if (method == 2) // cvmSet(&mat_kernel, i, j, tmp / 12.0f);
				mat_kernel.at<float>(i, j) = tmp / 12.0f;
		}
	}

	// 회선 
	cv::Mat dst_image = GS_conv2(src_image, mat_kernel);

	// 할당한 메모리 해제
	cUtil.GS_free1D(single_mask);
	cUtil.GS_free2D(mask, mask_height);

	return dst_image;
}

// 샤프닝
cv::Mat CFilter::GS_sharpening(cv::Mat src_image, int method)
{
	CUtil cUtil;

	int mask_height = 3;
	int mask_width = 3;

	float **mask = cUtil.GS_floatAlloc2D(mask_height, mask_width);

	if (method == 0)
	{
		mask[0][0] = 1.0f; mask[0][1] = -2.0f; mask[0][2] = 1.0f;
		mask[1][0] = -2.0f; mask[1][1] = 5.0f; mask[1][2] = -2.0f;
		mask[2][0] = 1.0f; mask[2][1] = -2.0f; mask[2][2] = 1.0f;
	}
	else if (method == 1)
	{
		mask[0][0] = 0.0f; mask[0][1] = -1.0f; mask[0][2] = 0.0f;
		mask[1][0] = -1.0f; mask[1][1] = 5.0f; mask[1][2] = -1.0f;
		mask[2][0] = 0.0f; mask[2][1] = -1.0f; mask[2][2] = 0.0f;
	}
	else if (method == 2)
	{
		mask[0][0] = -1.0f; mask[0][1] = -1.0f; mask[0][2] = -1.0f;
		mask[1][0] = -1.0f; mask[1][1] = 9.0f; mask[1][2] = -1.0f;
		mask[2][0] = -1.0f; mask[2][1] = -1.0f; mask[2][2] = -1.0f;
	}
	// 명암대비 개선 마스크로 알려져 있다.
	else if (method == 3)
	{
		mask[0][0] = 1.0f; mask[0][1] = 1.0f; mask[0][2] = 1.0f;
		mask[1][0] = 1.0f; mask[1][1] = -7.0f; mask[1][2] = 1.0f;
		mask[2][0] = 1.0f; mask[2][1] = 1.0f; mask[2][2] = 1.0f;
	}
	else if (method == 4)
	{
		mask[0][0] = 0.0f;      mask[0][1] = -1.0f / 6.0f;  mask[0][2] = 0.0f;
		mask[1][0] = -1.0f / 6.0f; mask[1][1] = 10.0f / 6.0f;  mask[1][2] = -1.0f / 6.0f;
		mask[2][0] = 0.0f;      mask[2][1] = -1.0f / 6.0f;   mask[2][2] = 0.0f;
	}
	else if (method == 5)
	{
		mask[0][0] = 0.0f;      mask[0][1] = -1.0 / 4.0f;  mask[0][2] = 0.0f;
		mask[1][0] = -1.0f / 4.0f; mask[1][1] = 8.0f / 4.0f; mask[1][2] = -1.0f / 4.0f;
		mask[2][0] = 0.0f;      mask[2][1] = -1.0 / 4.0f;  mask[2][2] = 0.0f;
	}

	// 2차원 마스크를 1차원으로 변환
	float *single_mask = cUtil.GS_toSingleArray(mask, mask_height, mask_width);

	// CvMat으로 변환
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, single_mask);

	// 회선 
	cv::Mat dst_image = GS_conv2(src_image, mat_kernel);

	// 할당한 메모리 해제
	cUtil.GS_free1D(single_mask);
	cUtil.GS_free2D(mask, mask_height);

	return dst_image;
}

// 고주파 지원 통과 필터링
cv::Mat CFilter::GS_high_boost(cv::Mat src_image, float alpha, float bias)
{
	CUtil cUtil;

	int mask_height = 3;
	int mask_width = 3;
	int mask_mid_height = (mask_height + 1) / 2;
	int mask_mid_width = (mask_width + 1) / 2;

	// 마스크 계수 생성
	float mask_weights = (float)(mask_height*mask_width);

	// 마스크 초기화
	cv::Mat mat_kernel = cv::Mat(cv::Size(mask_width, mask_height),
						CV_32FC1,
						cv::Scalar(-alpha/mask_weights));

	// 중심 계수 삽입
	float mask_mid_var = (float)(mask_weights * alpha - 1.0) / mask_weights;
	mat_kernel.at<float>(mask_mid_height, mask_mid_width) = mask_mid_var;
	// 회선 
	cv::Mat tmp_image = GS_conv2(src_image, mat_kernel);

	// bias만큼 더해준다.
	cv::Mat dst_image = tmp_image.clone();
	cv::add(tmp_image, cv::Scalar(bias), dst_image);

	// 할당한 메모리 해제 
	mat_kernel.release();
	tmp_image.release();

	return dst_image;
}

// 언샤프닝
cv::Mat CFilter::GS_unsharpening(cv::Mat src_image, int method, float bias)
{
	CPixel cPixel;

	// 저주파 통과 필터링 : 블러링
	cv::Mat blurring_image = GS_blurring(src_image, 0, method);

	// 원 영상 - 저주파 통과 필터링 영상
	cv::Mat tmp_image = cPixel.GS_subtract_image(src_image, blurring_image);

	// bias 만큼 더해준다.
	cv::Mat dst_image = tmp_image.clone();
	cv::add(tmp_image, cv::Scalar(bias), dst_image);

	// 할당한 메모리 해제
	tmp_image.release();

	return dst_image;
}

// 블러링
cv::Mat CFilter::GS_blurring(cv::Mat src_image, int option, int method)
{
	cv::Mat dst_image;

	// 직접 구현한 함수 사용
	if (option == 0)
	{
		dst_image = GS_blurring_cvFilter2D(src_image, method);
	}
	// OpenCV 기본 제공 함수 사용
	else
	{
		dst_image = GS_blurring_cvSmooth(src_image, option, method);
	}

	return dst_image;
}

// 블러링 - 직접 구현
cv::Mat CFilter::GS_blurring_cvFilter2D(cv::Mat src_image, int method)
{
	CUtil cUtil;
	int mask_height, mask_width;

	// method : 0이면 3x3 마스크, 1이면 5x5 마스크, 2이면 7x7 마스크
	if (method == 0) mask_height = mask_width = 3;
	else if (method == 1) mask_height = mask_width = 5;
	else if (method == 2) mask_height = mask_width = 7;

	// 마스크 초기화
	cv::Mat mat_kernel = cv::Mat(mask_height, mask_width, CV_32FC1, cv::Scalar(0));

	// 마스크내 계수 생성
	int mask_length = mask_height * mask_width;
	for (int i = 0; i<mask_height; i++)
		for (int j = 0; j<mask_width; j++)
			mat_kernel.at<float>(i, j) = 1.0f / (float)mask_length;

	// 회선 
	cv::Mat dst_image = GS_conv2(src_image, mat_kernel);

	// 할당한 메모리 해제 
	mat_kernel.release();

	return dst_image;
}

// 블러링 - OpenCV 기본 제공 함수 사용
cv::Mat CFilter::GS_blurring_cvSmooth(cv::Mat src_image, int option, int method)
{
	int mask_height, mask_width;

	// 초기화
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	// method : 0이면 3x3 마스크, 1이면 5x5 마스크, 2이면 7x7 마스크
	if (method == 0) mask_height = mask_width = 3;
	else if (method == 1) mask_height = mask_width = 5;
	else if (method == 2) mask_height = mask_width = 7;

	// option 값 체크
	// option : 1 - CV_BLUR,   
	// option : 2 - CV_GAUSSIAN
	// option : 3 - CV_MEDIAN
	switch (option) {
	case 1: // CV_BLUR
		cv::blur(src_image, dst_image, cv::Size(mask_width, mask_height));
		break;
	case 2: // CV_GAUSSIAN
		cv::GaussianBlur(src_image, dst_image, cv::Size(mask_width, mask_height), 0, 0);
		break;
	case 3: // CV_MEDIAN
		cv::medianBlur(src_image, dst_image, 3); // ksize값 다시 수정해야함.
		break;
	case 4: // CV_BILATERAL
		cv::bilateralFilter(src_image, dst_image, -1 , 100, 100); // d<0 이면 d 값은 Sigmaspace로부터 계산됨.
	}

	return dst_image;
}

// 명암도 영상에서의 양각 효과 - 거친 효과
cv::Mat CFilter::GS_wild_embossing(cv::Mat src_image, int method)
{
	CUtil cUtil;
	int mask_length = 9;

	// North
	float mask_north[] = { -1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f };
	float mask_west[] = { -1.0f, 0.0f, 1.0f, -2.0f, 0.0f, 2.0f, -1.0f, 0.0f, 1.0f };
	float mask_south[] = { 1.0, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -2.0f, -1.0f };
	float mask_east[] = { 1.0f, 0.0f, -1.0f, 2.0f, 0.0f, -2.0f, 1.0f, 0.0f, -1.0f };
	float mask_northeast[] = { 0.0f, -1.0f, -2.0f, 1.0f, 0.0f, -1.0f, 2.0f, 1.0f, 0.0f };


	// 1차원 메모리 할당
	float *mask = cUtil.GS_floatAlloc1D(mask_length);

	// 마스크 복사
	if (method == 0)
	{
		memcpy(mask, mask_north, sizeof(float)*mask_length);
	}
	else if (method == 1)
	{
		memcpy(mask, mask_west, sizeof(float)*mask_length);
	}
	else if (method == 2)
	{
		memcpy(mask, mask_south, sizeof(float)*mask_length);
	}
	else if (method == 3)
	{
		memcpy(mask, mask_east, sizeof(float)*mask_length);
	}
	else if (method == 4)
	{
		memcpy(mask, mask_northeast, sizeof(float)*mask_length);
	}

	// CvMat으로 변환
	cv::Mat mat_kernel = cv::Mat(3, 3, CV_32FC1, mask);

	// 회선 
	cv::Mat tmp_image = GS_conv2(src_image, mat_kernel);

	// 원 영상의 평균값을 구한다.
	cv::Scalar avg = cv::mean(src_image);

	// 평균값을 더한다.
	cv::Mat dst_image = tmp_image.clone();
	cv::add(tmp_image, avg, dst_image);

	// 할당한 메모리 해제
	tmp_image.release();
	cUtil.GS_free1D(mask);

	return dst_image;
}

// HSV 컬러 공간에서의 양각 효과
cv::Mat CFilter::GS_hsv_embossing(cv::Mat src_image, int method)
{
	// 채널 분리
	cv::Mat rgb[3]; // 주의 ! 0 - blue 1 - green 2 - red
	cv::Mat hsv[3]; //0 - hue  1 - saturation 2 - value

	CColor cColor;
	cColor.GS_splitRGB(src_image, rgb);
	cColor.GS_rgb2hsv(rgb, hsv);

	// 양각 효과 수행
	cv::Mat value_embossing = GS_embossing(hsv[3], method);

	cv::Mat new_rgb[3];
	hsv[2] = value_embossing.clone();
	cColor.GS_hsv2rgb(hsv, new_rgb);

	cv::Mat dst_image = cColor.GS_compositeRGB(new_rgb);

	// 할당한 메모리 해제
	rgb->release(); new_rgb->release();

	hsv->release();
	value_embossing.release();

	return dst_image;
}

// RGB 컬러 공간에서의 양각 효과
cv::Mat CFilter::GS_rgb_embossing(cv::Mat src_image, int method)
{
	/** 참고 : cvFilter2D() 함수는 3차원까지 처리할 수 있으므로,
	아래 코드 대신 바꿔도 똑같다.
	return GS_embossing( src_image, method );
	*/

	CColor cColor;

	// 채널 분리
	cv::Mat rgb[3];
	cColor.GS_splitRGB(src_image, rgb);

	// 양각 효과 수행
	// 채널 분리는 rgb 순이 아닌 bgr 순으로 된다는 것을 참고
	cv::Mat blue_embossing = GS_embossing(rgb[0], method);
	cv::Mat green_embossing = GS_embossing(rgb[1], method);
	cv::Mat red_embossing = GS_embossing(rgb[2], method);
	

	cv::Mat dst_image = cColor.GS_compositeRGB(rgb);

	// 할당한 메모리 해제
	rgb->release();
	red_embossing.release();
	green_embossing.release();
	blue_embossing.release();

	return dst_image;
}

// 명암도 영상에서의 양각 효과
cv::Mat CFilter::GS_embossing(cv::Mat src_image, int method)
{
	CUtil cUtil;
	int mask_height = 3;
	int mask_width = 3;
	int mask_length = mask_height * mask_width;

	// 135도
	float mask_135[] = { -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f };
	// 90도
	float mask_90[] = { 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f };

	// 45도 
	float mask_45[] = { 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f };

	// 1차원 메모리 할당
	float *mask = cUtil.GS_floatAlloc1D(mask_length);

	// 마스크 복사
	if (method == 0)
	{
		memcpy(mask, mask_135, sizeof(float)*mask_length);
	}
	else if (method == 1)
	{
		memcpy(mask, mask_90, sizeof(float)*mask_length);
	}
	else if (method == 2)
	{
		memcpy(mask, mask_45, sizeof(float)*mask_length);
	}

	cv::Mat mat_kernel = cv::Mat(cv::Size(mask_width, mask_height), CV_32FC1, mask);

	// 회선 
	cv::Mat tmp_image = GS_conv2(src_image, mat_kernel);

	// 원 영상의 평균값을 구한다.
	cv::Scalar avg = mean(src_image, NULL);

	// 평균값을 더한다.
	cv::Mat dst_image = tmp_image.clone();
	cv::add(tmp_image, avg, dst_image);

	// 할당한 메모리 해제
	tmp_image.release();
	cUtil.GS_free1D(mask);

	return dst_image;
}

// 회선 수행
cv::Mat CFilter::GS_conv2(cv::Mat src_image, cv::Mat mat_kernel)
{
	// 회선 수행(=필터링) 결과를 담을 목적 영상 초기화
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	// 필터링 수행 : 마스크에 의한 회선(convolution)
	cv::filter2D(src_image, dst_image, dst_image.depth(), mat_kernel);

	return dst_image;
}

// 1차원 커널을 갖고 필터링. - ①
cv::Mat CFilter::ApplyFilter2D(cv::Mat src_image, float * kernel, int kHeight, int kWidth)
{
	// cv::Mat으로 변환.
	cv::Mat mat_kernel = cv::Mat(cv::Size(kWidth, kHeight), CV_32FC1, kernel);

	// 오버로딩된 멤버 함수 ③ 호출
	cv::Mat dst_image = ApplyFilter2D(src_image, mat_kernel);

	// 할당한 메모리 해제 
	mat_kernel.release();

	return dst_image;
}

// 2차원 커널을 갖고 필터링. - ②
cv::Mat CFilter::ApplyFilter2D(cv::Mat src_image, float ** kernel, int kHeight, int kWidth)
{
	int cnt = -1;

	// 2차원을 1차원으로 변환.
	float *single_kernel = (float *)calloc(kHeight*kWidth, sizeof(float));

	for (int i = 0; i<kHeight; i++)
	{
		for (int j = 0; j<kWidth; j++)
		{
			single_kernel[++cnt] = kernel[i][j];
		}
	}

	// 오버로딩된 멤버 함수 ① 호출.
	cv::Mat dst_image = ApplyFilter2D(src_image, single_kernel, kHeight, kWidth);

	// 할당한 메모리 해제
	free(single_kernel);

	return dst_image;
}

// CvMat인 kernel을 갖고 필터링. - ③
cv::Mat CFilter::ApplyFilter2D(cv::Mat src_image, cv::Mat kernel)
{
	// 필터링 결과를 담을 목적 영상 초기화
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type());

	// 필터링 수행 : kernel에 의한 회선(convolution)
	cv::filter2D(src_image, dst_image, -1 , kernel);

	return dst_image;
}


