#include "Puzzles4x4.h"

int Puzzles4x4::SleepTime = 200;
bool Puzzles4x4::IsShowStep = true;

//��⵱ǰҪ��ԭ�Ĳ���
//��������Ϊ��ǰ��Ӧ�����ֲ��裬���ⲽ�����������ֱ�ʶ
//����pair<int, int>& posΪ��ǰ����Ҫ����ĵ�
int Puzzles4x4::CheckSteps(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	//��⵱ǰҪ��ԭ�Ĳ��輰��ʼ��·����ͼ
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
				//���9��10�����ⲽ��
				if (vts[3][0] == RestoreStep::Num9 && vts[2][0] == RestoreStep::Num10) {
					sites[3][0] = 1;
					sites[2][0] = 1;
					//ֱ�Ӽ��12
					checknum = RestoreStep::Num12;
				}
				else if (vts[2][0] == RestoreStep::Num9 && vts[2][1] == RestoreStep::Num10) {
					//�������ⲽ��
					return RestoreStep::Step9and10;
				}
				else if (vts[2][0] != RestoreStep::Num9) {
					return RestoreStep::Num9;
				}
				else if (vts[2][0] == RestoreStep::Num9 && vts[2][1] != RestoreStep::Num10) {
					//����9����10
					sites[2][0] = 1;
					return RestoreStep::Num10;
				}
			}
			break;
			case RestoreStep::Num11:
			case RestoreStep::Num12:
			{
				//���12��11�����ⲽ��
				if (vts[3][3] == RestoreStep::Num12 && vts[2][3] == RestoreStep::Num11) {
					sites[3][3] = 1;
					sites[2][3] = 1;
					checknum = RestoreStep::Step13and15;
				}
				else if (vts[2][3] == RestoreStep::Num12 && vts[2][2] == RestoreStep::Num11) {
					//�������ⲽ��
					return RestoreStep::Step11and12;
				}
				else if (vts[2][3] != RestoreStep::Num12) {
					return RestoreStep::Num12;
				}
				else if (vts[2][3] == RestoreStep::Num12 && vts[2][2] != RestoreStep::Num11) {
					//������12�ٴ���11
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

//��ȡ���ֶ�Ӧλ��
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

//�����㽻��λ��
void Puzzles4x4::SwapPos(vector<vector<int>>& vts, pair<int, int>& firstPos, pair<int, int>& secondPos)
{
	//���뻹ԭ����
	pair<pair<int, int>, pair<int, int>> curstep(firstPos, secondPos);
	VetRestoreSteps.push_back(curstep);

	int tmpnum = vts[firstPos.first][firstPos.second];
	vts[firstPos.first][firstPos.second] = vts[secondPos.first][secondPos.second];
	vts[secondPos.first][secondPos.second] = tmpnum;

	DrawPuzzles(vts);
}

void Puzzles4x4::DealStep(vector<vector<int>>& vts, vector<vector<int>>& sites, int step)
{
	switch (step)
	{
		//4��8�����⴦��
	case RestoreStep::Num4:
	case RestoreStep::Num8:
	{
		//���㵱ǰ���������
		int row = step / 4 - 1;
		DealTopTwoRows(vts, sites, row);
	}
	break;
	case RestoreStep::Step9and10:
	{
		//����9��10��λ��
		DealNineTen(vts, sites);
	}
	break;
	case RestoreStep::Step11and12:
	{
		//����11��12��λ��
		DealElevenTwelve(vts, sites);
	}
	break;
	case RestoreStep::Step13and15:
	{
		//����һ���������·��
		RestorePath.clear();
		//����13��15�Ĳ���
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
		//����9����12����ʱ�޷��ƶ�10��11������
		DealError(vts, sites, step);
	}
	break;
	default:
		break;
	}
}

void Puzzles4x4::DealTopTwoRows(vector<vector<int>>& vts, vector<vector<int>>& sites, int row)
{
	//1.�Ƚ�0�ƶ�����ǰҪ������е������
	pair<int, int> endPos(row + 1, 0);
	ZeroMove(vts, sites, endPos, DirectFirst::Left);

	//2.����������ǰ����ϰ��㣬��0��λ�������ƶ��������
	for (int i = 0; i < sites[row].size(); ++i) {
		sites[row][i] = 0;
	}
	endPos.first = row;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos, DirectFirst::Up);

	//3.����0�ٺ͵�ǰҪ����ĵ����λ�û�����������4��8λ���ƶ�����Ӧ������
	endPos.first = row + 1;
	endPos.second = 3;
	//��ֹ�ƶ����������ģ�����Ϊ���ƶ�
	sites[endPos.first][endPos.second] = 0;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
	//����Ϊ�����ϰ���
	sites[row][3] = 1;

	//4.������0�ƶ����ڶ���λ�ú�ԭ��ǰ��ǰ��������
	endPos.first = row;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos);

	//5.��0���Ȱ����Ƶķ�ʽ��ԭ����ǰ������ֻ�ԭ����
	endPos.first = row + 1;
	endPos.second = 0;
	ZeroMove(vts, sites, endPos, DirectFirst::Left);
}

