#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"


int templateImgCols; //오차 범위 계산을 위한 template image의 가로 길이
int templateImgRows; //오차 범위 계산을 위한 template image의 세로 길이

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