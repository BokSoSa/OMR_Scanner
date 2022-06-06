#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;

Mat remove_shadow(Mat img);
void mouseClick(Mat img);
Mat get_cropped();

int main() {
	Mat omrImg = imread("resource/testimage/omr2_marking1.jpg");
	resize(omrImg, omrImg, Size(omrImg.cols / 4, omrImg.rows / 4));
	omrImg = remove_shadow(omrImg);

	mouseClick(omrImg);
	//Mat crop = get_cropped();
	
	waitKey();

	// 카메라 불러오기
	// 내장 웹캠 카메라 번호 : 0
	//VideoCapture cap(0);
	//if (!cap.isOpened()) { return-1; }		// 카메라 연결 안될 시 종료

	//CEdge cedge = CEdge();
	//CImageAnalysis cimageAnalysis = CImageAnalysis();
	//Mat src_img;
	//CImageAnalysis ci = CImageAnalysis();
	//Mat template_img = imread("template.png");
	//Mat screenshot;

	//while (1) {		// 계속해서 입력되는 웹캠 출력
	//	cap >> src_img;
	//	rectangle(src_img, Point(40, 100), cv::Point(600, 330), cv::Scalar(0, 255, 0), 3, 1, 0);
	//	//imshow("image1", src_img);

	//	Point temp_point;
	//	Mat result = ci.GS_templateMatching(src_img, template_img, temp_point);
	//	imshow("image2", result);

	//	if ((temp_point.x >= 40 && temp_point.x <= 45) && (temp_point.y >= 100 && temp_point.y <= 105)) {
	//		cout << temp_point << endl;
	//		for (int i = 0; i < 3; i++) {
	//			putText(result, to_string(i + 1), Point(250 + 50 * i, 50), 0, 2, Scalar(0, 255, 0), 5);
	//			cout << "COUNT :: " << i + 1 << endl;
	//			imshow("image2", result);
	//			waitKey(1000);
	//		}
	//		screenshot = src_img;
	//		break;
	//	}

	//	if (waitKey(1) == 27) break;	// ESC 입력되면 종료
	//}
	//screenshot = screenshot(Range(100, 330), Range(40, 600));
	//imshow("image3", screenshot);

	//Mat grayOMR;
	//cvtColor(screenshot, grayOMR, COLOR_BGR2GRAY);
	//Mat edgeDetect = cedge.GS_canny_edge_Canny(grayOMR, 10, 40);

	//imshow("image4", edgeDetect);								// 결과 출력

	//waitKey();
}
