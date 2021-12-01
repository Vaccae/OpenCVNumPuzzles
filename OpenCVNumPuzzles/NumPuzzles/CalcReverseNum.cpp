#include "CalcReverseNum.h"

//静态变量需要在CPP文件中先初始化
vector<int> CalcReverseNum::tmpvts(1);

int CalcReverseNum::merge(vector<int> &vts, int left, int mid, int right)
{
	//根据传入的数组调整静态变量的大小
	tmpvts.resize(vts.size());
	for (int i = left; i <= right; i++)
		tmpvts[i] = vts[i];
	int i = left, j = mid + 1, k= left;
	int count = 0;
	//遍历比较左右两个部分
	while (i <= mid && j <= right) {
		//左半部分元素小于右半部分的元素
		if (tmpvts[i] <= tmpvts[j])
			vts[k++] = tmpvts[i++];
		else
		{
			//统计左半边能和右半边该元素构成的逆序对数
			vts[k++] = tmpvts[j++];
			count += mid - i + 1;
		}
	}

	while (i <= mid)
		vts[k++] = tmpvts[i++];
	while (j <= right)
		vts[k++] = tmpvts[j++];
	return count;
}

int CalcReverseNum::MergeSort(vector<int>& vts, int left, int right)
{
	if (left >= right) return 0;
	//通过分治思想取中间数再递归下去
	int mid = left + (right - left) / 2;
	//递归排序左半部分
	int leftnum = MergeSort(vts, left, mid);
	//递归排序右半部分
	int rightnum = MergeSort(vts, mid + 1, right);
	//计算当前部分
	int nownum = merge(vts, left, mid, right);

	return leftnum + rightnum + nownum;
}

int CalcReverseNum::CheckCount(vector<int>& vts)
{
	int count = 0;

	for (int i = 0; i < vts.size(); ++i) {
		cout << vts[i] << " ";
		for (int j = 0; j < i; j++) {
			if (vts[j] > vts[i]) count++;
		}
	}
	return count;
}

