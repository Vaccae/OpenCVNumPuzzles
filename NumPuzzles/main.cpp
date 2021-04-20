#pragma once 
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "Puzzles4x4.h"

using namespace std;
using namespace cv;

//定义轮廓区域
vector<vector<Point>> contours;
vector<vector<int>> nummatrix(4, vector<int>(4, 0));
Mat src;

//计算还原时间
bool isFinish = true;
double usetime;

//初始化数字华容道
void DrawPuzzlesMat(Mat& src, vector<vector<int>>& nummatrix, vector<vector<Point>>& contours, bool iscontours=false);
//定义轮廓
void InsertContours(vector<vector<Point>>& contours, Rect rect);
//鼠标响应事件
void onMouse(int event, int x, int y, int flags, void* param);
//根据鼠标点击来判断对应的二维数组的位置
void SelectVectorItem(Point pt, vector<vector<Point>>& contours, vector<vector<int>>& nummatrix, int& col, int& row);


int main(int argc, char** argv) {

	//生成华容道
	vector<int> tmpvets = Puzzles4x4::RandVectorNum();
	Puzzles4x4::CreateNewGame(nummatrix, tmpvets);
	//打印生成的华容道
	Puzzles4x4::Printvectors(nummatrix);
	//生成图像显示
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
	//创建白底图像
	src = Mat(Size(600, 600), CV_8UC3, Scalar(255, 255, 255));
	//创建华容着棋盘背景
	Rect bgrect = Rect(startleft, starttop, rectlen * 4, rectlen * 4);
	rectangle(src, bgrect, Scalar(208, 208, 225), -1);

	for (int i = 0; i < 16; ++i) {
		//计算当前顺序为二维数组的几行几列
		int row = i / nummatrix.size();
		int col = i % nummatrix.size();
		//生成对应的矩形框
		Rect rect = Rect(startleft + col * rectlen, starttop + row * rectlen, rectlen, rectlen);
		rectangle(src, rect, Scalar::all(0));
		//将矩形的点写入到轮廓点中
		if(iscontours) InsertContours(contours, rect);

		//写上对应数字
		if (nummatrix[row][col] == 0) continue;
		string text = to_string(nummatrix[row][col]);
		putText(src, text, Point(rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 + 10),
			FONT_HERSHEY_DUPLEX, 1.0, Scalar::all(0));
	}
	imshow("src", src);
	//鼠标回调事件
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
					cout << "开始还原计时" << endl;
				}
				//重新生成图像显示
				DrawPuzzlesMat(src, nummatrix, contours);

				isFinish = Puzzles4x4::CheckFinished(nummatrix);
				if (isFinish) {
					usetime = ((double)getTickCount() - usetime) / getTickFrequency();
					cout << "还原完成！用时：" << usetime << "秒！" << endl;
				}

			}
		}

	}
	break;
	case EVENT_RBUTTONDBLCLK:
	{
		//生成华容道
		vector<int> tmpvets = Puzzles4x4::RandVectorNum();
		Puzzles4x4::CreateNewGame(nummatrix, tmpvets);
		//打印生成的华容道
		Puzzles4x4::Printvectors(nummatrix);
		//生成图像显示
		DrawPuzzlesMat(src, nummatrix, contours, true);

		//改变初始状态
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
		//如果点在轮廓内，计算是哪一个区域
		if (pointPolygonTest(contours[i], pt, true)>0) {
			//计算当前顺序为二维数组的几行几列
			row = i / nummatrix.size();
			col = i % nummatrix.size();
			break;
		}
	}
}
