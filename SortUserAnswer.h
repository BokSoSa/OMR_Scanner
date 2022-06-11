#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;
class AnswerByChecked
{
private:
	Point devidedSheetPoint;
	bool checked;

public:

	AnswerByChecked(Point AnswerByChecked, bool checked) {
		this->devidedSheetPoint = AnswerByChecked;
		this->checked = checked;
	}
	bool isChecked() {
		return checked;
	}
	Point getAnswerPoint() {
		return devidedSheetPoint;
	}

};
Mat checkAnswerByNumberTemplate(Mat origin, Mat input, int answer[]);
vector<AnswerByChecked> mergeCheckedAnswer(vector<Point> checkedVector, vector<Point> uncheckedVector);
bool compareAnswer(AnswerByChecked a, AnswerByChecked b);
bool compareAnswer2(Point a, Point b);
vector<AnswerByChecked> sortAnswerByPoint(vector<AnswerByChecked> answers, int inputTemplateImgW, int inputTemplateImgH);