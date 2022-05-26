#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "ImageAnalysis.h"

using namespace cv;

int main() {
	// ī�޶� �ҷ�����
	// ���� ��ķ ī�޶� ��ȣ : 0
	VideoCapture cap(0);
	if (!cap.isOpened()) { return-1; }		// ī�޶� ���� �ȵ� �� ����

	CEdge cedge = CEdge();
	CImageAnalysis cimageAnalysis = CImageAnalysis();


	Mat cam;
	while (1) {		// ����ؼ� �ԷµǴ� ��ķ ���
		cap >> cam;
		rectangle(cam, Point(40, 100), Point(600, 350), Scalar(0, 255, 0), 3, 1, 0);
		imshow("cam", cam);

		Mat grayOMR;
		cvtColor(cam, grayOMR, COLOR_BGR2GRAY);
		Mat edgeDetect = cedge.GS_canny_edge_Canny(grayOMR, 10, 40);

		imshow("camera", edgeDetect);								// ��� ���


		//Mat line = cimageAnalysis.GS_basicHoughTransformGray(grayOMR);
		//imshow("line", line);

		if (waitKey(1) == 27) break;	// ESC �ԷµǸ� ����
	}
	Mat cutImage = cam(Range(50, 300), Range(20, 450));
	imshow("cut", cutImage);
	//Mat colorOMR = cam;
	//imshow("colorOMR", colorOMR);
	//waitKey();

	//// grayscale�� ��ȯ
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

	// �ƹ� Ű�� �Էµ� ������ wait
	waitKey();
}
