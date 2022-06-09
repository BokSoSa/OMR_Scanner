#include <opencv2/opencv.hpp>
#include "SortUserAnswer.h"
using namespace cv;
using namespace std;

Mat perspective_img(Mat img);
void findAnswers(Mat img, int ansW, int ansH, double rat);
vector<Point> templateMatch(Mat img, Mat tImg, double threshold);


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
	}
	if (loc_idx == 4) {
		Mat crop = perspective_img(omrImg);

		imshow("cropped", crop);
		double ratio = double(crop.cols) / double(crop.rows); // crop 이미지 가로세로 비율
		int op = 0;
		if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5번
		else if (ratio < 0.8) op = 2;						// 1~10번
		else if ((ratio > 1.0) && (ratio < 1.5)) op = 3;	// 1~20번
		else if ((ratio > 1.5) && (ratio < 2.0)) op = 4;	// 1~30번
		else op = 5;										// 1~40번

		Mat tempMarking = imread("resource/templateImage/template0.png");
		Mat tempCircle = imread("resource/templateImage/template2.png");
		if (op == 1) { // 1~5번일 때
			resize(tempMarking, tempMarking, Size(crop.rows * 0.049, crop.rows * 0.089));
			resize(tempCircle, tempCircle, Size(crop.rows * 0.049, crop.rows * 0.089));
		}
		else {
			resize(tempMarking, tempMarking, Size(crop.rows * 0.028, crop.rows * 0.05));
			resize(tempCircle, tempCircle, Size(crop.rows * 0.028, crop.rows * 0.05));
		}

		vector<Point> checkedAnswer = templateMatch(crop, tempMarking, 0.15);
		vector<Point> uncheckedAnswer = templateMatch(crop, tempCircle, 0.5);
		//cout << "???: " << checkedAnswer.size() << endl;
		vector<AnswerByChecked> answers = mergeCheckedAnswer(checkedAnswer, uncheckedAnswer);
		answers = sortAnswerByPoint(answers, tempMarking.cols, tempMarking.rows);
		
		int count = 0;
		for (int i = 0; i < answers.size(); i++) {
			//cout << i + 1 << " ";
			//cout << answers[i].getAnswerPoint().x << ", " << answers[i].getAnswerPoint().y << " : " << answers[i].isChecked()<<endl;
			//cout << "(" << answers[i].getAnswerPoint().x << "," << answers[i].getAnswerPoint().y << ") = " << answers[i].isChecked() << " / ";
			count++;
			if (answers[i].isChecked() == 1)
				cout << "Tester's Answer: " << count << endl;
			if (count == 5) count = 0;
		}

		return;
	}
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