#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "ImageAnalysis.h"

using namespace cv;

int main() {
	// 카메라 불러오기
	// 내장 웹캠 카메라 번호 : 0
	VideoCapture cap(0);
	if (!cap.isOpened()) { return-1; }		// 카메라 연결 안될 시 종료

	CEdge cedge = CEdge();
	CImageAnalysis cimageAnalysis = CImageAnalysis();


	Mat cam;
	while (1) {		// 계속해서 입력되는 웹캠 출력
		cap >> cam;
		rectangle(cam, Point(40, 100), Point(600, 350), Scalar(0, 255, 0), 3, 1, 0);
		imshow("cam", cam);

		Mat grayOMR;
		cvtColor(cam, grayOMR, COLOR_BGR2GRAY);
		Mat edgeDetect = cedge.GS_canny_edge_Canny(grayOMR, 10, 40);

		imshow("camera", edgeDetect);								// 결과 출력


		//Mat line = cimageAnalysis.GS_basicHoughTransformGray(grayOMR);
		//imshow("line", line);

		if (waitKey(1) == 27) break;	// ESC 입력되면 종료
	}
	Mat cutImage = cam(Range(50, 300), Range(20, 450));
	imshow("cut", cutImage);
	//Mat colorOMR = cam;
	//imshow("colorOMR", colorOMR);
	//waitKey();

	//// grayscale로 변환
	//Mat grayOMR;
	//cvtColor(colorOMR, grayOMR, COLOR_BGR2GRAY);
	//imshow("grayOMR", grayOMR);

	//CEdge cedge = CEdge();
	////Mat EdgeDetect = cedge.GS_laplacian_edge_Laplacian(grayOMR);
	//Mat binary = cedge.GS_canny_edge_Canny(grayOMR, 10, 40);

	////imshow("edge", EdgeDetect);
	//imshow("binary", binary);

	/*CImageAnalysis cimageAnalysis = CImageAnalysis();
	Mat line = cimageAnalysis.GS_basicHoughTransformGray(EdgeDetect);
	imshow("line", line);*/

	// 아무 키나 입력될 때까지 wait
	waitKey();
}
