#include "Puzzles4x4.h"

int Puzzles4x4::SleepTime = 200;

//检测当前要还原的步骤
//返回数字为当前对应的数字步骤，特殊步骤用其它数字标识
//参数pair<int, int>& pos为当前步骤要到达的点
int Puzzles4x4::CheckSteps(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	//检测当前要还原的步骤及初始化路径地图
	sites = vector<vector<int>>(vts.size(), vector<int>(vts[0].size(), 0));

	int step = RestoreStep::NONE;
	int checknum = 1;
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			switch (checknum)
			{
			case RestoreStep::Num1:
			case RestoreStep::Num2:
			case RestoreStep::Num3:
			case RestoreStep::Num4:
			case RestoreStep::Num5:
			case RestoreStep::Num6:
			case RestoreStep::Num7:
			case RestoreStep::Num8:
			{
				if (vts[row][col] == checknum) {
					sites[row][col] = 1;
					checknum++;
				}
				else {
					return checknum;
				}
			}
			break;
			case RestoreStep::Num9:
			case RestoreStep::Num10:
			{
				//检测9和10的特殊步骤
				if (vts[3][0] == RestoreStep::Num9 && vts[2][0] == RestoreStep::Num10) {
					sites[3][0] = 1;
					sites[2][0] = 1;
					//直接检测12
					checknum = RestoreStep::Num12;
				}
				else if (vts[2][0] == RestoreStep::Num9 && vts[2][1] == RestoreStep::Num10) {
					//进入特殊步骤
					return RestoreStep::Step9and10;
				}
				else if (vts[2][0] != RestoreStep::Num9) {
					return RestoreStep::Num9;
				}
				else if (vts[2][0] == RestoreStep::Num9 && vts[2][1] != RestoreStep::Num10) {
					//锁定9处理10
					sites[2][0] = 1;
					return RestoreStep::Num10;
				}
			}
			break;
			case RestoreStep::Num11:
			case RestoreStep::Num12:
			{
				//检测12和11的特殊步骤
				if (vts[3][3] == RestoreStep::Num12 && vts[2][3] == RestoreStep::Num11) {
					sites[3][3] = 1;
					sites[2][3] = 1;
					checknum = RestoreStep::Step13and15;
				}
				else if (vts[2][3] == RestoreStep::Num12 && vts[2][2] == RestoreStep::Num11) {
					//进入特殊步骤
					return RestoreStep::Step11and12;
				}
				else if (vts[2][3] != RestoreStep::Num12) {
					return RestoreStep::Num12;
				}
				else if (vts[2][3] == RestoreStep::Num12 && vts[2][2] != RestoreStep::Num11) {
					//处理完12再处理11
					sites[2][3] = 1;
					return RestoreStep::Num11;
				}
			}
			break;
			case RestoreStep::Step13and15:
			{
				if (vts[3][1] == RestoreStep::Num13
					&& vts[3][2] == RestoreStep::Num14
					&& vts[2][2] == RestoreStep::Num15) {
					return RestoreStep::StepFinal;
				}
				else {
					return RestoreStep::Step13and15;
				}
			}
			break;
			default:
				break;
			}
		}
	}
	return step;
}

//获取数字对应位置
pair<int, int> Puzzles4x4::GetPos(vector<vector<int>>& vts, int num)
{
	pair<int, int> pos;
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			if (vts[row][col] == num) {
				pos.first = row;
				pos.second = col;
				break;
			}
		}
	}
	return pos;
}

pair<int, int> Puzzles4x4::CheckNumPos(vector<vector<int>>& vts, int num1, int num2)
{
	pair<int, int> NumOne = GetPos(vts, num1);
	pair<int, int> NumTwo = GetPos(vts, num2);

	return pair<int, int>(NumTwo.first - NumOne.first, NumTwo.second - NumOne.second);
}

vector<pair<int, int>> Puzzles4x4::FindPath(vector<vector<int>>& sites, pair<int, int>& startpos, pair<int, int>& endpos, int directfirst)
{
	CalcPathPlan plan = CalcPathPlan();
	plan.DirectFirst = directfirst;
	plan.InitSites(sites);

	return plan.GetPath(startpos, endpos);
}

//两个点交换位置
void Puzzles4x4::SwapPos(vector<vector<int>>& vts, pair<int, int>& firstPos, pair<int, int>& secondPos)
{
	int tmpnum = vts[firstPos.first][firstPos.second];
	vts[firstPos.first][firstPos.second] = vts[secondPos.first][secondPos.second];
	vts[secondPos.first][secondPos.second] = tmpnum;

	DrawPuzzles(vts);
}

