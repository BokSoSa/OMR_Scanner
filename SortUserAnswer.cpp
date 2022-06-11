#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"

int templateImgCols; //오차 범위 계산을 위한 template image의 가로 길이
int templateImgRows; //오차 범위 계산을 위한 template image의 세로 길이
vector<Point> templateMatch(Mat img, Mat tImg, double threshold);
vector<Point> templateMatch(Mat img, vector<Mat> tImg, double threshold);

Mat checkAnswerByNumberTemplate(Mat origin, Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop 이미지 가로세로 비율
	int op = 0;
	int upQuestion = 0;
	if ((ratio > 0.8) && (ratio < 1)) {					// 1~5번	1단
		op = 1;
		upQuestion = 5;
	}
	else if (ratio < 0.8) {								// 1~10번	1단
		op = 1;
		upQuestion = 5;
	}
	else if ((ratio > 1.0) && (ratio < 1.5)) {			// 1~20번	2단
		op = 2;
		upQuestion = 10;
	}
	else if ((ratio > 1.5) && (ratio < 2.0)) {		 	// 1~30번	3단
		op = 3;
		upQuestion = 15;
	}
	else {												// 1~40번	4단
		op = 4;
		upQuestion = 20;
	}

	Mat tempMarking = imread("resource/templateImage/final0.png"); //체크된 이미지
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5번일 때
		resize(tempMarking, tempMarking, Size(input.rows * 0.103, input.rows * 0.103));
	}
	else {
		resize(tempMarking, tempMarking, Size(input.rows * 0.059, input.rows * 0.059));
	}
	// 체크된 답 좌표들
	vector<Point> checkedAnswer = templateMatch(input, tempMarking, 0.3);
	
	Mat removeAnswer; //체크된 답 지운 이미지 저장할 객체
	removeAnswer = input.clone();
	for (int i = 0; i < checkedAnswer.size(); i++) {
		rectangle(removeAnswer, Point(checkedAnswer[i].x, checkedAnswer[i].y), Point(checkedAnswer[i].x + templateImgCols, checkedAnswer[i].y + templateImgRows), Scalar(0, 0, 0), FILLED);
	}

	vector<Mat> number;
	for (int i = 0; i < 40; i++) {
		string img_path = format("resource/number/%d.png", i + 1);
		number.push_back(imread(img_path));
		if ((ratio > 0.8) && (ratio < 1)) { // 1~5번일 때
			resize(number.back(), number.back(), Size(input.rows * 0.14, input.rows * 0.14));
		}
		else {
			resize(number.back(), number.back(), Size(input.rows * 0.075, input.rows * 0.075));
		}
		
	}

	vector<Point> question = templateMatch(removeAnswer, number, 0.5);

	templateImgCols = number[0].cols;
	templateImgRows = number[0].rows;

	int userAnswer[40] = { 0, };
	Point userAnswerPoint[40];
	for (int i = 0; i < 40; i++) {
		userAnswerPoint[i] = Point(0, 0);
	}

	sort(question.begin(), question.end(), compareAnswer2);
	sort(checkedAnswer.begin(), checkedAnswer.end(), compareAnswer2);
	vector<pair<int, Point>> yDupCheck;
	for (int i = 0; i < checkedAnswer.size(); i++) { // 사용자가 마킹한 답안
		bool flag = false;
		for (int j = upQuestion; j < question.size(); j++) { // 마킹된 문제 번호을 제외한 문제 개수
			if (abs(checkedAnswer[i].y - question[j].y) < templateImgCols) { // 문제 번호와 마킹한 답안의 y좌표의 차이
				for (int k = 0; k < upQuestion; k++) {
					if (abs(question[k].x - checkedAnswer[i].x) < templateImgCols) {
						yDupCheck.push_back(make_pair(k / 5 * 10 + (j - upQuestion) / op + 1, checkedAnswer[i]));
						flag = true;
						break;
					}
				}
			}
			if (flag) break;
		}
	}

	for (int i = 0; i < yDupCheck.size(); i++) {
		for (int j = 0; j < upQuestion; j++) {
			if (abs(yDupCheck[i].second.x - question[j].x) < templateImgRows) {
				userAnswer[yDupCheck[i].first - 1] = (j % 5) + 1;
				userAnswerPoint[yDupCheck[i].first - 1] = yDupCheck[i].second;
				break;
			}
		}
	}

	for (int i = 0; i < 40; i++) {
		cout << i + 1 << "번 : " << userAnswer[i] << endl;
	}
	int correctAnswerCount = 0;
	int maxAnswerCount = 0;
	for (int i = 0; i < 40; i++) {
		if (userAnswer[i] == 0) continue;
		maxAnswerCount = i+1;
		if (answer[i] == userAnswer[i]) {
			rectangle(origin, Rect(Point(userAnswerPoint[i].x, userAnswerPoint[i].y), Point(userAnswerPoint[i].x + templateImgCols, userAnswerPoint[i].y + templateImgRows)), Scalar(0, 255, 0), 2);
			correctAnswerCount++;
		}
		else {
			rectangle(origin, Rect(Point(userAnswerPoint[i].x, userAnswerPoint[i].y), Point(userAnswerPoint[i].x + templateImgCols, userAnswerPoint[i].y + templateImgRows)), Scalar(0, 0, 255), 2);
		}
	}

	double score = ((double)correctAnswerCount / maxAnswerCount * 100);
	putText(origin, "SCORE: " + to_string((int)score), Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255, 255, 0), 2);
	cout << "\n**** 채점 결과 ****" << endl;
	cout << "푼 문제 수: " << maxAnswerCount << endl;
	cout << "맞은 문제 수: " << correctAnswerCount << endl;
	cout << "점수: " << (int)score << "점" << endl;
	imshow("gradingResult2", origin);

	return origin;
}

