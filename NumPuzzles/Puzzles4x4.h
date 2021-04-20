#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <Windows.h>
#include "CalcReverseNum.h"

using namespace std;

class Puzzles4x4
{
public:
	static vector<int> RandVectorNum();
	static void CreateNewGame(vector<vector<int>>& vts, vector<int> tmpvts);
	static void Printvectors(vector<vector<int>>& vts);
	//�����ƶ�
	static bool VectorsMove(vector<vector<int>>& vts, int col, int row);

	//����Ƿ����
	static bool CheckFinished(vector<vector<int>>& vts);
};

