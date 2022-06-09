#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"


int templateImgCols; //���� ���� ����� ���� template image�� ���� ����
int templateImgRows; //���� ���� ����� ���� template image�� ���� ����
vector<Point> templateMatch(Mat img, Mat tImg, double threshold);
Mat checkAnswerBySort(Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop �̹��� ���μ��� ����
	int op = 0;
	if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5��	1��
	else if (ratio < 0.8) op = 1;						// 1~10��	1��
	else if ((ratio > 1.0) && (ratio < 1.5)) op = 2;	// 1~20��	2��
	else if ((ratio > 1.5) && (ratio < 2.0)) op = 3;	// 1~30��	3��
	else op = 4;										// 1~40��	4��

	Mat tempMarking = imread("resource/templateImage/template0.png");
	Mat tempCircle = imread("resource/templateImage/template2.png");
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5���� ��
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
			cout << i * 10 + j + 1 << "��: " << userAnswer[i][j] << endl;
		}
	}
	
	
	Mat result;


	return result;
}


// ����ڰ� �Է��� ��� �Է����� ���� ���� checked�� �����Ͽ� AnswerBychecked Vector�� ����
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

//AnswerByChecked Vector�� �����ϱ� ���� ���ο� �� �ż���
bool compareAnswer(AnswerByChecked a, AnswerByChecked b) {

	//�������� ������ ����
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

// �̸� Y, X�������� �����Ͽ� ���������� ����
vector<AnswerByChecked> sortAnswerByPoint(vector<AnswerByChecked> answers, int inputTemplateImgW, int inputTemplateImgH) {
	templateImgCols = inputTemplateImgW;
	templateImgRows = inputTemplateImgH;

	sort(answers.begin(), answers.end(), compareAnswer);
	return answers;
}