void Puzzles4x4::DealStep(vector<vector<int>>& vts, vector<vector<int>>& sites, int step)
{
	switch (step)
	{
		//4个8的特殊处理
	case RestoreStep::Num4:
	case RestoreStep::Num8:
	{
		//计算当前处理的行数
		int row = step / 4 - 1;
		DealTopTwoRows(vts, sites, row);
	}
	break;
	case RestoreStep::Step9and10:
	{
		//处理9和10的位置
		DealNineTen(vts, sites);
	}
	break;
	case RestoreStep::Step11and12:
	{
		//处理11和12的位置
		DealElevenTwelve(vts, sites);
	}
	break;
	case RestoreStep::Step13and15:
	{
		//到这一步清除划的路径
		RestorePath.clear();
		//处理13到15的步骤
		DealGTThirteen(vts, sites);
	}
	break;
	case RestoreStep::StepFinal:
	{
		DealFinal(vts, sites);
	}
	break;
	case RestoreStep::Num11:
	case RestoreStep::Num10:
	{
		//处理9或是12锁定时无法移动10和11的问题
		DealError(vts, sites, step);
	}
	break;
	default:
		break;
	}
}

void Puzzles4x4::DealTopTwoRows(vector<vector<int>>& vts, vector<vector<int>>& sites, int row)
{
	//1.先将0移动到当前要处理的行的下面格
	pair<int, int> endPos(row + 1, 0);
	ZeroMove(vts, sites, endPos, DirectFirst::Left);

	//2.解锁处理行前面的障碍点，用0的位置优先移动到计算点
	for (int i = 0; i < sites[row].size(); ++i) {
		sites[row][i] = 0;
	}
	endPos.first = row;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos, DirectFirst::Up);

	//3.数字0再和当前要处理的点进行位置互换，将我们4或8位置移动到对应后锁定
	endPos.first = row + 1;
	endPos.second = 3;
	//防止移动点是锁定的，将改为可移动
	sites[endPos.first][endPos.second] = 0;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
	//设置为锁定障碍点
	sites[row][3] = 1;

	//4.将数字0移动到第二步位置后还原当前行前三个数字
	endPos.first = row;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos);

	//5.将0优先按左移的方式把原来行前面的数字还原回来
	endPos.first = row + 1;
	endPos.second = 0;
	ZeroMove(vts, sites, endPos, DirectFirst::Left);
}

void Puzzles4x4::DealNineTen(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	if ((vts[2][0] == 9) && (vts[2][1] = 10)) {
		//1.锁定9个10当前位置,然后将0移动到左下角
		sites[2][0] = 1;
		sites[2][1] = 1;
		pair<int, int> endPos(3, 0);
		ZeroMove(vts, sites, endPos, DirectFirst::Left);

		//2.解锁9个10的位置，将0移动到第三行第二个
		sites[2][0] = 0;
		sites[2][1] = 0;
		endPos.first = 2;
		endPos.second = 1;
		ZeroMove(vts, sites, endPos, DirectFirst::Up);
	}
}

void Puzzles4x4::DealElevenTwelve(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	if ((vts[2][3] == 12) && (vts[2][2] = 11)) {
		//1.锁定11个12当前位置,然后将0移动到右下角
		sites[2][2] = 1;
		sites[2][3] = 1;
		pair<int, int> endPos(3, 3);
		ZeroMove(vts, sites, endPos, DirectFirst::Right);

		//2.解锁11个12的位置，将0移动到第三行第三个
		sites[2][2] = 0;
		sites[2][3] = 0;
		endPos.first = 2;
		endPos.second = 2;
		ZeroMove(vts, sites, endPos, DirectFirst::Up);
	}
}

void Puzzles4x4::DealGTThirteen(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	//按逆时针旋转0，直到符合结束的情况
	while (!((vts[3][1] == RestoreStep::Num13)
		&& (vts[3][2] == RestoreStep::Num14)
		&& vts[2][2] == RestoreStep::Num15)) {
		pair<int, int> ZeroPos = GetPos(vts, 0);
		pair<int, int> EndPos;
		if (ZeroPos.first == 2 && ZeroPos.second == 1) {
			EndPos.first = 3;
			EndPos.second = 1;
		}
		else if (ZeroPos.first == 3 && ZeroPos.second == 1) {
			EndPos.first = 3;
			EndPos.second = 2;
		}
		else if (ZeroPos.first == 3 && ZeroPos.second == 2) {
			EndPos.first = 2;
			EndPos.second = 2;
		}
		else if (ZeroPos.first == 2 && ZeroPos.second == 2) {
			EndPos.first = 2;
			EndPos.second = 1;
		}
		cout << "from:" << ZeroPos.first << " " << ZeroPos.second << " to:" << EndPos.first << " " << EndPos.second << endl;
		SwapPos(vts, ZeroPos, EndPos);
	}
}

