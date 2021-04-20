
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
	//���η����������Ը���
	static int MergeSort(vector<int> &vts, int left, int right);
	//�����ƽ�������Ը���
	static int CheckCount(vector<int>& vts);
};

