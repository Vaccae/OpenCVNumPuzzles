#include "ImgPuzzles.h"


//�����ʾͼ��
cv::Mat ImgPuzzles::TmpSrc = cv::Mat();
cv::Mat ImgPuzzles::PuzzleMat = cv::Mat();
int ImgPuzzles::SleepTime = 400;
bool ImgPuzzles::IsShowStep = true;
//���㻹ԭʱ��
double ImgPuzzles::PuzzleUseTime = (double)cv::getTickCount();
//��ǰ״̬ 0-δ��ʼ  1-ƴͼ��  2-�����
int ImgPuzzles::Status = 0;
//��ǰ��ƴͼ����
std::vector<std::vector<CutMat*>> ImgPuzzles::vtsCutMat = std::vector<std::vector<CutMat*>>();
//��ǰͼ����������
std::vector<std::vector<cv::Point>> ImgPuzzles::vtsContours = std::vector<std::vector<cv::Point>>();
//���½����һ��ͼ
CutMat* ImgPuzzles::finalCutMat = nullptr;
//�ָ�ͼ��Ŀ��
int ImgPuzzles::rectwidth = 0;
//�ָ�ͼ��ĸ߶�
int ImgPuzzles::rectheight = 0;

void ImgPuzzles::SplitMats(cv::Mat& img, int cols, int rows)
{

	if (cols == 0 || rows == 0)
	{
		std::cout << "��������������Ϊ0" << std::endl;
		return;
	}
	//����Դͼ��
	img.copyTo(TmpSrc);

	Status = 0;
	//�����к���ֱ�����������ĸ���
	vtsCutMat = std::vector<std::vector<CutMat*>>(rows, std::vector<CutMat*>(cols, nullptr));

	//����ƽ���ֵĸ�����width��height
	int width = img.cols / cols;
	int height = img.rows / rows;

	//��������б�
	std::vector<int> nums = GetVtsPos(cols, rows);

	//�������ֶԱȣ����ڼ�����Ե�����
	std::vector<int> reversenums;

	//����������к��л��ֿ�����
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			//���㵱ǰ���ε���ʼX��Y����
			int x = col * width;
			if (x > 0) x++;

			int y = row * height;
			if (y > 0) y++;

			//�����ȡ���εĿ�͸ߣ�������Ʋ��ܳ���Դͼ��ı߽�
			int rwidth = width;
			if (x + rwidth > img.cols) rwidth = img.cols - x;
			int rheight = height;
			if (y + rheight > img.rows) rheight = img.rows - y;

			//���ɽ�ȡ�ľ��β���ȡͼ���ŵ�map��
			cv::Rect rect = cv::Rect(x, y, rwidth, rheight);
			cv::Mat matrect = img(rect);

			//��ȡ���ͼ����Ҫ�ж��Ƿ���һ�£���һ��ʱ����Ϊһ����������һ��ͼ����ʾ
			if (rwidth != width || rheight != height) {
				cv::resize(matrect, matrect, cv::Size(width, height));
			}

			//��ǰMat�����
			int pos = row * rows + col + 1;
			CutMat* tmpcurmat = new CutMat(pos, matrect);
			//���ָ������λ��
			tmpcurmat->curposition = GetRandNum(nums);
			int newrow = (tmpcurmat->curposition - 1) / cols;
			int newcol = (tmpcurmat->curposition - 1) % cols;

			//�������������λ�ò��뵽������
			vtsCutMat[newrow][newcol] = tmpcurmat;

			//��������Լ���������
			reversenums.push_back(tmpcurmat->curposition);
		}
	}

	//���ɵ����������У����һ���ǹ̶��ģ�����ȥ�����һ������
	reversenums.pop_back();
	//���������ɵ�����ԣ���ֹ�Ź����޽�
	int count = CalcReverseNum::MergeSort(reversenums, 0, reversenums.size() - 1);
	cout << "���μ���:" << count << endl;

	//��������Ը����������ż�����ɻ�ԭ
	if ((count % 2) != 0) {
		//���������˵���޽⣬��ԾŹ��񽫵����е�һ�к͵ڶ��е�һ�н����滻
		finalCutMat = vtsCutMat[vtsCutMat.size() - 1][0];

		vtsCutMat[vtsCutMat.size() - 1][0] = vtsCutMat[vtsCutMat.size() - 2][0];
		int tmpcurposition = vtsCutMat[vtsCutMat.size() - 1][0]->curposition;
		vtsCutMat[vtsCutMat.size() - 1][0]->curposition = finalCutMat->curposition;

		vtsCutMat[vtsCutMat.size() - 2][0] = finalCutMat;
		vtsCutMat[vtsCutMat.size() - 2][0]->curposition = tmpcurposition;
	}
	//for (int row = 0; row < vtsCutMat.size(); ++row) {
	//	for (int col = 0; col < vtsCutMat[row].size(); ++col) {
	//		std::cout << " " << vtsCutMat[row][col]->curposition << std::endl;
	//	}
	//}
}

