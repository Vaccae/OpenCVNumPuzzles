#include "CalcPathPlan.h"

CalcPos* CalcPathPlan::findPath(CalcPos& startPos, CalcPos& endPos)
{
	//����д�����,��������һ���ڵ㣬�������
	CalcPos* firstpt = new CalcPos(startPos.row, startPos.col);
	firstpt->H = calcH(firstpt, &endPos);
	firstpt->F = calcF(firstpt);

	openList.push_front(firstpt);

	while (!openList.empty()) {
		//�ҵ�Fֵ��С�ĵ�
		auto curPoint = getLeastFpoint();

		//�ӿ����б���ɾ��
		openList.remove(curPoint);
		//��ŵ��ر��б���
		closeList.push_front(curPoint);


		//1.�ҵ���ǰ����Χ�ĸ����п���ͨ���ĵ�
 		auto surroundPos = getSurroundPos(curPoint);

		for (auto& target : surroundPos) {
			//2.��ĳһ���㣬������ڿ����б��У����뵽�����б��У����õ�ǰ��Ϊ���ڵ㣬����F,G,H��ֵ
			CalcPos* targetpos = isInList(openList, target);
			if (!targetpos) {
				//����F,G,H��ֵ
				target->G = calcG(curPoint, target);
				target->H = calcH(target, &endPos);
				target->F = calcF(target);

				target->parent = curPoint;

				//���뵽�����б���
				openList.push_front(target);
			}
			//3.��ĳ�����ڿ����б��м���Gֵ�������ԭ���Ĵ󣬾�ʲô������
			//�����������ĸ��ڵ�Ϊ��ǰ�㣬������G��F
			else {
				int tempG = calcG(curPoint, targetpos);
				if (tempG < targetpos->G) {
					targetpos->parent = curPoint;

					targetpos->G = tempG;
					targetpos->F = calcF(targetpos);
				}
			}

			CalcPos* resPoint = isInList(openList, &endPos);
			//�����б���Ľڵ�ָ�룬��Ҫ��ԭ�������endpointָ�룬��Ϊ��������� 
			if (resPoint)
				return resPoint;
		}
	}
	return NULL;
}

//���㵱ǰ��Ŀ��ƶ����������ҵ�
vector<CalcPos*> CalcPathPlan::getSurroundPos(const CalcPos* pos)
{
	vector<CalcPos*> surroundPos;

	//�Ϸ���
	if (isCanCalc(pos, new CalcPos(pos->row - 1, pos->col))) {
		surroundPos.push_back(new CalcPos(pos->row - 1, pos->col));
	}
	//�·���
	if (isCanCalc(pos, new CalcPos(pos->row + 1, pos->col))) {
		surroundPos.push_back(new CalcPos(pos->row + 1, pos->col));
	}
	//�󷽵�
	if (isCanCalc(pos, new CalcPos(pos->row, pos->col - 1))) {
		surroundPos.push_back(new CalcPos(pos->row, pos->col - 1));
	}
	//�ҷ���
	if (isCanCalc(pos, new CalcPos(pos->row, pos->col + 1))) {
		surroundPos.push_back(new CalcPos(pos->row, pos->col + 1));
	}

	return surroundPos;
}

bool CalcPathPlan::isCanCalc(const CalcPos* pos, const CalcPos* target)
{
	//����С��0ֱ�Ӳ�������
	if (target->col < 0 || target->row < 0
		//����ĵ��뵱ǰһ��Ҳ������
		|| (target->col == pos->col && target->row == pos->row)
		//�жϵ����ϰ����в�����
		|| isInSites(target->row, target->col)
		//������ڹر��б���Ҳ������
		|| isInList(closeList, target))
		return false;
	else {
		return true;
	}
}

//�жϿ���/�ر��б����Ƿ����ĳ��
CalcPos* CalcPathPlan::isInList(const list<CalcPos*>& list, const CalcPos* pos)
{
	//�ж�ĳ���ڵ��Ƿ����б��У����ﲻ�ܱȽ�ָ�룬��Ϊÿ�μ����б����¿��ٵĽڵ㣬ֻ�ܱȽ�����
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

//��ȡ�����б���Fֵ��С�ĵ���ж������
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

//�����Ѿ��ߵľ��� 
float CalcPathPlan::calcG(CalcPos* temp_start, CalcPos* pos)
{
	float tempG = 1.0f;
	//�ж��ƶ������Ƿ������ȼ�
	//��Ϊ�ƶ��������ʱ���������پ��룬������ľ����п���һ������������
	//�������ȼ��жϣ�������������ߣ����ƶ�ʱ���ĵı�ԭ��1.0Сһ�㣬����
	//�������ʱ������
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

	//�ж��ǲ��ǳ�ʼ�Ľڵ㣬����ǳ�ʼ��Լ�����丸�ڵ�Ϊ��
	int parentG = pos->parent == NULL ? 0 : pos->parent->G;
	//����G��������ж�����ֱ�ߺ�б�������ĵ���G
	return parentG + tempG;
}

float CalcPathPlan::calcH(CalcPos* pos, CalcPos* end)
{
	//�����յ㵽��ǰ��ľ��룬��Ϊ����б���ߣ����Բ��������پ������
	return abs(end->col - pos->col) + abs(end->row - pos->row);
}

float CalcPathPlan::calcF(CalcPos* pos)
{
	//��ʽ F=G+H
	return pos->F = pos->G + pos->H;
}


//��ʼ����ͼ
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
	//����·�������û���ҵ�·�������ؿ�����
	while (result) {
		pair<int, int> curpath(result->row, result->col);
		path.insert(path.begin(), curpath);
		result = result->parent;
	}
	return path;
}
