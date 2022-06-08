#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "Color.h"
#include "Filter.h"
#include "ImageAnalysis.h"

using namespace cv;
using namespace std;

void templateMatch(Mat img, Mat tImg, double threshold) {
	imshow("init",img);

	CImageAnalysis ci = CImageAnalysis();
	Mat templateImg = tImg.clone();

	Mat result; 
	double minVal, maxVal;
	Point minLoc, maxLoc;
	int count = 0;
	Mat dstImg = img.clone();

	matchTemplate(dstImg, templateImg, result, TM_SQDIFF_NORMED);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			if (result.at<float>(i, j) < threshold) {
				rectangle(dstImg, Point(j, i), Point(j + templateImg.cols, i + templateImg.rows), Scalar(0, 0, 255), 1);
				count++;
			}
		}
	}
	cout << "minVal: " << minVal << ", minLoc: " << minLoc << '\n';
	cout << "maxVal: " << maxVal << ", maxLoc: " << maxLoc << '\n';
	cout << "Found: " << count << '\n';

	//imshow("result", result);
	imshow("final", dstImg);

	waitKey(0);
}