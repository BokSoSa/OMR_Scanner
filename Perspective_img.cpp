#include <opencv2/opencv.hpp>
#include "SortUserAnswer.h"
using namespace cv;
using namespace std;

Mat perspective_img(Mat img);
Mat checkAnswerBySort(Mat origin, Mat input, int answer[]);
Mat remove_shadow(Mat img);

struct locate {
	int x;
	int y;
};

locate xy[4];
int loc_idx = 0;
Mat omrImg;

void onMouseEvent(int event, int x, int y, int flags, void* dstImg) {
	if (event == EVENT_LBUTTONDOWN) {
		xy[loc_idx].x = x;
		xy[loc_idx].y = y;
		loc_idx++;
		cout << "loc_idx: " << loc_idx << endl;
		if (loc_idx == 4) {
			Mat rmImg;
			rmImg = remove_shadow(omrImg);

			Mat cropOrigin = perspective_img(omrImg);
			Mat crop = perspective_img(rmImg);

			int correctAnswer[40] = { 3,4,3,2,5, 1,4,3,1,3,
									4,5,3,1,2, 2,3,3,5,5,
									2,1,1,5,4, 3,2,1,4,5,
									1,2,1,1,4, 2,3,3,2,5 };

			Mat cropOrigin2 = cropOrigin.clone();
			//time_t start, end;
			//start = clock();
			checkAnswerBySort(cropOrigin, crop, correctAnswer);
			//end = clock();
			//cout << "first time : " << (double)(end - start) << endl;
			//start = clock();
			checkAnswerByNumberTemplate(cropOrigin2, crop, correctAnswer);
			//end = clock();
			//cout << "second time : " << (double)(end - start) << endl;
		}
	}
	return;
}

void mouseClick(Mat img) {
	omrImg = img;
	imshow("result", img);
	setMouseCallback("result", onMouseEvent, (void*)&img);
}

Mat perspective_img(Mat img) {
	// 인풋 이미지 평면 좌표 입력
	Point2f inputQuad[4];
	// 출력 이미지 평면 좌표
	Point2f outputQuad[4];

	// Lambda Matrix
	Mat lambda(2, 4, CV_32FC1);
	//Mat 형식의 입력,출력 이미지 
	Mat output;
	// Set the lambda matrix the same type and size as input
	lambda = Mat::zeros(img.rows, img.cols, img.type());

	// 4점 포인트를 시계방향으로 설정해준다
	inputQuad[0] = Point2f(xy[0].x, xy[0].y); // 왼쪽위
	inputQuad[1] = Point2f(xy[1].x, xy[1].y); // 오른쪽위
	inputQuad[2] = Point2f(xy[2].x, xy[2].y); // 오른쪽아래
	inputQuad[3] = Point2f(xy[3].x, xy[3].y); // 왼쪽아래
	// 맵핑될 이미지를 시계방향으로 잡아준다.
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f((xy[1].x - xy[0].x)*2, 0);
	outputQuad[2] = Point2f((xy[1].x - xy[0].x) * 2, (xy[2].y - xy[1].y) * 2);
	outputQuad[3] = Point2f(0, (xy[2].y - xy[1].y) * 2);

	// 입력,출력 Quad를 통해 getPerspectiveTransform함수를 이용하여 변환행렬을 구함
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	// 이미지 와핑
	warpPerspective(img, output, lambda, Size((xy[1].x - xy[0].x) * 2, (xy[2].y - xy[1].y) * 2));//입력,출력,변환행렬,크기

	return output;
}
