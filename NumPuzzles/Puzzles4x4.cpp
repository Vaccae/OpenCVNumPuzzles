#include "Puzzles4x4.h"

vector<int> Puzzles4x4::RandVectorNum()
{
	vector<int> vts;
	//标记0的位置
	int zeroidx = -1;
	//定义华容道数字
	vector<int> vtsnums{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	int size = vtsnums.size();
	for (int i = 0; i < size; ++i) {
		//初始化随机数种子
		srand((int)time(0));
		int index = vtsnums.size() == 1 ? 0 : rand() % (vtsnums.size() - 1);
		vts.push_back(vtsnums[index]);
		//标记0所在的位置
		if (vts[vts.size() - 1] == 0) zeroidx = vts.size() - 1;
		//容器中删除已经赋值的数字
		vtsnums.erase(vtsnums.begin() + index);
	}

	//计算逆序数，防止无解
	vector<int> tmpvts = vts;
	int count = CalcReverseNum::MergeSort(tmpvts, 0, vts.size() - 1);
	cout << "分治计算:" << count << endl;

	//获取0所在的行数和列数
	int zerorow = zeroidx / 4;
	int zerocol = zeroidx % 4;

	//得到的逆序对加上0所在的位置判断是奇数还是偶数
	int totalcount = count + zerorow + zerocol;
	if ((totalcount % 2) == 0) {
		//如果是偶数说明无解，把最后两位换一下序
		int tmpnum = vts[vts.size() - 1];
		vts[vts.size() - 1] = vts[vts.size() - 2];
		vts[vts.size() - 2] = tmpnum;
	}

	return vts;
}

void Puzzles4x4::CreateNewGame(vector<vector<int>>& vts, vector<int> tmpvts)
{
	for (int i = 0; i < vts.size(); ++i) {
		for (int j = 0; j < vts[i].size(); ++j) {
			int index = vts.size() * i + j;
			vts[i][j] = tmpvts[index];
		}
	}
}

void Puzzles4x4::Printvectors(vector<vector<int>>& vts)
{
	for (int i = 0; i < vts.size(); ++i) {
		for (int j = 0; j < vts[i].size(); ++j) {

			string str = vts[i][j] == 0 ? "" : to_string(vts[i][j]);
			cout << setw(2) << str << "  ";
		}
		cout << endl;
	}
}

bool Puzzles4x4::VectorsMove(vector<vector<int>>& vts, int col, int row)
{
	bool res = true;
	//计算可移动的区域
	//1.左边
	if (col-1>=0 && vts[row][col-1]==0){
		vts[row][col - 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//2.右边
    else if (col + 1 <= 3 && vts[row][col + 1] == 0) {
		vts[row][col + 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//3.上边
    else if (row + 1 <= 3 && vts[row+1][col] == 0) {
		vts[row+1][col] = vts[row][col];
		vts[row][col] = 0;
	}
	//4.下边
	else if (row - 1 >= 0 && vts[row - 1][col] == 0) {
		vts[row - 1][col] = vts[row][col];
		vts[row][col] = 0;
	}
	else {
		res = false;
	}
	return res;
}


bool Puzzles4x4::CheckFinished(vector<vector<int>>& vts)
{
	//计算总行数
	int rows = vts.size() - 1;
	//计算总列数
	int cols = vts[rows].size() - 1;
	//先判断最后一位是否是0，如果不是下面就不再浪费时间检查
	if (vts[rows][cols] != 0) return false;;
	int checknum = 1;
	for (int row = 0; row <= rows; ++row) {
		for (int col = 0; col <= cols; ++col) {
			//最后一格已经检测，直接退出
			if (col == cols && row == rows) return true;
			if (vts[row][col] != checknum) return false;
			checknum++;
		}
	}
}

