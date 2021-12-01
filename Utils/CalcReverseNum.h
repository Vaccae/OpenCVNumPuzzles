
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <Windows.h>

using namespace std;

class CalcReverseNum
{
private:
	static vector<int> tmpvts;
	static int merge(vector<int> &vts, int left, int mid, int right);
public :
	//分治方法检测逆序对个数
	static int MergeSort(vector<int> &vts, int left, int right);
	//暴力破解检测逆序对个数
	static int CheckCount(vector<int>& vts);
};