void Puzzles4x4::DealNineTen(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	if ((vts[2][0] == 9) && (vts[2][1] = 10)) {
		//1.����9��10��ǰλ��,Ȼ��0�ƶ������½�
		sites[2][0] = 1;
		sites[2][1] = 1;
		pair<int, int> endPos(3, 0);
		ZeroMove(vts, sites, endPos, DirectFirst::Left);

		//2.����9��10��λ�ã���0�ƶ��������еڶ���
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
		//1.����11��12��ǰλ��,Ȼ��0�ƶ������½�
		sites[2][2] = 1;
		sites[2][3] = 1;
		pair<int, int> endPos(3, 3);
		ZeroMove(vts, sites, endPos, DirectFirst::Right);

		//2.����11��12��λ�ã���0�ƶ��������е�����
		sites[2][2] = 0;
		sites[2][3] = 0;
		endPos.first = 2;
		endPos.second = 2;
		ZeroMove(vts, sites, endPos, DirectFirst::Up);
	}
}

void Puzzles4x4::DealGTThirteen(vector<vector<int>>& vts, vector<vector<int>>& sites)
{
	//����ʱ����ת0��ֱ�����Ͻ��������
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
	//1.��9��10�Ȼ�ԭ��������
	sites[2][0] = 0;
	sites[3][0] = 0;
	pair<int, int> endPos(3, 0);
	ZeroMove(vts, sites, endPos, DirectFirst::Left);
	sites[2][0] = 1;
	sites[2][1] = 1;
	//2.��ԭ13��15��λ��
	endPos.first = 2;
	endPos.second = 2;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
	sites[3][0] = 1;
	sites[3][1] = 1;
	sites[3][2] = 1;
	//3.��ԭ11��12��λ��
	sites[2][3] = 0;
	sites[3][3] = 0;
	endPos.first = 3;
	endPos.second = 3;
	ZeroMove(vts, sites, endPos, DirectFirst::Right);
}

