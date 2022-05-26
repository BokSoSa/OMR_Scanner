// Edge.h: interface for the CEdge class.
//
//////////////////////////////////////////////////////////////////////

#include<opencv2/opencv.hpp>

#if !defined(AFX_EDGE_H__F2A61D84_5347_4099_AEA6_2061F2B32778__INCLUDED_)
#define AFX_EDGE_H__F2A61D84_5347_4099_AEA6_2061F2B32778__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEdge
{
public:
	CEdge();
	virtual ~CEdge();

	// 소벨 에지 추출 : 마스크(3x3)
	cv::Mat GS_sobel_edge(cv::Mat src_image, int method = 2);

	// 소벨 에지 추출 : cv::Sobel() 함수 이용
	cv::Mat GS_sobel_edge_Sobel(cv::Mat src_image, int method = 2);

	// 소벨 에지 추출 : 마스크(7x7)
	cv::Mat GS_sobel_edge_7x7(cv::Mat src_image, int method = 2);

	// 소벨 에지 추출 : 대각선
	cv::Mat GS_sobel_diagonal_edge(cv::Mat src_image, int method = 2);

	// 프리윗 에지 추출
	cv::Mat GS_prewitt_edge(cv::Mat src_image, int method = 2);

	// 프레이첸 에지 추출 
	cv::Mat GS_freichen_edge(cv::Mat src_image, int method = 2);

	// 서터캐스틱 에지 추출
	cv::Mat GS_stochastic_Edge(cv::Mat src_image, int method = 2);

	// 로버츠 에지 추출
	cv::Mat GS_roberts_edge(cv::Mat src_image, int method = 2);

	// 라플라시안 에지 추출 : 마스크
	cv::Mat GS_laplacian_edge(cv::Mat src_image, int method = 1);

	// 라플라시안 에지 추출 : cv::Laplacian() 함수 이용
	cv::Mat GS_laplacian_edge_Laplacian(cv::Mat src_image);

	// 라플라시안 샤프닝 
	cv::Mat GS_laplacian_sharpening(cv::Mat src_image, int method = 1);

	double **GS_get_LoG_mask(double sigma, int *mask_height, int *mask_width);
	double GS_get_LoG_formular(double x, double sigma);
	double norm(double x, double y);
	double GS_get_distance(double a, double b, double c, double d);
	double gauss(double x, double sigma);

	// 캐니 에지 추출 
	cv::Mat GS_canny_edge(cv::Mat src_image, double sigma, int threshold, int method);

	// 캐니 에지 추출 : cv::Canny() 함수 이용 
	cv::Mat GS_canny_edge_Canny(cv::Mat src_image, double low_threshold = 50.0, double high_threshold = 100.0);

};
#endif // !defined(AFX_EDGE_H__F2A61D84_5347_4099_AEA6_2061F2B32778__INCLUDED_)
