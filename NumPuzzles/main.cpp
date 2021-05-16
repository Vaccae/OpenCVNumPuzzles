#pragma once 
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "Puzzles4x4.h"

using namespace std;
using namespace cv;

//������������
vector<vector<Point>> contours;
vector<vector<int>> nummatrix(4, vector<int>(4, 0));
Mat src;

//���㻹ԭʱ��
bool isFinish = true;
double usetime;

//��ʼ�����ֻ��ݵ�
void DrawPuzzlesMat(Mat& src, vector<vector<int>>& nummatrix, vector<vector<Point>>& contours, bool iscontours=false);
//��������
void InsertContours(vector<vector<Point>>& contours, Rect rect);
//�����Ӧ�¼�
void onMouse(int event, int x, int y, int flags, void* param);
//������������ж϶�Ӧ�Ķ�ά�����λ��
void SelectVectorItem(Point pt, vector<vector<Point>>& contours, vector<vector<int>>& nummatrix, int& col, int& row);


int main(int argc, char** argv) {

	//���ɻ��ݵ�
	vector<int> tmpvets = Puzzles4x4::RandVectorNum();
	Puzzles4x4::CreateNewGame(nummatrix, tmpvets);
	//��ӡ���ɵĻ��ݵ�
	Puzzles4x4::Printvectors(nummatrix);
	//����ͼ����ʾ
	DrawPuzzlesMat(src, nummatrix, contours, true);

	cout << contours.size() << endl;

	waitKey(0);
	return 0;
}


void DrawPuzzlesMat(Mat& src, vector<vector<int>>& nummatrix, vector<vector<Point>>& contours, bool iscontours)
{
	
	int starttop = 50;
	int startleft = 50;
	int rectlen = 125;
	//�����׵�ͼ��
	src = Mat(Size(600, 600), CV_8UC3, Scalar(255, 255, 255));
	//�������������̱���
	Rect bgrect = Rect(startleft, starttop, rectlen * 4, rectlen * 4);
	rectangle(src, bgrect, Scalar(208, 208, 225), -1);

	for (int i = 0; i < 16; ++i) {
		//���㵱ǰ˳��Ϊ��ά����ļ��м���
		int row = i / nummatrix.size();
		int col = i % nummatrix.size();
		//���ɶ�Ӧ�ľ��ο�
		Rect rect = Rect(startleft + col * rectlen, starttop + row * rectlen, rectlen, rectlen);
		rectangle(src, rect, Scalar::all(0));
		//�����εĵ�д�뵽��������
		if(iscontours) InsertContours(contours, rect);

		//д�϶�Ӧ����
		if (nummatrix[row][col] == 0) continue;
		string text = to_string(nummatrix[row][col]);
		putText(src, text, Point(rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 + 10),
			FONT_HERSHEY_DUPLEX, 1.0, Scalar::all(0));
	}
	imshow("src", src);
	//���ص��¼�
	setMouseCallback("src", onMouse, 0);
}

void InsertContours(vector<vector<Point>>& contours, Rect rect)
{
	vector<Point> vetpt;
	Point pt1 = Point(rect.x, rect.y);
	vetpt.push_back(pt1);
	Point pt2 = Point(rect.x + rect.width, rect.y);
	vetpt.push_back(pt2);
	Point pt3 = Point(rect.x + rect.width, rect.y + rect.height);
	vetpt.push_back(pt3);
	Point pt4 = Point(rect.x, rect.y + rect.height);
	vetpt.push_back(pt4);

	contours.push_back(vetpt);
}

void onMouse(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case EVENT_LBUTTONUP:
	{
		Point point = Point(x, y);
		int col = -1;
		int row = -1;
		SelectVectorItem(point, contours, nummatrix, col, row);
		if (col >= 0 && row >= 0) {
			if (Puzzles4x4::VectorsMove(nummatrix, col, row)) {
				if (isFinish) {
					isFinish = false;
					usetime = (double)getTickCount();
					cout << "��ʼ��ԭ��ʱ" << endl;
				}
				//��������ͼ����ʾ
				DrawPuzzlesMat(src, nummatrix, contours);

				isFinish = Puzzles4x4::CheckFinished(nummatrix);
				if (isFinish) {
					usetime = ((double)getTickCount() - usetime) / getTickFrequency();
					cout << "��ԭ��ɣ���ʱ��" << usetime << "�룡" << endl;
				}
			}
		}

	}
	break;
	case EVENT_MBUTTONUP:
	{
		//�Զ���ԭ
		Puzzles4x4::IsShowStep = false;
		Puzzles4x4::RestoreGame(nummatrix);
	}
	break;
	case EVENT_RBUTTONDBLCLK:
	{
		//���ɻ��ݵ�
		vector<int> tmpvets = Puzzles4x4::RandVectorNum();
		Puzzles4x4::CreateNewGame(nummatrix, tmpvets);
		//��ӡ���ɵĻ��ݵ�
		Puzzles4x4::Printvectors(nummatrix);
		//����ͼ����ʾ
		DrawPuzzlesMat(src, nummatrix, contours, true);

		//�ı��ʼ״̬
		isFinish = true;
	}
	break;
	default:
		break;
	}
}

void SelectVectorItem(Point pt, vector<vector<Point>>& contours, vector<vector<int>>& nummatrix, int& col, int& row)
{
	for (int i = 0; i < contours.size(); ++i) {
		//������������ڣ���������һ������
		if (pointPolygonTest(contours[i], pt, true)>0) {
			//���㵱ǰ˳��Ϊ��ά����ļ��м���
			row = i / nummatrix.size();
			col = i % nummatrix.size();
			break;
		}
	}
}