void Puzzles4x4::DealFinal(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	//1.将9和10先还原后再锁定
	sites[2][0] = 0;
	sites[3][0] = 0;
	pair<int, int> endPos(3, 0);
	ZeroMove(vts, sites, endPos, DirectFirst::Left);
	sites[2][0] = 1;
	sites[2][1] = 1;
	//2.还原13到15的位置
	endPos.first = 2;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
	sites[3][0] = 1;
	sites[3][1] = 1;
	sites[3][2] = 1;
	//3.还原11和12的位置
	sites[2][3] = 0;
	sites[3][3] = 0;
	endPos.first = 3;
	endPos.second = 3;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
}

void Puzzles4x4::DealError(vector<vector<int>>& vts, vector<vector<int>>& sites, int num)
{
	//进入这里说明遇到特殊情况，解锁9，然后移动10
	sites[2][0] = 0;
	//获取开始结束的点位置
	pair<int, int> sPos = GetPos(vts, num);
	//获取应到达位置
	pair<int, int> ePos;
	switch (num)
	{
	case 10:
	{
		sites[2][0] = 0;
		ePos.first = 3;
		ePos.second = 2;
	}
	break;
	case 11:
	{
		sites[2][3] = 0;
		ePos.first = 3;
		ePos.second = 1;
	}
	default:
		break;
	}

	RestorePath = FindPath(sites, sPos, ePos);
	for (int i = 0; i < RestorePath.size(); ++i) {
		//cout << restorepath[i].first << " " << restorepath[i].second << endl;
		//1数字当前位置不计算
		int front = i - 1;
		if (front < 0) continue;

		//2 计算0到当前点的路径
		pair<int, int> ZeroPos = GetPos(vts, 0);

		//3.2.1 如果0的点与要移动的点重合，直接交换位置
		if (ZeroPos.first == RestorePath[i].first && ZeroPos.second == RestorePath[i].second)
		{
			SwapPos(vts, RestorePath[i], RestorePath[i - 1]);
			continue;
		}

		//3把数字当前位置设置为障碍点，这样0不允许与当前数字交换位置
		sites[RestorePath[front].first][RestorePath[front].second] = 1;

		int stepnum = ZeroMove(vts, sites, RestorePath[i]);
		if (stepnum > 0) {
			//3.3.2 移动完成当前数字与0互换进行位移
			SwapPos(vts, RestorePath[i], RestorePath[front]);
			//3.3.3 取消当前地图上的障碍点
			sites[RestorePath[front].first][RestorePath[front].second] = 0;
		}
		else {
			//如果没有路径就是遇到特殊情况，进行单独处理
			DealStep(vts, sites, num);
		}
	}
}

int Puzzles4x4::ZeroMove(vector<vector<int>>& vts, vector<vector<int>>& sites, pair<int, int>& endPos, int MoveDirect)
{
	pair<int, int> ZeroPos = GetPos(vts, 0);
	vector<pair<int, int>> zeropath;

	if (ZeroPos.first == endPos.first && ZeroPos.second == endPos.second) {
		return 0;
	}
	zeropath = FindPath(sites, ZeroPos, endPos, MoveDirect);
	//3.3.1 开始移动0到当前位置,从第一步开始，因为起点不动
	for (int k = 1; k < zeropath.size(); ++k) {
		//cout << "zeropath:" << zeropath[k].first << " " << zeropath[k].second << endl;
		//当前位置与前一位置与换
		SwapPos(vts, zeropath[k], zeropath[k - 1]);
	}

	return zeropath.size();
}

