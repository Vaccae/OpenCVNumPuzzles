#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "..\..\Utils\CalcPathPlan.h"
#include "..\..\Utils\CalcReverseNum.h"

using namespace std;
using namespace cv;

//���ݵ��ƶ�����
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
	Num10 = 10,
	Num11 = 11,
	Num12 = 12,
	Num13 = 13,
	Num14 = 14,
	Num15 = 15,
	Step9and10 = 51,
	Step11and12 = 53,
	Step13and15 = 54,
	StepFinal = 59,
	NONE = 99
};

static vector<pair<int, int>> RestorePath;
//��ԭ�Ĳ���
static vector<pair<pair<int, int>, pair<int, int>>> VetRestoreSteps;

class Puzzles4x4
{
private:
	//��⵱ǰҪ��ԭ�Ĳ���
	//��һ������Ϊ��ǰ���֣��ڶ����ǿ��ƶ��ĵ�ͼ��
	//����ֵΪ��ǰ�Ĳ���
	static int CheckSteps(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//��ȡ���ֶ�Ӧλ��
	static pair<int, int> GetPos(vector<vector<int>>& vts, int num);
	//�������������λ��
	//���ز����ǵڶ��������һ����Ĳ�ֵ
	static pair<int, int> CheckNumPos(vector<vector<int>>& vts, int num1, int num2);
	//�����ж�·��
	static vector<pair<int, int>> FindPath(vector<vector<int>>& sites, pair<int, int>& startpos, pair<int, int>& endpos, int directfirst = DirectFirst::None);
	//��������λ��
	static void SwapPos(vector<vector<int>>& vts, pair<int, int>& firstPos, pair<int, int>& secondPos);
	//���ⲽ�账��
	static void DealStep(vector<vector<int>>& vts, vector<vector<int>>& sites, int step);
	//��һ�ڶ������⴦��
	static void DealTopTwoRows(vector<vector<int>>& vts,vector<vector<int>> &sites,  int row);
	//����9��10�����ⲽ��
	static void DealNineTen(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//����11��12�����ⲽ��
	static void DealElevenTwelve(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//����13��15�����ⲽ��
	static void DealGTThirteen(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//�����ɴ���
	static void DealFinal(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//�쳣����������������9��10 �� 11��12��ɲ��˵����
	static void DealError(vector<vector<int>>& vts, vector<vector<int>>& sites, int num);
	//����0���ƶ�����
	static int ZeroMove(vector<vector<int>>& vts, vector<vector<int>>& sites, pair<int, int>& endPos, int MoveDirect = DirectFirst::Up);;
	//��ʾ��ǰ����
	static void DrawPuzzles(vector<vector<int>>& nummatrix, vector<pair<int, int>> movepath = RestorePath);
public:
	static int SleepTime;
	static bool IsShowStep;

	static vector<int> RandVectorNum();
	static void CreateNewGame(vector<vector<int>>& vts, vector<int> tmpvts);
	static void Printvectors(vector<vector<int>>& vts);
	//�����ƶ�
	static bool VectorsMove(vector<vector<int>>& vts, int col, int row);

	//����Ƿ����
	static bool CheckFinished(vector<vector<int>>& vts);


	//��ԭ���ݵ���Ϸ
	static void RestoreGame(vector<vector<int>>& vts);
};

