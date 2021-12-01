#pragma once
#include <iostream>
#include <vector>
#include <list>

using namespace std;

//修改后简化版的A*路径规划,不存在斜线移动，计算的邻近点也只为上下左右4个
struct CalcPos
{
public:
	int col; //X坐标位置  
	int row; //Y坐标位置
	float F, G, H; //F=G+H   G为起点到当前点已经移动的距离，H为当前点到终点的距离 
	CalcPos* parent; //父节点

	CalcPos(int _row, int _col) : row(_row), col(_col), F(0), G(0), H(0), parent(NULL)
	{
	}
};

//优先移动方向
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
	//计算临近的上下左右4个点
	vector<CalcPos*> getSurroundPos(const CalcPos* pos);
	//判断当前的点是否可以列入计算列表
	bool isCanCalc(const CalcPos* pos, const CalcPos* target);
	//判断点是否在开启或是关闭列表中
	CalcPos* isInList(const list<CalcPos*>& list, const CalcPos* pos);
	//检测障碍点
	bool isInSites(const int row, const int col) const;
	//从开启列表中返回F值最小的节点 
	CalcPos* getLeastFpoint();

	//计算FGH值 
	float calcG(CalcPos* temp_start, CalcPos* pos);
	float calcH(CalcPos* pos, CalcPos* end);
	float calcF(CalcPos* pos);

	vector<vector<int>> sites;  //华容道棋盘 0-可通行，1-障碍点
	list<CalcPos*> openList;  //开启列表 
	list<CalcPos*> closeList; //关闭列表 

public:
	//参数优先移动方向
	int DirectFirst = DirectFirst::Up;
	//初始化地图
	void InitSites(vector<vector<int>> _sites);
	//获取到路径
	vector<pair<int,int>> GetPath(pair<int, int>& startPos, pair<int, int>& endPos);
};

