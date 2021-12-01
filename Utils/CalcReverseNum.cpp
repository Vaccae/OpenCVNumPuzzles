#include "CalcReverseNum.h"

//��̬������Ҫ��CPP�ļ����ȳ�ʼ��
vector<int> CalcReverseNum::tmpvts(1);

int CalcReverseNum::merge(vector<int> &vts, int left, int mid, int right)
{
	//���ݴ�������������̬�����Ĵ�С
	tmpvts.resize(vts.size());
	for (int i = left; i <= right; i++)
		tmpvts[i] = vts[i];
	int i = left, j = mid + 1, k= left;
	int count = 0;
	//�����Ƚ�������������
	while (i <= mid && j <= right) {
		//��벿��Ԫ��С���Ұ벿�ֵ�Ԫ��
		if (tmpvts[i] <= tmpvts[j])
			vts[k++] = tmpvts[i++];
		else
		{
			//ͳ�������ܺ��Ұ�߸�Ԫ�ع��ɵ��������
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
	//ͨ������˼��ȡ�м����ٵݹ���ȥ
	int mid = left + (right - left) / 2;
	//�ݹ�������벿��
	int leftnum = MergeSort(vts, left, mid);
	//�ݹ������Ұ벿��
	int rightnum = MergeSort(vts, mid + 1, right);
	//���㵱ǰ����
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

