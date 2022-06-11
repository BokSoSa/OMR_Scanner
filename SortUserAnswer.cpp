#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"


int templateImgCols; //���� ���� ����� ���� template image�� ���� ����
int templateImgRows; //���� ���� ����� ���� template image�� ���� ����
vector<Point> templateMatch(Mat img, Mat tImg, double threshold);
vector<Point> templateMatch(Mat img, vector<Mat> tImg, double threshold);


Mat checkAnswerByNumberTemplate(Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop �̹��� ���μ��� ����
	int op = 0;
	if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5��	1��
	else if (ratio < 0.8) op = 1;						// 1~10��	1��
	else if ((ratio > 1.0) && (ratio < 1.5)) op = 2;	// 1~20��	2��
	else if ((ratio > 1.5) && (ratio < 2.0)) op = 3;	// 1~30��	3��
	else op = 4;										// 1~40��	4��

	Mat tempMarking = imread("resource/templateImage/final0.png"); //üũ�� �̹���
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5���� ��
		resize(tempMarking, tempMarking, Size(input.rows * 0.103, input.rows * 0.103));
	}
	else {
		resize(tempMarking, tempMarking, Size(input.rows * 0.059, input.rows * 0.059));
	}
	// üũ�� �� ��ǥ��
	vector<Point> checkedAnswer = templateMatch(input, tempMarking, 0.3);
	
	Mat removeAnswer; //üũ�� �� ���� �̹��� ������ ��ü
	removeAnswer = input.clone();
	for (int i = 0; i < checkedAnswer.size(); i++) {
		rectangle(removeAnswer, Point(checkedAnswer[i].x, checkedAnswer[i].y), Point(checkedAnswer[i].x + templateImgCols, checkedAnswer[i].y + templateImgRows), Scalar(0, 0, 0), FILLED);
	}

	vector<Mat> number;
	for (int i = 0; i < 40; i++) {
		string img_path = format("resource/number/%d.png", i + 1);
		number.push_back(imread(img_path));
		resize(number.back(), number.back(), Size(input.rows * 0.075, input.rows * 0.075));
	}

	vector<Point> question = templateMatch(removeAnswer, number, 0.5);

	imshow("removeAns", removeAnswer);
	return removeAnswer;
}



Mat checkAnswerBySort(Mat input, int answer[]) {
	double ratio = double(input.cols) / double(input.rows); // crop �̹��� ���μ��� ����
	int op = 0;
	if ((ratio > 0.8) && (ratio < 1)) op = 1;			// 1~5��	1��
	else if (ratio < 0.8) op = 1;						// 1~10��	1��
	else if ((ratio > 1.0) && (ratio < 1.5)) op = 2;	// 1~20��	2��
	else if ((ratio > 1.5) && (ratio < 2.0)) op = 3;	// 1~30��	3��
	else op = 4;										// 1~40��	4��

	Mat tempMarking = imread("resource/templateImage/final0.png");
	Mat tempCircle = imread("resource/templateImage/final4.png");

	
	if ((ratio > 0.8) && (ratio < 1)) { // 1~5���� ��
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
			cout << i * 10 + j + 1 << "��: " << userAnswer[i][j] << endl;
			Point drawTL = userAnswerPoint[i][j];
			if (userAnswer[i][j]==0) continue;

			maxAnswerCount=i*10+j+1;
			if (userAnswer[i][j] == answer[i * 10 + j]) {
				correctAnswerCount++;
				rectangle(result, drawTL, Point(drawTL.x + templateImgCols, drawTL.y + templateImgRows), Scalar(0, 255, 0), 2);
			}
			else {
				rectangle(result, drawTL, Point(drawTL.x + templateImgCols, drawTL.y + templateImgRows), Scalar(0, 0, 255), 2);
			}
		}
	}
	cout << "MAX: " << maxAnswerCount << " correct: "<<correctAnswerCount << endl;
	//ä�� + ��� ǥ��
	//������ 20���ε� input image�� 40������ �ִ� ��� ����ؼ� �ۼ�
	//Ʋ�� ��� ��üũ X Ʋ�� ���θ� üũ
	//���� üũ ���Ѱ��� ��üũX Ʋ������ üũ��X

	double score = ((double)correctAnswerCount / maxAnswerCount * 100);
	putText(result, "SCORE: "+to_string((int)score), Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255, 255, 0), 2);
	cout << "\n**** ä�� ��� ****" << endl;
	cout << "Ǭ ���� ��: " << maxAnswerCount << endl;
	cout << "���� ���� ��: " << correctAnswerCount << endl;
	cout << "����: " << (int)score << "��" << endl;
	
	imshow("rrrresult", result);

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