Mat checkAnswerBySort(Mat origin, Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop 이미지 가로세로 비율
	int op = 0;
	if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5번	1단
	else if (ratio < 0.8) op = 1;						// 1~10번	1단
	else if ((ratio > 1.0) && (ratio < 1.5)) op = 2;	// 1~20번	2단
	else if ((ratio > 1.5) && (ratio < 2.0)) op = 3;	// 1~30번	3단
	else op = 4;										// 1~40번	4단

	Mat tempMarking = imread("resource/templateImage/final0.png");
	Mat tempCircle = imread("resource/templateImage/final4.png");

	
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5번일 때
		resize(tempMarking, tempMarking, Size(input.rows * 0.103, input.rows * 0.103));
		resize(tempCircle, tempCircle, Size(input.rows * 0.103, input.rows * 0.103));
	}
	else {
		resize(tempMarking, tempMarking, Size(input.rows * 0.059, input.rows * 0.059));
		resize(tempCircle, tempCircle, Size(input.rows * 0.059, input.rows * 0.059));
	}

	vector<Point> checkedAnswer = templateMatch(input, tempMarking, 0.3);
	vector<Point> uncheckedAnswer = templateMatch(input, tempCircle, 0.55);

	vector<AnswerByChecked> answers = mergeCheckedAnswer(checkedAnswer, uncheckedAnswer);
	answers = sortAnswerByPoint(answers, tempMarking.cols, tempMarking.rows);

	int** userAnswer = new int* [op];
	Point** userAnswerPoint = new Point* [op];
	for (int i = 0; i < op; i++) {
		userAnswer[i] = new int[10];
		userAnswerPoint[i] = new Point[10];
		for (int j = 0; j < 10; j++) {
			userAnswer[i][j] = 0;
			userAnswerPoint[i][j] = Point(0,0);
		}
	}

	int checkedNumber = 0;
	int answerX = 0, answerY = 0;
	for (int i = 0; i < answers.size(); i++) {
		checkedNumber++;
		if (answers[i].isChecked() == 1) {
			userAnswer[answerY][answerX] = checkedNumber;
			userAnswerPoint[answerY][answerX] = answers[i].getAnswerPoint();
		}
		if (checkedNumber == 5) {
			checkedNumber = 0;
			answerY++;
			if (answerY >= op) {
				answerY = 0;
				answerX++;
			}
		}

	}

	Mat result;
	input.copyTo(result);

	int correctAnswerCount = 0;
	int maxAnswerCount = 0;
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < 10; j++) {
			cout << i * 10 + j + 1 << "번: " << userAnswer[i][j] << endl;
			Point drawTL = userAnswerPoint[i][j];
			if (userAnswer[i][j]==0) continue;

			maxAnswerCount=i*10+j+1;
			if (userAnswer[i][j] == answer[i * 10 + j]) {
				correctAnswerCount++;
				rectangle(origin, drawTL, Point(drawTL.x + templateImgCols, drawTL.y + templateImgRows), Scalar(0, 255, 0), 2);
			}
			else {
				rectangle(origin, drawTL, Point(drawTL.x + templateImgCols, drawTL.y + templateImgRows), Scalar(0, 0, 255), 2);
			}
		}
	}
	cout << "MAX: " << maxAnswerCount << " correct: "<<correctAnswerCount << endl;
	//채점 + 결과 표시
	//정답이 20개인데 input image가 40번까지 있는 경우 고려해서 작성
	//틀린 경우 답체크 X 틀림 여부만 체크
	//답을 체크 안한경우는 답체크X 틀림여부 체크도X

	double score = ((double)correctAnswerCount / maxAnswerCount * 100);
	putText(origin, "SCORE: "+to_string((int)score), Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255, 255, 0), 2);
	cout << "\n**** 채점 결과 ****" << endl;
	cout << "푼 문제 수: " << maxAnswerCount << endl;
	cout << "맞은 문제 수: " << correctAnswerCount << endl;
	cout << "점수: " << (int)score << "점" << endl;
	
	imshow("gradingResult1", origin);

	return result;
}


