#include <opencv2/opencv.hpp>
#include "Pixel.h"
#include "Edge.h"
#include "Color.h"
#include "Filter.h"
#include "ImageAnalysis.h"
#include <vector>
using namespace cv;
using namespace std;

vector<Point> templateMatch(Mat img, Mat tImg, double threshold) {
	imshow("init",img);

	CImageAnalysis ci = CImageAnalysis();
	Mat templateImg = tImg.clone();

	Mat result; 
	double minVal, maxVal;
	Point minLoc, maxLoc;
	int count = 0;
	Mat dstImg = img.clone();

	vector<Point> templateMatchedPoints;
	

	matchTemplate(dstImg, templateImg, result, TM_SQDIFF_NORMED);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			if (result.at<float>(i, j) < threshold) {
				// 중복 체크
				bool isOverlapped = false;

				for(int k=0; k< templateMatchedPoints.size(); k++){
					if (j >= templateMatchedPoints[k].x - templateImg.cols
						&& j <= templateMatchedPoints[k].x + templateImg.cols
						&& i >= templateMatchedPoints[k].y - templateImg.rows
						&& i <= templateMatchedPoints[k].y + templateImg.rows) {
						isOverlapped = true;
						break;
					}
				}
				if (!isOverlapped) {
					templateMatchedPoints.push_back(Point(j, i));
					rectangle(dstImg, Point(j, i), Point(j + templateImg.cols, i + templateImg.rows), Scalar(0, 0, 255), 1);
					count++;
				}
			}
		}
	}
	cout << "minVal: " << minVal << ", minLoc: " << minLoc << '\n';
	cout << "maxVal: " << maxVal << ", maxLoc: " << maxLoc << '\n';
	cout << "Found: " << count << '\n';

	//imshow("result", result);
	imshow("final", dstImg);

	waitKey(0);
	return templateMatchedPoints;
}