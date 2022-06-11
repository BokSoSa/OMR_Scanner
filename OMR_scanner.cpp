#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;

Mat remove_shadow(Mat img);
void mouseClick(Mat img);
//Mat get_cropped();

int main() {
	//Mat omrImg = imread("resource/testimage/omr2_marking1.jpg");		// �Ϻ�
	//Mat omrImg = imread("resource/testimage/omr2_marking2.jpg");		// �Ϻ�
	//Mat omrImg = imread("resource/testimage/card4.jpg");				// �Ϻ�
	//Mat omrImg = imread("resource/testimage/card7.jpg");				// �Ϻ�
	//Mat omrImg = imread("resource/testimage/card10.jpg");				// �Ϻ�
	//Mat omrImg = imread("resource/testimage/card11.jpg");				// �Ϻ�
	//Mat omrImg = imread("resource/testimage/card12.jpg");				// �Ϻ�, size 1.5��
	//Mat omrImg = imread("resource/testimage/card13.jpg");				// �Ϻ�, size 1.5��
	Mat omrImg = imread("resource/testimage/card10.jpg");				
	resize(omrImg, omrImg, Size(omrImg.cols / 4, omrImg.rows / 4));
	omrImg = remove_shadow(omrImg);

	mouseClick(omrImg);
	//Mat crop = get_cropped();
	
	waitKey();
}