void ImgPuzzles::CreatePuzzleMat()
{
	if (vtsCutMat.empty()) return;

	int starttop = 20;
	int startleft = 20;
	//����ͼ����
	if (rectwidth == 0 && vtsCutMat[0][0] != nullptr) {
		rectwidth = vtsCutMat[0][0]->mat.cols;
		rectheight = vtsCutMat[0][0]->mat.rows;
	}

	vtsContours.clear();


	//����ͼ��
	PuzzleMat = cv::Mat(cv::Size(600, 700), CV_8UC3, cv::Scalar(240, 240, 240));

	for (int row = 0; row < vtsCutMat.size(); ++row) {
		for (int col = 0; col < vtsCutMat[row].size(); ++col) {
			//���ɶ�Ӧ�ľ��ο�
			cv::Rect rect = cv::Rect(startleft + col * rectwidth, starttop + row * rectheight, rectwidth, rectheight);
			//�滻��Ӧλ�õ�ͼ��
			if (vtsCutMat[row][col] != nullptr) {
				vtsCutMat[row][col]->mat.copyTo(PuzzleMat(rect));

				//����ֵ
				InsertContours(rect);
			}
		}
	}

	//��������������½ǵ��·�������һ���հ״�����Ϊ���ƶ���λ��
	cv::Rect rect = cv::Rect(startleft + (vtsCutMat[0].size() - 1) * rectwidth, starttop + (vtsCutMat[0].size()) * rectheight, rectwidth, rectheight);
	cv::Mat(cv::Size(rectwidth, rectheight), CV_8UC3, cv::Scalar(255, 255, 255))
		.copyTo(PuzzleMat(rect));
	//����ֵ
	InsertContours(rect);


	cv::imshow("puzzle", PuzzleMat);

	//���ص��¼�
	cv::setMouseCallback("puzzle", OnMouseEvent, 0);
}

//��ʾƴͼ��Ϸͼ��
void ImgPuzzles::DrawPuzzleMat(int& curposition, int& newposition)
{
	//��ͼ��������ŵ���ʱMat��
	std::vector<cv::Point> curpts = vtsContours[curposition - 1];
	cv::Rect currect = cv::Rect(curpts[0], curpts[2]);
	cv::Mat curmat = PuzzleMat(currect);
	cv::Mat tmpmat;
	curmat.copyTo(tmpmat);

	//������ֱ���滻ԭ����
	std::vector<cv::Point> newpts = vtsContours[newposition - 1];
	cv::Rect newrect = cv::Rect(newpts[0], newpts[2]);
	cv::Mat newmat = PuzzleMat(newrect);

	//�滻�¾�����
	newmat.copyTo(PuzzleMat(currect));
	tmpmat.copyTo(PuzzleMat(newrect));

	cv::imshow("puzzle", PuzzleMat);
}

