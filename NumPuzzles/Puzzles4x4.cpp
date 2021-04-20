#include "Puzzles4x4.h"

vector<int> Puzzles4x4::RandVectorNum()
{
	vector<int> vts;
	//���0��λ��
	int zeroidx = -1;
	//���廪�ݵ�����
	vector<int> vtsnums{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	int size = vtsnums.size();
	for (int i = 0; i < size; ++i) {
		//��ʼ�����������
		srand((int)time(0));
		int index = vtsnums.size() == 1 ? 0 : rand() % (vtsnums.size() - 1);
		vts.push_back(vtsnums[index]);
		//���0���ڵ�λ��
		if (vts[vts.size() - 1] == 0) zeroidx = vts.size() - 1;
		//������ɾ���Ѿ���ֵ������
		vtsnums.erase(vtsnums.begin() + index);
	}

	//��������������ֹ�޽�
	vector<int> tmpvts = vts;
	int count = CalcReverseNum::MergeSort(tmpvts, 0, vts.size() - 1);
	cout << "���μ���:" << count << endl;

	//��ȡ0���ڵ�����������
	int zerorow = zeroidx / 4;
	int zerocol = zeroidx % 4;

	//�õ�������Լ���0���ڵ�λ���ж�����������ż��
	int totalcount = count + zerorow + zerocol;
	if ((totalcount % 2) == 0) {
		//�����ż��˵���޽⣬�������λ��һ����
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
	//������ƶ�������
	//1.���
	if (col-1>=0 && vts[row][col-1]==0){
		vts[row][col - 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//2.�ұ�
    else if (col + 1 <= 3 && vts[row][col + 1] == 0) {
		vts[row][col + 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//3.�ϱ�
    else if (row + 1 <= 3 && vts[row+1][col] == 0) {
		vts[row+1][col] = vts[row][col];
		vts[row][col] = 0;
	}
	//4.�±�
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
	//����������
	int rows = vts.size() - 1;
	//����������
	int cols = vts[rows].size() - 1;
	//���ж����һλ�Ƿ���0�������������Ͳ����˷�ʱ����
	if (vts[rows][cols] != 0) return false;;
	int checknum = 1;
	for (int row = 0; row <= rows; ++row) {
		for (int col = 0; col <= cols; ++col) {
			//���һ���Ѿ���⣬ֱ���˳�
			if (col == cols && row == rows) return true;
			if (vts[row][col] != checknum) return false;
			checknum++;
		}
	}
}

