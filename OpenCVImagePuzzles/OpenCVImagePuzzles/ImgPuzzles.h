#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

struct CutMat {
public:
	//图像应存放位置
	int position;
	//图像数据
	cv::Mat mat;
	//图像存放当前位置
	int curposition;


	CutMat(int _pos, cv::Mat _mat) : position(_pos), mat(_mat), curposition(_pos)
	{
	}
};

class ImgPuzzles
{
public:
	//输出的图像
	static cv::Mat PuzzleMat;
	static int SleepTime;
	static bool IsShowStep;
	//计算还原时间
	static double PuzzleUseTime;
	//当前状态 0-未开始  1-拼图中  2-已完成
	static int Status;
	//当前图像容器
	static std::vector<std::vector<CutMat*>> vtsCutMat;
	//当前图像轮廓容器
	static std::vector<std::vector<cv::Point>> vtsContours;

	//生成分割后的图像容器
	static void SplitMats(cv::Mat& img, int cols = 3, int rows = 3);

	//生成拼图游戏图像
	static void CreatePuzzleMat();

	//显示移动后的拼图游戏图像
	static void DrawPuzzleMat(int& curposition, int& newposition);
private:
	static CutMat* finalCutMat;
	//分割图像的宽度
	static int rectwidth;
	//分割图像的高度
	static int rectheight;

	//根据行和列生成对应的位置序号容器
	static std::vector<int> GetVtsPos(int cols = 3, int rows = 3);
	//获取当前随机序号
	static int GetRandNum(std::vector<int>& nums);
	//插入轮廓
	static void InsertContours(cv::Rect rect);


	//鼠标点击事件
	static void OnMouseEvent(int event, int x, int y, int flags, void* param);
	//获取当前鼠标左键点击事件
	static bool ImageMove(int &row, int &col, int &curposition, int &newposition);
	//获取鼠标点击位置
	static void GetMousePostion(cv::Point pt, int& row, int& col, int &curposition);
};

