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
	//Mat crop = get_cropped();
	
	waitKey();
}
