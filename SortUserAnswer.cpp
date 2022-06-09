#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"


int templateImgCols; //오차 범위 계산을 위한 template image의 가로 길이
int templateImgRows; //오차 범위 계산을 위한 template image의 세로 길이
vector<Point> templateMatch(Mat img, Mat tImg, double threshold);
Mat checkAnswerBySort(Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop 이미지 가로세로 비율
	int op = 0;
	if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5번	1단
	else if (ratio < 0.8) op = 1;						// 1~10번	1단
	else if ((ratio > 1.0) && (ratio < 1.5)) op = 2;	// 1~20번	2단
	else if ((ratio > 1.5) && (ratio < 2.0)) op = 3;	// 1~30번	3단
	else op = 4;										// 1~40번	4단

	Mat tempMarking = imread("resource/templateImage/template0.png");
	Mat tempCircle = imread("resource/templateImage/template2.png");
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5번일 때
		resize(tempMarking, tempMarking, Size(input.rows * 0.049, input.rows * 0.089));
		resize(tempCircle, tempCircle, Size(input.rows * 0.049, input.rows * 0.089));
	}
	else {
		resize(tempMarking, tempMarking, Size(input.rows * 0.028, input.rows * 0.05));
		resize(tempCircle, tempCircle, Size(input.rows * 0.028, input.rows * 0.05));
	}

	vector<Point> checkedAnswer = templateMatch(input, tempMarking, 0.15);
	vector<Point> uncheckedAnswer = templateMatch(input, tempCircle, 0.5);
	//cout << "???: " << checkedAnswer.size() << endl;
	vector<AnswerByChecked> answers = mergeCheckedAnswer(checkedAnswer, uncheckedAnswer);
	answers = sortAnswerByPoint(answers, tempMarking.cols, tempMarking.rows);

	int** userAnswer = new int* [op];
	for (int i = 0; i < op; i++) {
		userAnswer[i] = new int[10];
		for (int j = 0; j < 10; j++) {
			userAnswer[i][j] = 0;
		}
	}

	int checkedNumber = 0;
	int answerX = 0, answerY = 0;
	for (int i = 0; i < answers.size(); i++) {
		checkedNumber++;
		if (answers[i].isChecked() == 1) {
			userAnswer[answerY][answerX] = checkedNumber;
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
	for (int i = 0; i < op; i++) {
		for (int j = 0; j < 10; j++) {
			cout << i * 10 + j + 1 << "번: " << userAnswer[i][j] << endl;
		}
	}
	
	
	Mat result;


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

// 이를 Y, X기준으로 정렬하여 순차적으로 나열
vector<AnswerByChecked> sortAnswerByPoint(vector<AnswerByChecked> answers, int inputTemplateImgW, int inputTemplateImgH) {
	templateImgCols = inputTemplateImgW;
	templateImgRows = inputTemplateImgH;

	sort(answers.begin(), answers.end(), compareAnswer);
	return answers;
}