void Puzzles4x4::DealError(vector<vector<int>>& vts, vector<vector<int>>& sites, int num)
{
	//��������˵�������������������9��Ȼ���ƶ�10
	sites[2][0] = 0;
	//��ȡ��ʼ�����ĵ�λ��
	pair<int, int> sPos = GetPos(vts, num);
	//��ȡӦ����λ��
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
		//1���ֵ�ǰλ�ò�����
		int front = i - 1;
		if (front < 0) continue;

		//2 ����0����ǰ���·��
		pair<int, int> ZeroPos = GetPos(vts, 0);

		//3.2.1 ���0�ĵ���Ҫ�ƶ��ĵ��غϣ�ֱ�ӽ���λ��
		if (ZeroPos.first == RestorePath[i].first && ZeroPos.second == RestorePath[i].second)
		{
			SwapPos(vts, RestorePath[i], RestorePath[i - 1]);
			continue;
		}

		//3�����ֵ�ǰλ������Ϊ�ϰ��㣬����0�������뵱ǰ���ֽ���λ��
		sites[RestorePath[front].first][RestorePath[front].second] = 1;

		int stepnum = ZeroMove(vts, sites, RestorePath[i]);
		if (stepnum > 0) {
			//3.3.2 �ƶ���ɵ�ǰ������0��������λ��
			SwapPos(vts, RestorePath[i], RestorePath[front]);
			//3.3.3 ȡ����ǰ��ͼ�ϵ��ϰ���
			sites[RestorePath[front].first][RestorePath[front].second] = 0;
		}
		else {
			//���û��·����������������������е�������
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
	//3.3.1 ��ʼ�ƶ�0����ǰλ��,�ӵ�һ����ʼ����Ϊ��㲻��
	for (int k = 1; k < zeropath.size(); ++k) {
		//cout << "zeropath:" << zeropath[k].first << " " << zeropath[k].second << endl;
		//��ǰλ����ǰһλ���뻻
		SwapPos(vts, zeropath[k], zeropath[k - 1]);
	}

	return zeropath.size();
}

//����ͼ��
void Puzzles4x4::DrawPuzzles(vector<vector<int>>& nummatrix, vector<pair<int, int>> movepath)
{
	if (!IsShowStep) return;
	int starttop = 50;
	int startleft = 50;
	int rectlen = 100;
	//�����׵�ͼ��
	Mat src = Mat(Size(500, 500), CV_8UC3, Scalar(255, 255, 255));
	//�������������̱���
	Rect bgrect = Rect(startleft, starttop, rectlen * 4, rectlen * 4);
	rectangle(src, bgrect, Scalar(208, 208, 225), -1);

	for (int i = 0; i < 16; ++i) {
		//���㵱ǰ˳��Ϊ��ά����ļ��м���
		int row = i / nummatrix.size();
		int col = i % nummatrix.size();

		//����Ƿ�Ҫ��·��
		for (int k = 0; k < movepath.size(); ++k) {
			if (movepath[k].first == row && movepath[k].second == col) {
				Rect pathrect = Rect(startleft + col * rectlen + 1, starttop + row * rectlen + 1, rectlen - 1, rectlen - 1);
				Scalar color;
				if (k == 0)
				{
					//·���滮�������ɫ
					color = Scalar(200, 240, 220);
				}
				else if (k == movepath.size() - 1) {
					//·���滮���յ���ɫ
					color = Scalar(200, 220, 250);
				}
				else {
					//·���滮��·����ɫ
					color = Scalar(255, 255, 266);
				}
				rectangle(src, pathrect, color, -1);
				break;
			}
		}

		//���ɶ�Ӧ�ľ��ο�
		Rect rect = Rect(startleft + col * rectlen, starttop + row * rectlen, rectlen, rectlen);
		rectangle(src, rect, Scalar::all(0));

		//д�϶�Ӧ����
		if (nummatrix[row][col] == 0) continue;
		string text = to_string(nummatrix[row][col]);
		putText(src, text, Point(rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 + 10),
			FONT_HERSHEY_DUPLEX, 1.0, Scalar::all(0));
	}
	imshow("CalcPath", src);
	//��ESC���˳���ԭ����
	if (waitKey(SleepTime) == 13) {
		waitKey(0);
	};
	
}

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
	if (col - 1 >= 0 && vts[row][col - 1] == 0) {
		vts[row][col - 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//2.�ұ�
	else if (col + 1 <= 3 && vts[row][col + 1] == 0) {
		vts[row][col + 1] = vts[row][col];
		vts[row][col] = 0;
	}
	//3.�ϱ�
	else if (row + 1 <= 3 && vts[row + 1][col] == 0) {
		vts[row + 1][col] = vts[row][col];
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

//��ԭ��Ϸ
void Puzzles4x4::RestoreGame(vector<vector<int>>& vts)
{
	double usetime = (double)getTickCount();
	cout << "AI�Զ���ԭ��ʱ��ʼ" << endl;
	//����ԭ����
	vector<vector<int>> bakvts(vts.size(), vector<int>(vts[0].size(), 0));
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			bakvts[row][col] = vts[row][col];
		}
	}

	vector<vector<int>> sites;
	//��ǰ�滮·����ÿһ�����ж�·��
	vector<pair<int, int>> currectpath;
	//��ʼ��ԭ
	while (!CheckFinished(bakvts)) {
		//1.�ȼ�⵱ǰ������Ҫ��ԭ��
		int step = CheckSteps(bakvts, sites);
		cout << "step:" << step << endl;
		//SleepTime = step > 8 ? 500 : 10;

		//2.����·��
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
			//��ȡ��ʼ�����ĵ�λ��
			pair<int, int> sPos = GetPos(bakvts, step);
			//��ȡӦ����λ��
			int row = (step - 1) / 4;
			int col = (step - 1) % 4;
			pair<int, int> ePos(row, col);
			RestorePath = FindPath(sites, sPos, ePos, DirectFirst::Up);

			DrawPuzzles(bakvts);
			//3.����·��ÿһ�������ƶ�·��
			for (int i = 0; i < RestorePath.size(); ++i) {
				cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
				//3.1���ֵ�ǰλ�ò�����
				int front = i - 1;
				if (front < 0) continue;

				//3.2 ����0����ǰ���·��
				pair<int, int> ZeroPos = GetPos(bakvts, 0);
				//cout << "ZeroPos:" << ZeroPos.first << " " << ZeroPos.second << endl;

				//3.2.1 ���0�ĵ���Ҫ�ƶ��ĵ��غϣ�ֱ�ӽ���λ��
				if (ZeroPos.first == RestorePath[i].first && ZeroPos.second == RestorePath[i].second)
				{
					SwapPos(bakvts, RestorePath[i], RestorePath[i - 1]);
					continue;
				}

				//3.3�����ֵ�ǰλ������Ϊ�ϰ��㣬����0�������뵱ǰ���ֽ���λ��
				//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
				sites[RestorePath[front].first][RestorePath[front].second] = 1;

				int stepnum = ZeroMove(bakvts, sites, RestorePath[i]);
				if (stepnum > 0) {
					//3.3.2 �ƶ���ɵ�ǰ������0��������λ��
					SwapPos(bakvts, RestorePath[i], RestorePath[front]);
					//3.3.3 ȡ����ǰ��ͼ�ϵ��ϰ���
					sites[RestorePath[front].first][RestorePath[front].second] = 0;
				}
				else {
					//���û��·����������������������е�������
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

	usetime = ((double)getTickCount() - usetime) / getTickFrequency();
	cout << "��ԭ�������£�" << endl;
	for (auto curstep : VetRestoreSteps) {
		cout << curstep.first.first << "," << curstep.first.second << " --> " << curstep.second.first << "," << curstep.second.second << endl;
	}

	cout << "���ƣ�" << VetRestoreSteps.size() << "��" << endl;
	cout << "AI�Զ���ԭ��ɣ���ʱ��" << usetime << "�룡" << endl;
}

