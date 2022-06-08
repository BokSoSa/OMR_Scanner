#include <opencv2/opencv.hpp>

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
		double ratio = double(crop.cols) / double(crop.rows); // crop �̹��� ���μ��� ����
		cout << "crop.cols / crop.rows: " << ratio << '\n';

		Mat tempMarking = imread("resource/templateImage/template0.png");
		Mat tempCircle = imread("resource/templateImage/template2.png");
		if ((ratio > 0.8) && (ratio < 1)) { // 1~5���� ��
			resize(tempMarking, tempMarking, Size(crop.rows * 0.049, crop.rows * 0.089));
			resize(tempCircle, tempCircle, Size(crop.rows * 0.049, crop.rows * 0.089));
		}
		else {
			resize(tempMarking, tempMarking, Size(crop.rows * 0.028, crop.rows * 0.05));
			resize(tempCircle, tempCircle, Size(crop.rows * 0.028, crop.rows * 0.05));
		}

		//templateMatch(crop, tempMarking, 0.15);
		templateMatch(crop, tempCircle, 0.5);
		return;
	}
}

void mouseClick(Mat img) {
	omrImg = img;
	imshow("result", img);
	setMouseCallback("result", onMouseEvent, (void*)&img);
}

Mat perspective_img(Mat img) {
	// ��ǲ �̹��� ��� ��ǥ �Է�
	Point2f inputQuad[4];
	// ��� �̹��� ��� ��ǥ
	Point2f outputQuad[4];

	// Lambda Matrix
	Mat lambda(2, 4, CV_32FC1);
	//Mat ������ �Է�,��� �̹��� 
	Mat output;
	// Set the lambda matrix the same type and size as input
	lambda = Mat::zeros(img.rows, img.cols, img.type());

	// 4�� ����Ʈ�� �ð�������� �������ش�
	inputQuad[0] = Point2f(xy[0].x, xy[0].y); // ������
	inputQuad[1] = Point2f(xy[1].x, xy[1].y); // ��������
	inputQuad[2] = Point2f(xy[2].x, xy[2].y); // �����ʾƷ�
	inputQuad[3] = Point2f(xy[3].x, xy[3].y); // ���ʾƷ�
	// ���ε� �̹����� �ð�������� ����ش�.
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f((xy[1].x - xy[0].x)*2, 0);
	outputQuad[2] = Point2f((xy[1].x - xy[0].x) * 2, (xy[2].y - xy[1].y) * 2);
	outputQuad[3] = Point2f(0, (xy[2].y - xy[1].y) * 2);

	// �Է�,��� Quad�� ���� getPerspectiveTransform�Լ��� �̿��Ͽ� ��ȯ����� ����
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	// �̹��� ����
	warpPerspective(img, output, lambda, Size((xy[1].x - xy[0].x) * 2, (xy[2].y - xy[1].y) * 2));//�Է�,���,��ȯ���,ũ��

	return output;
}