// 사용자가 입력한 답과 입력하지 않은 답을 checked로 구분하여 AnswerBychecked Vector에 저장
vector<AnswerByChecked> mergeCheckedAnswer(vector<Point> checkedVector, vector<Point> uncheckedVector){
	vector<AnswerByChecked> devidedSheetByCheckedVector;
	for (int i = 0; i < checkedVector.size(); i++) {
		devidedSheetByCheckedVector.push_back(AnswerByChecked(checkedVector[i], true));
	}

	for (int i = 0; i < uncheckedVector.size(); i++) {
		devidedSheetByCheckedVector.push_back(AnswerByChecked(uncheckedVector[i], false));
	}

	return devidedSheetByCheckedVector;
}

//AnswerByChecked Vector을 정렬하기 위한 새로운 비교 매서드
bool compareAnswer(AnswerByChecked a, AnswerByChecked b) {

	//오차범위 포함한 정렬
	if (a.getAnswerPoint().y < b.getAnswerPoint().y - templateImgCols)
		return true;
	if (a.getAnswerPoint().y > b.getAnswerPoint().y + templateImgCols)
		return false;
	if (a.getAnswerPoint().x < b.getAnswerPoint().x - templateImgRows)
		return true;
	if (a.getAnswerPoint().x > b.getAnswerPoint().x + templateImgRows)
		return false;
	return false;
}

bool compareAnswer2(Point a, Point b) {
	//오차범위 포함한 정렬
	if (a.y < b.y - templateImgCols)
		return true;
	if (a.y > b.y + templateImgCols)
		return false;
	if (a.x < b.x - templateImgRows)
		return true;
	if (a.x > b.x + templateImgRows)
		return false;
	return false;
}

// 이를 Y, X기준으로 정렬하여 순차적으로 나열
vector<AnswerByChecked> sortAnswerByPoint(vector<AnswerByChecked> answers, int inputTemplateImgW, int inputTemplateImgH) {
	templateImgCols = inputTemplateImgW;
	templateImgRows = inputTemplateImgH;

	sort(answers.begin(), answers.end(), compareAnswer);
	return answers;
}