void Puzzles4x4::DrawPuzzles(vector<vector<int>>& nummatrix, vector<pair<int, int>> movepath)
{

	int starttop = 50;
	int startleft = 50;
	int rectlen = 100;
	//创建白底图像
	Mat src = Mat(Size(500, 500), CV_8UC3, Scalar(255, 255, 255));
	//创建华容着棋盘背景
	Rect bgrect = Rect(startleft, starttop, rectlen * 4, rectlen * 4);
	rectangle(src, bgrect, Scalar(208, 208, 225), -1);

	for (int i = 0; i < 16; ++i) {
		//计算当前顺序为二维数组的几行几列
		int row = i / nummatrix.size();
		int col = i % nummatrix.size();

		//检测是否要划路径
		for (int k = 0; k < movepath.size(); ++k) {
			if (movepath[k].first == row && movepath[k].second == col) {
				Rect pathrect = Rect(startleft + col * rectlen + 1, starttop + row * rectlen + 1, rectlen - 1, rectlen - 1);
				Scalar color;
				if (k == 0)
				{
					//路径规划的起点颜色
					color = Scalar(200, 240, 220);
				}
				else if (k == movepath.size() - 1) {
					//路径规划的终点颜色
					color = Scalar(200, 220, 250);
				}
				else {
					//路径规划的路径颜色
					color = Scalar(255, 255, 266);
				}
				rectangle(src, pathrect, color, -1);
				break;
			}
		}

		//生成对应的矩形框
		Rect rect = Rect(startleft + col * rectlen, starttop + row * rectlen, rectlen, rectlen);
		rectangle(src, rect, Scalar::all(0));

		//写上对应数字
		if (nummatrix[row][col] == 0) continue;
		string text = to_string(nummatrix[row][col]);
		putText(src, text, Point(rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 + 10),
			FONT_HERSHEY_DUPLEX, 1.0, Scalar::all(0));
	}
	imshow("CalcPath", src);
	//按ESC键退出还原过程
	if (waitKey(SleepTime) == 13) {
		waitKey(0);
	};
	
}

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
	if (col - 1 >= 0 && vts[row][col - 1] == 0) {
		vts[row][col - 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//2.右边
	else if (col + 1 <= 3 && vts[row][col + 1] == 0) {
		vts[row][col + 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//3.上边
	else if (row + 1 <= 3 && vts[row + 1][col] == 0) {
		vts[row + 1][col] = vts[row][col];
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

//还原游戏
void Puzzles4x4::RestoreGame(vector<vector<int>>& vts)
{
	//复制原棋盘
	vector<vector<int>> bakvts(vts.size(), vector<int>(vts[0].size(), 0));
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			bakvts[row][col] = vts[row][col];
		}
	}

	vector<vector<int>> sites;
	//当前规划路线中每一步的行动路线
	vector<pair<int, int>> currectpath;
	//开始还原
	while (!CheckFinished(bakvts)) {
		//1.先检测当前棋盘需要还原的
		int step = CheckSteps(bakvts, sites);
		cout << "step:" << step << endl;
		//SleepTime = step > 8 ? 500 : 10;

		//2.计算路径
		switch (step)
		{
		case RestoreStep::Num1:
		case RestoreStep::Num2:
		case RestoreStep::Num3:
		case RestoreStep::Num4:
		case RestoreStep::Num5:
		case RestoreStep::Num6:
		case RestoreStep::Num7:
		case RestoreStep::Num8:
		case RestoreStep::Num9:
		case RestoreStep::Num10:
		case RestoreStep::Num11:
		case RestoreStep::Num12:
		{
			//获取开始结束的点位置
			pair<int, int> sPos = GetPos(bakvts, step);
			//获取应到达位置
			int row = (step - 1) / 4;
			int col = (step - 1) % 4;
			pair<int, int> ePos(row, col);
			RestorePath = FindPath(sites, sPos, ePos, DirectFirst::Up);

			DrawPuzzles(bakvts);
			//3.遍历路径每一步处理移动路径
			for (int i = 0; i < RestorePath.size(); ++i) {
				cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
				//3.1数字当前位置不计算
				int front = i - 1;
				if (front < 0) continue;

				//3.2 计算0到当前点的路径
				pair<int, int> ZeroPos = GetPos(bakvts, 0);
				//cout << "ZeroPos:" << ZeroPos.first << " " << ZeroPos.second << endl;

				//3.2.1 如果0的点与要移动的点重合，直接交换位置
				if (ZeroPos.first == RestorePath[i].first && ZeroPos.second == RestorePath[i].second)
				{
					SwapPos(bakvts, RestorePath[i], RestorePath[i - 1]);
					continue;
				}

				//3.3把数字当前位置设置为障碍点，这样0不允许与当前数字交换位置
				//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
				sites[RestorePath[front].first][RestorePath[front].second] = 1;

				int stepnum = ZeroMove(bakvts, sites, RestorePath[i]);
				if (stepnum > 0) {
					//3.3.2 移动完成当前数字与0互换进行位移
					SwapPos(bakvts, RestorePath[i], RestorePath[front]);
					//3.3.3 取消当前地图上的障碍点
					sites[RestorePath[front].first][RestorePath[front].second] = 0;
				}
				else {
					//如果没有路径就是遇到特殊情况，进行单独处理
					DealStep(bakvts, sites, step);
				}
			}
		}
		break;
		case RestoreStep::Step9and10:
		case RestoreStep::Step11and12:
		case RestoreStep::Step13and15:
		case RestoreStep::StepFinal:
		{
			DealStep(bakvts, sites, step);
		}
		break;
		default:
			break;
		}
	}
}

