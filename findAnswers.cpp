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

void findAnswers(Mat img, int ansW, int ansH, double rat) {
    // ä���� ��� ����
    const int NoOfChoice = 5;
    const int NoOfQuestion = 5;
    map<int, int> standardAnswer;
    map<int, int> testerAnswer;
    standardAnswer.insert(make_pair(0, 1)); //Question 1: answer: 2
    standardAnswer.insert(make_pair(1, 3)); //Question 2: answer: 4
    standardAnswer.insert(make_pair(2, 2)); //Question 3: answer: 3
    standardAnswer.insert(make_pair(3, 2)); //Question 4: answer: 3
    standardAnswer.insert(make_pair(4, 0)); //Question 5: answer: 1

    // �̹��� �ҷ�����
    Mat image, gray;
    image = img;

    if (image.empty()) {
        cout << "Failed to read file\n";
        exit(1);
    }
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // �̹��� ����ȭ, ��輱 ���Ϳ� ����
    Mat thresh;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    threshold(gray, thresh, 50, 255, THRESH_BINARY_INV);
    imshow("thresh", thresh);

    findContours(thresh, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
    //cout << "�̹��� �ҷ����� : " << imgName << '\n';
    cout << "�̹��� ����ȭ" << '\n';

    // ��� ���� ����
    vector<vector<Point>> contours_poly(contours.size());
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
                cout << "-> (" << w << ", " << h << ", " << ar << ")\n";
            }
            else if ((w >= ansW-2 && w <= ansW * 2) && (h >= ansH-2 && h <= ansH * 2) && (ar <= rat + 0.2 && ar >= rat - 0.2)) {
                cout << "(" << w << ", " << h << ", " << ar << ")\n";
            }
    }
    cout << "��ȿ��� ����" << '\n';

    // ��� ����
    sort_contour(questionCnt, 0, (int)questionCnt.size(), string("top-to-bottom"));

    for (int i = 0; i < questionCnt.size(); i = i + NoOfChoice)
    {
        sort_contour(questionCnt, i, i + 5, string("left-to-right"));
    }
    cout << "��� ����" << '\n';

    // ����ڰ� üũ�� ���� ���
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
    cout << "�� ��� �Ϸ�" << '\n';
}