//AI�Զ���ԭ
void ImgPuzzles::RestoreGame()
{
	double usetime = (double)cv::getTickCount();
	cout << "AI�Զ���ԭ��ʱ��ʼ" << endl;


	vector<vector<int>> sites;
	//��ǰ�滮·����ÿһ�����ж�·��
	vector<pair<int, int>> currectpath;
	//��ʼ��ԭ
	while (!CheckFinished(vtsCutMat)) {
		if (Status == 0) {
			int row = 2;
			int col = 2;
			int curposition = vtsCutMat[row][col]->curposition;
			int newposition = -1;
			if (ImageMove(row, col, curposition, newposition)) {
				DrawPuzzleMat(curposition, newposition);
				Status = 1;
				cv::waitKey(SleepTime);
			}

			continue;
		}
		//1.�ȼ�⵱ǰ������Ҫ��ԭ��
		int step = CheckSteps(vtsCutMat, sites);
		cout << "step:" << step << endl;

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
		{
			//��ȡ��ʼ�����ĵ�λ��
			pair<int, int> sPos;
			pair<int, int> ePos;
			GetPos(vtsCutMat, step, sPos, ePos);
			//��ȡӦ����λ��
			RestorePath = FindPath(sites, sPos, ePos, DirectFirst::Up);

			//3.����·��ÿһ�������ƶ�·��
			for (int i = 0; i < RestorePath.size(); ++i) {
				cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
				//3.1���ֵ�ǰλ�ò�����
				int front = i - 1;
				if (front < 0) continue;

				//3.3�����ֵ�ǰλ������Ϊ�ϰ��㣬����0�������뵱ǰ���ֽ���λ��
				//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
				sites[RestorePath[front].first][RestorePath[front].second] = 1;

				int stepnum = NullMove(sites, RestorePath[i]);
				if (stepnum != 0) {
					//3.3.3 ȡ����ǰ��ͼ�ϵ��ϰ���
					sites[RestorePath[front].first][RestorePath[front].second] = 0;

					int row = RestorePath[front].first;
					int col = RestorePath[front].second;
					int curposition = vtsCutMat[row][col]->curposition;
					int newposition = -1;

					if (ImageMove(row, col, curposition, newposition)) {
						DrawPuzzleMat(curposition, newposition);
						cv::waitKey(SleepTime);
					}
				}
				else {
					std::cout << "dealstep:" << step << std::endl;
					//���û��·����������������������е�������
					DealStep(sites, step);
				}
			}
		}
		break;
		case RestoreStep::Step4and5:
		case RestoreStep::StepFinal:
		{
			DealStep(sites, step);
		}
		break;
		case RestoreStep::Num9:
		{
			int row = 99;
			int col = 99;
			int curposition = vtsContours.size();
			int newposition = -1;

			if (ImageMove(row, col, curposition, newposition)) {
				//��������ͼ����ʾ
				DrawPuzzleMat(curposition, newposition);
				cv::waitKey(SleepTime);
			}
		}
		default:
			break;
		}
	}

	usetime = ((double)cv::getTickCount() - usetime) / cv::getTickFrequency();
	cout << "AI�Զ���ԭ��ɣ���ʱ��" << usetime << "�룡" << endl;

}

//�����������
std::vector<int> ImgPuzzles::GetVtsPos(int cols, int rows)
{
	std::vector<int> nums;
	int total = cols * rows;
	if (total > 0) {
		for (int i = 1; i <= total; ++i) {
			nums.push_back(i);
		}
	}
	return nums;
}

//��ȡ��ǰ������
int ImgPuzzles::GetRandNum(std::vector<int>& nums)
{
	//��ʼ�����������
	srand((int)time(0));
	//���½ǵ����һ����ֵ���ԣ�����ȡ����ʱ����������
	int index = nums.size() == 1 ? 0 : rand() % (nums.size() - 1);
	//��ȡ������ֵ
	int resint = nums[index];
	//������ɾ���Ѿ���ֵ������
	nums.erase(nums.begin() + index);
	return resint;
}

void ImgPuzzles::InsertContours(cv::Rect rect)
{
	std::vector<cv::Point> vetpt;
	vetpt.push_back(cv::Point(rect.x, rect.y));
	vetpt.push_back(cv::Point(rect.x + rect.width, rect.y));
	vetpt.push_back(cv::Point(rect.x + rect.width, rect.y + rect.height));
	vetpt.push_back(cv::Point(rect.x, rect.y + rect.height));

	vtsContours.push_back(vetpt);
}

