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
	cout << "Found: " << count << '\n';

	//imshow("result", result);

	return templateMatchedPoints;
}

vector<Point> templateMatch(Mat img, vector<Mat> tImg, double threshold) {
	//imshow("template", tImg);

	CImageAnalysis ci = CImageAnalysis();
	

	Mat result;
	double minVal, maxVal;
	Point minLoc, maxLoc;
	int count = 0;

	Mat dstImg = img.clone();

	vector<Point> templateMatchedPoints;

	
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
	for (int h = 0; h < tImg.size(); h++) {
		Mat templateImg = tImg[h].clone();
		matchTemplate(dstImg, templateImg, result, TM_SQDIFF_NORMED);

		for (int i = 0; i < result.rows; i++) {
			for (int j = 0; j < result.cols; j++) {
				if (result.at<float>(i, j) < threshold) {
					// 중복 체크
					bool isOverlapped = false;

					for (int k = 0; k < templateMatchedPoints.size(); k++) {
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
						//rectangle(dstImg, Point(j, i), Point(j + templateImg.cols, i + templateImg.rows), Scalar(0, 0, 255), 1);
						count++;
					}
				}
			}
		}
	}
	cout << "Found: " << count << '\n';

	//imshow("result", result);

	return templateMatchedPoints;
}