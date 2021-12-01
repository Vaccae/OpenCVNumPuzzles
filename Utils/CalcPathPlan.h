#pragma once
#include <iostream>
#include <vector>
#include <list>

using namespace std;

//�޸ĺ�򻯰��A*·���滮,������б���ƶ���������ڽ���ҲֻΪ��������4��
struct CalcPos
{
public:
	int col; //X����λ��  
	int row; //Y����λ��
	float F, G, H; //F=G+H   GΪ��㵽��ǰ���Ѿ��ƶ��ľ��룬HΪ��ǰ�㵽�յ�ľ��� 
	CalcPos* parent; //���ڵ�

	CalcPos(int _row, int _col) : row(_row), col(_col), F(0), G(0), H(0), parent(NULL)
	{
	}
};

//�����ƶ�����
enum DirectFirst {
	Up = 0,
	Down = 1,
	Left = 2,
	Right = 3,
	None = 9
};

class CalcPathPlan
{
private:
	CalcPos* findPath(CalcPos& startPos, CalcPos& endPos);
	//�����ٽ�����������4����
	vector<CalcPos*> getSurroundPos(const CalcPos* pos);
	//�жϵ�ǰ�ĵ��Ƿ������������б�
	bool isCanCalc(const CalcPos* pos, const CalcPos* target);
	//�жϵ��Ƿ��ڿ������ǹر��б���
	CalcPos* isInList(const list<CalcPos*>& list, const CalcPos* pos);
	//����ϰ���
	bool isInSites(const int row, const int col) const;
	//�ӿ����б��з���Fֵ��С�Ľڵ� 
	CalcPos* getLeastFpoint();

	//����FGHֵ 
	float calcG(CalcPos* temp_start, CalcPos* pos);
	float calcH(CalcPos* pos, CalcPos* end);
	float calcF(CalcPos* pos);

	vector<vector<int>> sites;  //���ݵ����� 0-��ͨ�У�1-�ϰ���
	list<CalcPos*> openList;  //�����б� 
	list<CalcPos*> closeList; //�ر��б� 

public:
	//���������ƶ�����
	int DirectFirst = DirectFirst::Up;
	//��ʼ����ͼ
	void InitSites(vector<vector<int>> _sites);
	//��ȡ��·��
	vector<pair<int,int>> GetPath(pair<int, int>& startPos, pair<int, int>& endPos);
};