void ImgPuzzles::OnMouseEvent(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case cv::MouseEventTypes::EVENT_LBUTTONUP:
	{
		cv::Point point = cv::Point(x, y);
		int col = -1;
		int row = -1;
		int curposition = -1;
		int newposition = -1;
		GetMousePostion(point, row, col, curposition);
		if (col >= 0 && row >= 0) {
			if (ImageMove(row, col, curposition, newposition)) {
				if (Status == 0) {
					//����״̬Ϊ��ʼƴͼ
					Status = 1;
					PuzzleUseTime = (double)cv::getTickCount();
					std::cout << "��ʼ��ԭ��ʱ" << std::endl;
				}
				else if (Status == 1 && finalCutMat == nullptr) {
					Status = 0;
				}
				else if (Status == 2) {
					PuzzleUseTime = ((double)cv::getTickCount() - PuzzleUseTime) / cv::getTickFrequency();
					std::cout << "��ԭ��ɣ���ʱ��" << PuzzleUseTime << "�룡" << std::endl;
				}
				//��������ͼ����ʾ
				DrawPuzzleMat(curposition, newposition);
			}
		}

	}
	break;
	case cv::MouseEventTypes::EVENT_MBUTTONUP:
	{
		//�Զ���ԭ
		RestoreGame();
	}
	break;
	case cv::MouseEventTypes::EVENT_RBUTTONDBLCLK:
	{
		//��ȡͼ��ָ��ļ���
		SplitMats(TmpSrc);

		//����ͼ��
		CreatePuzzleMat();

		Status = 0;
	}
	break;
	default:
		break;
	}
}

bool ImgPuzzles::ImageMove(int& row, int& col, int& curposition, int& newposition)
{
	bool res = true;
	int cols = vtsCutMat[0].size();
	if (Status == 0) {
		if (vtsCutMat.size() - 1 != row || vtsCutMat[vtsCutMat.size() - 1].size() - 1 != col) {
			std::cout << "�����ƶ����¸�ʼ" << std::endl;
			res = false;
		}
		else {
			//�ƿ������½ǵ�ͼƬ
			finalCutMat = vtsCutMat[row][col];
			newposition = vtsContours.size();
			finalCutMat->curposition = newposition;
			vtsCutMat[row][col] = nullptr;
		}
	}
	else if (Status == 1) {
		//������ƶ�������
		//0.�ж��ǲ��ǽ���λ��
		if (row == 99 && col == 99) {
			int tmprow = vtsCutMat.size() - 1;
			int tmpcol = vtsCutMat[tmprow].size() - 1;
			if (vtsCutMat[tmprow][tmpcol] == nullptr) {
				//�ƿ������½ǵ�ͼƬ
				newposition = tmprow * cols + tmpcol + 1;
				finalCutMat->curposition = newposition;
				vtsCutMat[tmprow][tmpcol] = finalCutMat;
				finalCutMat = nullptr;
			}
			else
			{
				res = false;
			}
		}
		//1.���
		else if (col - 1 >= 0 && vtsCutMat[row][col - 1] == nullptr) {
			newposition = row * cols + col;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row][col - 1] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;

		}
		//2.�ұ�
		else if (col + 1 <= vtsCutMat.size() - 1 && vtsCutMat[row][col + 1] == nullptr) {
			newposition = row * cols + col + 2;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row][col + 1] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;
		}
		//3.�ϱ�
		else if (row - 1 >= 0 && vtsCutMat[row - 1][col] == nullptr) {
			newposition = (row - 1) * cols + col + 1;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row - 1][col] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;
		}
		//4.�±�
		else if (row + 1 <= vtsCutMat.size() - 1 && vtsCutMat[row + 1][col] == nullptr) {
			newposition = (row + 1) * cols + col + 1;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row + 1][col] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;
		}
		else {
			res = false;
		}
	}

	//std::cout << "row:" << row << " col:" << col << " cur:" << curposition << " new:" << newposition << std::endl;
	return res;
}


//��ȡ���������������
void ImgPuzzles::GetMousePostion(cv::Point pt, int& row, int& col, int& curposition)
{
	for (int i = 0; i < vtsContours.size(); ++i) {
		//������������ڣ���������һ������
		if (pointPolygonTest(vtsContours[i], pt, true) > 0) {
			//�ж��ǲ������һ�������ʱ�ĸ�����Ƿ���-99
			if (i == vtsContours.size() - 1) {
				row = 99;
				col = 99;
				curposition = vtsContours.size();
			}
			else {
				//���㵱ǰ˳��Ϊ��ά����ļ��м���
				row = i / vtsCutMat[0].size();
				col = i % vtsCutMat[0].size();
				curposition = i + 1;
				break;
			}
		}
	}
}

