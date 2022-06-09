#include <opencv2/opencv.hpp>
#include "SortUserAnswer.h"
using namespace cv;
using namespace std;

Mat perspective_img(Mat img);
void findAnswers(Mat img, int ansW, int ansH, double rat);
Mat checkAnswerBySort(Mat input, int answer[]);
vector<Rect> find_number_location(Mat image);
void recognize_number(Mat original, vector<Rect> boundRect);

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

			Mat crop = perspective_img(omrImg);

			int correctAnswer[40] = { 2,4,3,4,1,2,3,2,1,3,2,4,4,2,1,3,5,3,4,2 };

			imshow("cropped", crop);
			checkAnswerBySort(crop, correctAnswer);
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

vector<Rect> find_number_location(Mat image) {
	Mat drawing;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	double ratio;
	int refinery_count = 0;

	cvtColor(image, image, COLOR_BGR2GRAY);
	imshow("gray", image);

	//Canny(image, image, 100, 300, 3);
	//imshow("canny", image);

	// Finding contours
	findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Rect> boundRect2(contours.size());

	// Bind rectangle to every rectangle.
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	drawing = Mat::zeros(image.size(), CV_8UC3);

	for (int i = 0; i < contours.size(); i++) {

		ratio = (double)boundRect[i].height / boundRect[i].width;

		// Filtering rectangles height/width ratio, and size.
		if (((ratio <= 3.5) && (ratio >= 0.5)) && ((boundRect[i].area() <= 500) && (boundRect[i].area() >= 70))) {

			drawContours(drawing, contours, i, Scalar(0, 255, 255), 1, 8, hierarchy, 0, Point());
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);

			// Include only suitable rectangles.
			boundRect2[refinery_count] = boundRect[i];
			refinery_count += 1;
		}
	}

	boundRect2.resize(refinery_count);  //  Resize refinery rectangle array.

	imshow("Contours&Rectangles", drawing);


	cout << "boundRect2.size()" << boundRect2.size() << endl;

	Mat drawingPoint = image;
	for (int i = 0; i < boundRect2.size(); i++) {
		cout << boundRect2[i] << endl;
		//cout << "X : " << boundRect2[i].x <<", Y : " << boundRect2[i].y << endl;
		rectangle(drawingPoint, boundRect2[i].tl(), boundRect2[i].br(), Scalar(255, 0, 0), 1, 8, 0);
	}
	imshow("drawingRect", drawingPoint);

	return boundRect2;
}

void recognize_number(Mat original, vector<Rect> boundRect) {
	vector<Rect> realNumber;

	for (int i = 0; i < boundRect.size(); i++) {
		cout << boundRect[i] << endl;
		Mat number = original(boundRect[i]);
		string name = "number" + to_string(i);
		imshow(name, number);
	}
	waitKey();
}
