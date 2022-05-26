// Geometry.cpp: implementation of the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "Geometry.h"
#include "Pixel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define cvWarpPerspectiveQMatrix cvGetPerspectiveTransform
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometry::CGeometry()
{

}

CGeometry::~CGeometry()
{

}


// 관심 영역 추출 (extraction the region of interest)
// 관심 영역을 return함.
// Mat(Rect(int x,int y,int width,int height)) 와 동일
cv::Mat CGeometry::GS_getROI(cv::Mat src_image, int startH, int startW, int height, int width)
{
	cv::Rect roi;
	roi.y = startH; // 높이에 대한 시작 위치 (Starting point for height)
	roi.x = startW; // 너비에 대한 시작 위치 (Starting point for width)
	roi.height = height; // 가져올 높이에 대한 범위 (
	roi.width = width; // 가져올 너비에 대한 범위

	cv::Mat	dst_image = src_image(roi);

	return dst_image;
}

// 관심 영역 추출된 것을 원래대로 복원  (restoration of the region of interest that was extracted)
// cv::Mat 자료형은 해당 기능 없음.
//void CGeometry::GS_resetROI(IplImage *roi_image)
//{
//    cvResetImageROI(roi_image);
//}


// 반사 (Reflection) or 대칭
cv::Mat CGeometry::GS_reflect(cv::Mat src_image, int flip_mode)
{
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type(), cv::Scalar(0));

	// flip_mode = 0 : 상하 뒤집기(flipping)
	// flip_mode > 0 : 좌우 뒤집기(mirroring)
	// flip_mode < 0 : 상하 뒤집기, 좌우 뒤집기 순 수행(flipping + mirroring)
	flip(src_image, dst_image, flip_mode);
	
	return dst_image;
}

// 원근 변환 (Perspective transform)
cv::Mat CGeometry::GS_perspective(cv::Mat src_image, cv::Point2f src_point, cv::Point2f dst_point)
{
	// 초기화 (Initialization)
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type(), cv::Scalar(0));

	cv::Mat map_matrix = cv::Mat(3, 3, CV_32FC1);

	cv::getPerspectiveTransform((cv::InputArray)src_point, (cv::InputArray)dst_point);
	cv::warpPerspective(src_image, dst_image, map_matrix, dst_image.size());

	// 할당한 메모리 해제 (memory deallocation)
	map_matrix.release();

	return dst_image;
}

// 어파인 변환 (Affine transform)
cv::Mat CGeometry::GS_affine(cv::Mat src_image, cv::Mat map_matrix)
{
	// 초기화 (Initialization)
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type(), cv::Scalar(0));

	warpAffine(src_image, dst_image, map_matrix, dst_image.size(), cv::INTER_LINEAR + cv::WARP_FILL_OUTLIERS);

	return dst_image;
}

// 어파인 변환을 이용한 회전 (Rotation using affine transform)
cv::Mat CGeometry::GS_affineRotate(cv::Mat src_image, double angle, double scale)
{
	// 초기화 (Initialization)
	int height = src_image.rows;
	int width = src_image.cols;
	cv::Mat dst_image = cv::Mat(src_image.size(), src_image.type(), cv::Scalar(0));

	// 회전 중심점 (center point of rotation)
	cv::Point2f center(width / 2.0, height / 2.0);
	
	cv::Mat map_matrix = cv::Mat(2, 3, CV_32FC1);

	// angle : 회전 각도
	// scale : 확대/축소 비율 , 양의 실수여야함.
	map_matrix = cv::getRotationMatrix2D(center, angle, scale);
	
	warpAffine(src_image, dst_image, map_matrix, dst_image.size(), cv::INTER_LINEAR + cv::WARP_FILL_OUTLIERS);

	// 할당한 메모리 해제 (memory deallocation)
	map_matrix.release();

	return dst_image;
}