int ImgPuzzles::CheckSteps(std::vector<std::vector<CutMat*>>& vts, std::vector<std::vector<int>>& sites)
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
			case RestoreStep::Num6:
			case RestoreStep::Num7:
			case RestoreStep::Num8:
			{
				if (vts[row][col] != nullptr && vts[row][col]->position == checknum) {
					sites[row][col] = 1;
					checknum++;
				}
				else {
					return checknum;
				}
			}
			break;
			case RestoreStep::Num4:
			case RestoreStep::Num5:
			{
				if (vts[2][0] != nullptr && vts[1][0] != nullptr && vts[2][0]->position == 4 && vts[1][0]->position == 5 ){
					if (vts[1][2] != nullptr && vts[1][2]->position != 6)
					{
						sites[2][0] = 1;
						sites[1][0] = 1;
						return RestoreStep::Num6;
					}
					else if (vts[1][2] == nullptr) {
						sites[2][0] = 1;
						sites[1][0] = 1;
						return RestoreStep::Num6;
					}
					else {
						return RestoreStep::Step4and5;
					}
				}
				else if (vts[row][col] != nullptr && vts[row][col]->position == checknum) {
					sites[row][col] = 1;
					checknum++;
				}
				else {
					return checknum;
				}
			}
			break;
			case RestoreStep::Num9:
			{
				if (vts[row][col] == nullptr) {
					return checknum;
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

bool ImgPuzzles::CheckFinished(std::vector<std::vector<CutMat*>>& vts)
{
	//����������
	int rows = vts.size() - 1;
	//����������
	int cols = vts[rows].size() - 1;

	int checknum = 1;
	for (int row = 0; row <= rows; ++row) {
		for (int col = 0; col <= cols; ++col) {
			//���һ���Ѿ���⣬ֱ���˳�
			//if (col == cols && row == rows) return true;
			if (vts[row][col] == nullptr) return false;
			if (vts[row][col]->position != checknum) return false;
			checknum++;
			if (checknum > 9) {
				return true;
			}
		}
	}
	return false;
}

void ImgPuzzles::GetPos(std::vector<std::vector<CutMat*>>& vts, int num, std::pair<int, int>& startpos, std::pair<int, int>& endpos)
{
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			if (vts[row][col] == nullptr) continue;
			if (vts[row][col]->position == num) {
				startpos.first = row;
				startpos.second = col;

				int erow = (vts[row][col]->position - 1) / vts[row].size();
				int ecol = (vts[row][col]->position - 1) % vts[row].size();
				endpos.first = erow;
				endpos.second = ecol;

				break;
			}
		}
	}
}

std::pair<int, int> ImgPuzzles::GetNullPos(std::vector<std::vector<CutMat*>>& vts)
{
	pair<int, int> pos;
	for (int row = 0; row <= vts.size() - 1; ++row) {
		for (int col = 0; col <= vts[row].size() - 1; ++col) {
			if (vts[row][col] == nullptr) {
				pos.first = row;
				pos.second = col;
				break;
			}
		}
	}
	return pos;
}

int ImgPuzzles::NullMove(vector<vector<int>>& sites, pair<int, int>& endPos, int MoveDirect)
{
	pair<int, int> ZeroPos = GetNullPos(vtsCutMat);
	vector<pair<int, int>> zeropath;

	if (ZeroPos.first == endPos.first && ZeroPos.second == endPos.second) {
		return -1;
	}
	zeropath = FindPath(sites, ZeroPos, endPos, MoveDirect);

	//3.3.1 ��ʼ�ƶ�0����ǰλ��,�ӵ�һ����ʼ����Ϊ��㲻��
	for (int k = 1; k < zeropath.size(); ++k) {
		int row = zeropath[k].first;
		int col = zeropath[k].second;
		int curposition = vtsCutMat[row][col]->curposition;
		int newposition = -1;

		if (ImageMove(row, col, curposition, newposition)) {
			//��������ͼ����ʾ
			DrawPuzzleMat(curposition, newposition);
			cv::waitKey(SleepTime);
		}
	}


	return zeropath.size();
}

