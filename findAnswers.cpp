#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>

#include "transform.h"
using namespace cv;
using namespace std;

void findAnswers(string imgName, int imgW, int imgH, int ansW, int ansH, double rat) {
    // 채점용 답안 정보
    const int NoOfChoice = 5;
    const int NoOfQuestion = 5;
    map<int, int> standardAnswer;
    map<int, int> testerAnswer;
    standardAnswer.insert(make_pair(0, 1)); //Question 1: answer: 2
    standardAnswer.insert(make_pair(1, 3)); //Question 2: answer: 4
    standardAnswer.insert(make_pair(2, 2)); //Question 3: answer: 3
    standardAnswer.insert(make_pair(3, 2)); //Question 4: answer: 3
    standardAnswer.insert(make_pair(4, 0)); //Question 5: answer: 1

    // 이미지 불러오고 엣지 찾기 (Canny)
    Mat image, gray, blurred, edge;

    image = imread(imgName, IMREAD_COLOR);
    resize(image, image, Size(imgW, imgH));

    if (image.empty()) {
        cout << "Failed to read file\n";
        exit(1);
    }
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(5, 5), 0);
    Canny(gray, edge, 75, 200);
    //imshow("blurred", blurred);
    //imshow("Canny edge", edge);

    // 이미지 이진화, 경계선 벡터에 저장
    Mat thresh;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    threshold(gray, thresh, 150, 255, THRESH_BINARY_INV | THRESH_OTSU);
    //imshow("thresh", thresh);

    findContours(thresh, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
    cout << "이미지 불러오기 : " << imgName << '\n';
    cout << "이미지 이진화" << '\n';

    // 답안 영역 추출
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<vector<Point>> questionCnt;

    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 0.1, true);
        int w = boundingRect(Mat(contours[i])).width;
        int h = boundingRect(Mat(contours[i])).height;
        double ar = (double)w / h;

        if (hierarchy[i][3] == -1)
            if ((w >= ansW && w <= ansW * 2) && (h >= ansH && h <= ansH * 2) && (ar <= rat + 0.2 && ar >= rat - 0.1)) {
                questionCnt.push_back(contours_poly[i]);
                //cout << "-> (" << w << ", " << h << ", " << ar << ")\n";
            }
            //else if ((w >= ansW-2 && w <= ansW * 2) && (h >= ansH-2 && h <= ansH * 2) && (ar <= rat + 0.2 && ar >= rat - 0.2)) {
            //    std::cout << "(" << w << ", " << h << ", " << ar << ")\n";
            //}
    }
    cout << "답안영역 추출" << '\n';

    // 답안 정렬
    sort_contour(questionCnt, 0, (int)questionCnt.size(), string("top-to-bottom"));

    for (int i = 0; i < questionCnt.size(); i = i + NoOfChoice)
    {
        sort_contour(questionCnt, i, i + 5, string("left-to-right"));
    }
    cout << "답안 정렬" << '\n';

    // 사용자가 체크한 답을 출력
    for (int i = 0; i < questionCnt.size();)
    {   
        int maxPixel = 0;
        int answerKey = 0;
        for (int j = 0; j < NoOfChoice; ++i, ++j)
        {
            Mat mask = Mat::zeros(thresh.size(), CV_8U);
            drawContours(mask, questionCnt, i, 255, FILLED, 8, hierarchy, 0, Point());
            bitwise_and(mask, thresh, mask);
            if (countNonZero(mask) > maxPixel)
            {
                maxPixel = countNonZero(mask);
                answerKey = j;
            }
        }
        testerAnswer.insert(make_pair(i / NoOfChoice - 1, answerKey));
        cout << "Question: " << i / NoOfChoice << " Tester's Answer: " << answerKey + 1 << "\n";
    }
    cout << "답 출력 완료" << '\n';
}

int main() 
{
	// string imgName, int imgW, int imgH, int ansW, int ansH, double rat
	findAnswers("t1.jpg", 330, 350, 11, 22, 0.5);
	findAnswers("test05.jpg", 330, 350, 11, 22, 0.5);
	//findAnswers("t3.jpg", 330, 350, 11, 22, 0.5);
	//findAnswers("t4.jpg", 330, 350, 11, 22, 0.5);
}