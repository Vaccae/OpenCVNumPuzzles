#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "..\..\Utils\CalcReverseNum.h"
#include "..\..\Utils\CalcPathPlan.h"

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


//ƴͼ�ƶ�����
enum RestoreStep {
	Num1 = 1,
	Num2 = 2,
	Num3 = 3,
	Num4 = 4,
	Num5 = 5,
	Num6 = 6,
	Num7 = 7,
	Num8 = 8,
	Num9 = 9,
	Step3 = 50,
	Step4and5 = 51,
	StepFinal = 59,
	NONE = 99
};

static vector<pair<int, int>> RestorePath;

class ImgPuzzles
{
public:
	//�����ͼ��
	static cv::Mat TmpSrc;
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


	//AI�Զ���ԭ
	static void RestoreGame();
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
	static bool ImageMove(int& row, int& col, int& curposition, int& newposition);
	//��ȡ�����λ��
	static void GetMousePostion(cv::Point pt, int& row, int& col, int& curposition);

	//��⵱ǰҪ��ԭ�Ĳ���
	//��һ������Ϊ��ǰ���֣��ڶ����ǿ��ƶ��ĵ�ͼ��
	//����ֵΪ��ǰ�Ĳ���
	static int CheckSteps(std::vector<std::vector<CutMat*>>& vts, std::vector<std::vector<int>>& sites);

	//����Ƿ����
	static bool CheckFinished(std::vector<std::vector<CutMat*>>& vts);

	//��ȡ���ֶ�Ӧλ��
	static void GetPos(std::vector<std::vector<CutMat*>>& vts, int num, std::pair<int, int> &startpos, std::pair<int, int>& endpos);

	//��ȡ�հ׸�λ��
	static  std::pair<int, int> GetNullPos(std::vector<std::vector<CutMat*>>& vts);

	//�հ׸��ƶ�
	static int NullMove(vector<vector<int>>& sites, pair<int, int>& endPos, int MoveDirect = DirectFirst::Up);

	//���ⲽ�账��
	static void DealStep(vector<vector<int>>& sites, int step);

	//�����ж�·��
	static std::vector<std::pair<int, int>> FindPath(std::vector<std::vector<int>>& sites, std::pair<int, int>& startpos, 
		std::pair<int, int>& endpos, int directfirst = DirectFirst::None);
};