void ImgPuzzles::DealStep(vector<vector<int>>& sites, int step)
{
	int row = 0;
	pair<int, int> sPos;
	pair<int, int> endPos;
	switch (step)
	{
	case RestoreStep::Num3:
		row = step / sites.size() - 1;
		//1.�Ƚ�0�ƶ�����ǰҪ������е������
		endPos = std::pair<int, int>(row + 1, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.����������ǰ����ϰ��㣬��0��λ�������ƶ��������
		for (int i = 0; i < sites[row].size(); ++i) {
			sites[row][i] = 0;
		}
		endPos.first = row;
		endPos.second = 2;
		NullMove(sites, endPos, DirectFirst::Up);

		//3.����0�ٺ͵�ǰҪ����ĵ����λ�û�����������3λ���ƶ�����Ӧ������
		endPos.first = row + 1;
		endPos.second = 2;
		//��ֹ�ƶ����������ģ�����Ϊ���ƶ�
		sites[endPos.first][endPos.second] = 0;
		NullMove(sites, endPos, DirectFirst::Right);
		//����Ϊ�����ϰ���
		sites[row][2] = 1;

		//4.������0�ƶ����ڶ���λ�ú�ԭ��ǰ��ǰ��������
		endPos.first = row;
		endPos.second = 1;
		NullMove(sites, endPos);

		//5.��0���Ȱ����Ƶķ�ʽ��ԭ����ǰ������ֻ�ԭ����
		endPos.first = row + 1;
		endPos.second = 0;
		NullMove(sites, endPos, DirectFirst::Left);
		break;
	case RestoreStep::Num5:
		sites[1][0] = 0;

		//��ȡ��ʼ�����ĵ�λ��

		GetPos(vtsCutMat, step, sPos, endPos);
		endPos.first = 1;
		endPos.second = 2;
		//��ȡӦ����λ��
		RestorePath = FindPath(sites, sPos, endPos, DirectFirst::Up);

		//3.����·��ÿһ�������ƶ�·��
		for (int i = 0; i < RestorePath.size(); ++i) {
			cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
			//3.1���ֵ�ǰλ�ò�����
			int front = i - 1;
			if (front < 0) continue;

			//3.3�����ֵ�ǰλ������Ϊ�ϰ��㣬����0�������뵱ǰ���ֽ���λ��
			//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
			sites[RestorePath[front].first][RestorePath[front].second] = 1;

			int stepnum = NullMove(sites, RestorePath[i]);
			if (stepnum != 0) {
				//3.3.3 ȡ����ǰ��ͼ�ϵ��ϰ���
				sites[RestorePath[front].first][RestorePath[front].second] = 0;

				int row = RestorePath[front].first;
				int col = RestorePath[front].second;
				int curposition = vtsCutMat[row][col]->curposition;
				int newposition = -1;

				if (ImageMove(row, col, curposition, newposition)) {
					DrawPuzzleMat(curposition, newposition);
					cv::waitKey(SleepTime);
				}
			}
			else {

				//���û��·����������������������е�������
				DealStep(sites, step);
			}
		}
		break;
	case RestoreStep::Num6:
		row = step / sites.size() - 1;
		//1.�Ƚ�0�ƶ�����ǰҪ������е������
		endPos = std::pair<int, int>(row + 1, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.����������ǰ����ϰ��㣬��0��λ�������ƶ��������
		for (int i = 0; i < sites[row].size(); ++i) {
			sites[row][i] = 0;
		}
		endPos.first = row;
		endPos.second = 1;
		NullMove(sites, endPos, DirectFirst::Up);

		//3.��4��5����
		sites[row][0] = 1;
		sites[row + 1][0] = 1;
		sites[row][2] = 0;
		break;
	case RestoreStep::Step4and5:
		//1.���հ��ƶ���5��λ��
		endPos = std::pair<int, int>(1, 1);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.����4��5����ԭ��ȥ
		sites[1][0] = 0;
		sites[2][0] = 0;
		pair<int, int> endPos(2, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//3.����4��5
		sites[1][0] = 1;
		sites[1][1] = 1;
		break;
	}


}

//�����ж�·��
std::vector<std::pair<int, int>> ImgPuzzles::FindPath(std::vector<std::vector<int>>& sites, std::pair<int, int>& startpos, std::pair<int, int>& endpos, int directfirst)
{
	CalcPathPlan plan = CalcPathPlan();
	plan.DirectFirst = directfirst;
	plan.InitSites(sites);

	return plan.GetPath(startpos, endpos);
}

