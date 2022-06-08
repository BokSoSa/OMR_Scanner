#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;

Mat remove_shadow(Mat img);
void mouseClick(Mat img);
//Mat get_cropped();
void templateMatch(Mat img, Mat tImg, double threshold);

int main() {
	Mat omrImg = imread("resource/testimage/omr2_marking1.jpg");
	resize(omrImg, omrImg, Size(omrImg.cols / 4, omrImg.rows / 4));
	omrImg = remove_shadow(omrImg);

	mouseClick(omrImg);
	/*
	Mat t = imread("resource/testimage/t7.jpg");
	Mat temp = imread("resource/templateImage/11.png");
	int r = 2;
	resize(t, t, Size(141.7*r*2, 274.6*r)); // t.cols = 141.7, t.rows = 274.6
	resize(temp, temp, Size(10*r, 14*r)); // temp.cols = 10, temp.rows = 14
	templateMatch(t, temp, 0.055);
	*/
	//Mat crop = get_cropped();
	
	waitKey();
}
