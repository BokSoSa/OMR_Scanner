#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "Color.h"
#include "Filter.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;

Mat remove_shadow(string imgLocate) {
	CEdge c; CPixel cp; CFilter cf;	CColor cc;
	
	Mat testImage = imread(imgLocate);
	resize(testImage, testImage, Size(testImage.cols / 4, testImage.rows / 4));

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
	testImage = cc.GS_compositeRGB(outputSplits);

	return testImage;
}
