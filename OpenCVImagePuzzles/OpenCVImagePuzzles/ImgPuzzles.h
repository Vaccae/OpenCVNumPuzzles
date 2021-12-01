#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

struct CutMat {
public:
	//ͼ��Ӧ���λ��
	int position;
	//ͼ������
	cv::Mat mat;
	//ͼ���ŵ�ǰλ��
	int curposition;


	CutMat(int _pos, cv::Mat _mat) : position(_pos), mat(_mat), curposition(_pos)
	{
	}
};

class ImgPuzzles
{
public:
	//�����ͼ��
	static cv::Mat PuzzleMat;
	static int SleepTime;
	static bool IsShowStep;
	//���㻹ԭʱ��
	static double PuzzleUseTime;
	//��ǰ״̬ 0-δ��ʼ  1-ƴͼ��  2-�����
	static int Status;
	//��ǰͼ������
	static std::vector<std::vector<CutMat*>> vtsCutMat;
	//��ǰͼ����������
	static std::vector<std::vector<cv::Point>> vtsContours;

	//���ɷָ���ͼ������
	static void SplitMats(cv::Mat& img, int cols = 3, int rows = 3);

	//����ƴͼ��Ϸͼ��
	static void CreatePuzzleMat();

	//��ʾ�ƶ����ƴͼ��Ϸͼ��
	static void DrawPuzzleMat(int& curposition, int& newposition);
private:
	static CutMat* finalCutMat;
	//�ָ�ͼ��Ŀ��
	static int rectwidth;
	//�ָ�ͼ��ĸ߶�
	static int rectheight;

	//�����к������ɶ�Ӧ��λ���������
	static std::vector<int> GetVtsPos(int cols = 3, int rows = 3);
	//��ȡ��ǰ������
	static int GetRandNum(std::vector<int>& nums);
	//��������
	static void InsertContours(cv::Rect rect);


	//������¼�
	static void OnMouseEvent(int event, int x, int y, int flags, void* param);
	//��ȡ��ǰ����������¼�
	static bool ImageMove(int &row, int &col, int &curposition, int &newposition);
	//��ȡ�����λ��
	static void GetMousePostion(cv::Point pt, int& row, int& col, int &curposition);
};

