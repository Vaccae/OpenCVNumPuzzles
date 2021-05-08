#include "CalcPathPlan.h"

CalcPos* CalcPathPlan::findPath(CalcPos& startPos, CalcPos& endPos)
{
	//首先写入起点,拷贝开启一个节点，内外隔离
	CalcPos* firstpt = new CalcPos(startPos.row, startPos.col);
	firstpt->H = calcH(firstpt, &endPos);
	firstpt->F = calcF(firstpt);

	openList.push_front(firstpt);

	while (!openList.empty()) {
		//找到F值最小的点
		auto curPoint = getLeastFpoint();

		//从开启列表中删除
		openList.remove(curPoint);
		//存放到关闭列表中
		closeList.push_front(curPoint);


		//1.找到当前点周围四个点中可以通过的点
 		auto surroundPos = getSurroundPos(curPoint);

		for (auto& target : surroundPos) {
			//2.对某一个点，如果不在开启列表中，加入到开启列表中，设置当前格为父节点，计算F,G,H的值
			CalcPos* targetpos = isInList(openList, target);
			if (!targetpos) {
				//计算F,G,H的值
				target->G = calcG(curPoint, target);
				target->H = calcH(target, &endPos);
				target->F = calcF(target);

				target->parent = curPoint;

				//插入到开启列表中
				openList.push_front(target);
			}
			//3.对某个点在开启列表中计算G值，如果比原来的大，就什么都不做
			//否则设置它的父节点为当前点，并更新G和F
			else {
				int tempG = calcG(curPoint, targetpos);
				if (tempG < targetpos->G) {
					targetpos->parent = curPoint;

					targetpos->G = tempG;
					targetpos->F = calcF(targetpos);
				}
			}

			CalcPos* resPoint = isInList(openList, &endPos);
			//返回列表里的节点指针，不要用原来传入的endpoint指针，因为发生了深拷贝 
			if (resPoint)
				return resPoint;
		}
	}
	return NULL;
}

//计算当前点的可移动的上下左右点
vector<CalcPos*> CalcPathPlan::getSurroundPos(const CalcPos* pos)
{
	vector<CalcPos*> surroundPos;

	//上方点
	if (isCanCalc(pos, new CalcPos(pos->row - 1, pos->col))) {
		surroundPos.push_back(new CalcPos(pos->row - 1, pos->col));
	}
	//下方点
	if (isCanCalc(pos, new CalcPos(pos->row + 1, pos->col))) {
		surroundPos.push_back(new CalcPos(pos->row + 1, pos->col));
	}
	//左方点
	if (isCanCalc(pos, new CalcPos(pos->row, pos->col - 1))) {
		surroundPos.push_back(new CalcPos(pos->row, pos->col - 1));
	}
	//右方点
	if (isCanCalc(pos, new CalcPos(pos->row, pos->col + 1))) {
		surroundPos.push_back(new CalcPos(pos->row, pos->col + 1));
	}

	return surroundPos;
}

bool CalcPathPlan::isCanCalc(const CalcPos* pos, const CalcPos* target)
{
	//坐标小于0直接不计算了
	if (target->col < 0 || target->row < 0
		//计算的点与当前一致也不计算
		|| (target->col == pos->col && target->row == pos->row)
		//判断点在障碍点中不计算
		|| isInSites(target->row, target->col)
		//如果点在关闭列表中也不计算
		|| isInList(closeList, target))
		return false;
	else {
		return true;
	}
}

//判断开启/关闭列表中是否包含某点
CalcPos* CalcPathPlan::isInList(const list<CalcPos*>& list, const CalcPos* pos)
{
	//判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标
	for (auto p : list)
		if (p->col == pos->col && p->row == pos->row)
			return p;
	return NULL;
}

bool CalcPathPlan::isInSites(const int row, const int col) const
{
	if (col < 0 || row < 0 || row >= sites.size()
		|| col >= sites[0].size()) return true;
	return sites[row][col] == 1;
}

//获取开启列表中F值最小的点才行动计算点
CalcPos* CalcPathPlan::getLeastFpoint()
{
	if (!openList.empty())
	{
		auto resPos = openList.front();
		for (auto& pos : openList)
			if (pos->F < resPos->F)
				resPos = pos;
		return resPos;
	}
	return NULL;
}

//计算已经走的距离 
float CalcPathPlan::calcG(CalcPos* temp_start, CalcPos* pos)
{
	float tempG = 1.0f;
	//判断移动方向，是否有优先级
	//因为移动距离计算时采用曼哈顿距离，两个点的距离有可能一样，所以这里
	//加入优先级判断，如果优先往上走，则移动时消耗的比原来1.0小一点，这样
	//计算距离时更近了
	switch (DirectFirst)
	{
	case DirectFirst::Up: {
		tempG = temp_start->row > pos->row ? 0.9f : tempG;
		break;
	}
	case DirectFirst::Down: {
		tempG = temp_start->row < pos->row ? 0.9f : tempG;
		break;
	}
	case DirectFirst::Left: {
		tempG = temp_start->col > pos->col ? 0.9f : tempG;
		break;
	}
	case DirectFirst::Right: {
		tempG = temp_start->col < pos->col ? 0.9f : tempG;
		break;
	}
	default:
		tempG = 1.0f;
		break;
	}

	//判断是不是初始的节点，如果是初始节约，则其父节点为空
	int parentG = pos->parent == NULL ? 0 : pos->parent->G;
	//两个G相加用于判断是走直线和斜线所消耗的总G
	return parentG + tempG;
}

float CalcPathPlan::calcH(CalcPos* pos, CalcPos* end)
{
	//计算终点到当前点的距离，因为不用斜着走，所以采用曼哈顿距离计算
	return abs(end->col - pos->col) + abs(end->row - pos->row);
}

float CalcPathPlan::calcF(CalcPos* pos)
{
	//公式 F=G+H
	return pos->F = pos->G + pos->H;
}


//初始化地图
void CalcPathPlan::InitSites(vector<vector<int>> _sites)
{
	sites = _sites;
}

vector<pair<int, int>> CalcPathPlan::GetPath(pair<int, int>& startPos, pair<int, int>& endPos)
{
	CalcPos sPos = CalcPos(startPos.first, startPos.second);
	CalcPos ePos = CalcPos(endPos.first, endPos.second);
	CalcPos* result = findPath(sPos, ePos);
	vector<pair<int, int>> path;
	//返回路径，如果没有找到路径，返回空链表
	while (result) {
		pair<int, int> curpath(result->row, result->col);
		path.insert(path.begin(), curpath);
		result = result->parent;
	}
	return path;
}
