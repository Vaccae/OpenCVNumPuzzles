#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "CalcReverseNum.h"
#include "CalcPathPlan.h"

using namespace std;
using namespace cv;

//华容道移动步骤
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
//还原的步骤
static vector<pair<pair<int, int>, pair<int, int>>> VetRestoreSteps;

class Puzzles4x4
{
private:
	//检测当前要还原的步骤
	//第一个参数为当前布局，第二个是可移动的地图，
	//返回值为当前的步骤
	static int CheckSteps(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//获取数字对应位置
	static pair<int, int> GetPos(vector<vector<int>>& vts, int num);
	//检测两个数方向及位置
	//返回参数是第二个点减第一个点的差值
	static pair<int, int> CheckNumPos(vector<vector<int>>& vts, int num1, int num2);
	//查找行动路径
	static vector<pair<int, int>> FindPath(vector<vector<int>>& sites, pair<int, int>& startpos, pair<int, int>& endpos, int directfirst = DirectFirst::None);
	//交换两点位置
	static void SwapPos(vector<vector<int>>& vts, pair<int, int>& firstPos, pair<int, int>& secondPos);
	//特殊步骤处理
	static void DealStep(vector<vector<int>>& vts, vector<vector<int>>& sites, int step);
	//第一第二行特殊处理
	static void DealTopTwoRows(vector<vector<int>>& vts,vector<vector<int>> &sites,  int row);
	//处理9和10的特殊步骤
	static void DealNineTen(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//处理11和12的特殊步骤
	static void DealElevenTwelve(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//处理13到15的特殊步骤
	static void DealGTThirteen(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//最后完成处理
	static void DealFinal(vector<vector<int>>& vts, vector<vector<int>>& sites);
	//异常处理，用于三四行中9和10 及 11和12完成不了的情况
	static void DealError(vector<vector<int>>& vts, vector<vector<int>>& sites, int num);
	//数字0的移动函数
	static int ZeroMove(vector<vector<int>>& vts, vector<vector<int>>& sites, pair<int, int>& endPos, int MoveDirect = DirectFirst::Up);;
	//显示当前操作
	static void DrawPuzzles(vector<vector<int>>& nummatrix, vector<pair<int, int>> movepath = RestorePath);
public:
	static int SleepTime;
	static bool IsShowStep;

	static vector<int> RandVectorNum();
	static void CreateNewGame(vector<vector<int>>& vts, vector<int> tmpvts);
	static void Printvectors(vector<vector<int>>& vts);
	//方格移动
	static bool VectorsMove(vector<vector<int>>& vts, int col, int row);

	//检测是否完成
	static bool CheckFinished(vector<vector<int>>& vts);


	//还原华容道游戏
	static void RestoreGame(vector<vector<int>>& vts);
};

