#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "SortUserAnswer.h"


int templateImgCols; //���� ���� ����� ���� template image�� ���� ����
int templateImgRows; //���� ���� ����� ���� template image�� ���� ����

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