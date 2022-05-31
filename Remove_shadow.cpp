#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "Color.h"
#include "Filter.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;
void remove_shadow() {

	Mat testImage = imread("resource/testimage/omr1_marking.jpg");

	resize(testImage, testImage, Size(testImage.cols / 4, testImage.rows / 4));
	CEdge c; CPixel cp; CFilter cf;
	imshow("image1", testImage);
	Mat splits[3];
	Mat outputSplits[3];
	Mat dilatedImage;
	split(testImage, splits);
	for (int i = 0; i < 3; i++) {
		dilate(splits[i], outputSplits[i], getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
		medianBlur(outputSplits[i], outputSplits[i], 21);
		absdiff(splits[i], outputSplits[i], outputSplits[i]);

		normalize(outputSplits[i], outputSplits[i], 0, 255, NORM_MINMAX);

	}
	CColor cc;
	testImage = cc.GS_compositeRGB(outputSplits);
	//cvtColor(testImage, testImage, COLOR_BGR2GRAY);
	//testImage += 50;
	//imshow("image3", testImage);

	//testImage = cp.GS_threshold(testImage, 50, 255, THRESH_BINARY);

	//testImage = c.GS_canny_edge_Canny(testImage, 150, 230);

	imshow("image4", testImage);

	waitKey();
}
