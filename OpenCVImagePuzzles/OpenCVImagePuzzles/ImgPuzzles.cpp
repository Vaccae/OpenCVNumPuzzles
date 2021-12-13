#include "ImgPuzzles.h"


//输出显示图像
cv::Mat ImgPuzzles::TmpSrc = cv::Mat();
cv::Mat ImgPuzzles::PuzzleMat = cv::Mat();
int ImgPuzzles::SleepTime = 400;
bool ImgPuzzles::IsShowStep = true;
//计算还原时间
double ImgPuzzles::PuzzleUseTime = (double)cv::getTickCount();
//当前状态 0-未开始  1-拼图中  2-已完成
int ImgPuzzles::Status = 0;
//当前的拼图容器
std::vector<std::vector<CutMat*>> ImgPuzzles::vtsCutMat = std::vector<std::vector<CutMat*>>();
//当前图像轮廓容器
std::vector<std::vector<cv::Point>> ImgPuzzles::vtsContours = std::vector<std::vector<cv::Point>>();
//右下角最后一张图
CutMat* ImgPuzzles::finalCutMat = nullptr;
//分割图像的宽度
int ImgPuzzles::rectwidth = 0;
//分割图像的高度
int ImgPuzzles::rectheight = 0;

void ImgPuzzles::SplitMats(cv::Mat& img, int cols, int rows)
{

	if (cols == 0 || rows == 0)
	{
		std::cout << "行数和列数不能为0" << std::endl;
		return;
	}
	//复制源图像
	img.copyTo(TmpSrc);

	Status = 0;
	//根据行和列直接设置容器的个数
	vtsCutMat = std::vector<std::vector<CutMat*>>(rows, std::vector<CutMat*>(cols, nullptr));

	//计算平均分的格数的width和height
	int width = img.cols / cols;
	int height = img.rows / rows;

	//生成序号列表
	std::vector<int> nums = GetVtsPos(cols, rows);

	//加入数字对比，用于计算逆对的问题
	std::vector<int> reversenums;

	//根据输入的行和列划分开矩形
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			//计算当前矩形的起始X和Y坐标
			int x = col * width;
			if (x > 0) x++;

			int y = row * height;
			if (y > 0) y++;

			//计算截取矩形的宽和高，加入控制不能超过源图像的边界
			int rwidth = width;
			if (x + rwidth > img.cols) rwidth = img.cols - x;
			int rheight = height;
			if (y + rheight > img.rows) rheight = img.rows - y;

			//生成截取的矩形并截取图像存放到map中
			cv::Rect rect = cv::Rect(x, y, rwidth, rheight);
			cv::Mat matrect = img(rect);

			//截取后的图像需要判断是否宽高一致，不一致时缩放为一样大，用于在一张图像显示
			if (rwidth != width || rheight != height) {
				cv::resize(matrect, matrect, cv::Size(width, height));
			}

			//当前Mat的序号
			int pos = row * rows + col + 1;
			CutMat* tmpcurmat = new CutMat(pos, matrect);
			//随机指定的新位置
			tmpcurmat->curposition = GetRandNum(nums);
			int newrow = (tmpcurmat->curposition - 1) / cols;
			int newcol = (tmpcurmat->curposition - 1) % cols;

			//根据随机排序后的位置插入到容器中
			vtsCutMat[newrow][newcol] = tmpcurmat;

			//插入逆序对计算容器中
			reversenums.push_back(tmpcurmat->curposition);
		}
	}

	//生成的随机逆序对中，最后一个是固定的，所以去掉最后一个计算
	reversenums.pop_back();
	//检测随机生成的逆序对，防止九宫格无解
	int count = CalcReverseNum::MergeSort(reversenums, 0, reversenums.size() - 1);
	cout << "分治计算:" << count << endl;

	//计算逆序对个数，如果是偶数即可还原
	if ((count % 2) != 0) {
		//如果是奇数说明无解，针对九宫格将第三行第一列和第二行第一列进行替换
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
	//设置图像宽度
	if (rectwidth == 0 && vtsCutMat[0][0] != nullptr) {
		rectwidth = vtsCutMat[0][0]->mat.cols;
		rectheight = vtsCutMat[0][0]->mat.rows;
	}

	vtsContours.clear();


	//创建图像
	PuzzleMat = cv::Mat(cv::Size(600, 700), CV_8UC3, cv::Scalar(240, 240, 240));

	for (int row = 0; row < vtsCutMat.size(); ++row) {
		for (int col = 0; col < vtsCutMat[row].size(); ++col) {
			//生成对应的矩形框
			cv::Rect rect = cv::Rect(startleft + col * rectwidth, starttop + row * rectheight, rectwidth, rectheight);
			//替换对应位置的图像
			if (vtsCutMat[row][col] != nullptr) {
				vtsCutMat[row][col]->mat.copyTo(PuzzleMat(rect));

				//区域赋值
				InsertContours(rect);
			}
		}
	}

	//创建布局完后，右下角的下方再设置一个空白处，做为可移动的位置
	cv::Rect rect = cv::Rect(startleft + (vtsCutMat[0].size() - 1) * rectwidth, starttop + (vtsCutMat[0].size()) * rectheight, rectwidth, rectheight);
	cv::Mat(cv::Size(rectwidth, rectheight), CV_8UC3, cv::Scalar(255, 255, 255))
		.copyTo(PuzzleMat(rect));
	//区域赋值
	InsertContours(rect);


	cv::imshow("puzzle", PuzzleMat);

	//鼠标回调事件
	cv::setMouseCallback("puzzle", OnMouseEvent, 0);
}

//显示拼图游戏图像
void ImgPuzzles::DrawPuzzleMat(int& curposition, int& newposition)
{
	//将图像旧区域存放到临时Mat中
	std::vector<cv::Point> curpts = vtsContours[curposition - 1];
	cv::Rect currect = cv::Rect(curpts[0], curpts[2]);
	cv::Mat curmat = PuzzleMat(currect);
	cv::Mat tmpmat;
	curmat.copyTo(tmpmat);

	//新区域直接替换原区域
	std::vector<cv::Point> newpts = vtsContours[newposition - 1];
	cv::Rect newrect = cv::Rect(newpts[0], newpts[2]);
	cv::Mat newmat = PuzzleMat(newrect);

	//替换新旧区域
	newmat.copyTo(PuzzleMat(currect));
	tmpmat.copyTo(PuzzleMat(newrect));

	cv::imshow("puzzle", PuzzleMat);
}

//AI自动还原
void ImgPuzzles::RestoreGame()
{
	double usetime = (double)cv::getTickCount();
	cout << "AI自动还原计时开始" << endl;


	vector<vector<int>> sites;
	//当前规划路线中每一步的行动路线
	vector<pair<int, int>> currectpath;
	//开始还原
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
		//1.先检测当前棋盘需要还原的
		int step = CheckSteps(vtsCutMat, sites);
		cout << "step:" << step << endl;

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
		{
			//获取开始结束的点位置
			pair<int, int> sPos;
			pair<int, int> ePos;
			GetPos(vtsCutMat, step, sPos, ePos);
			//获取应到达位置
			RestorePath = FindPath(sites, sPos, ePos, DirectFirst::Up);

			//3.遍历路径每一步处理移动路径
			for (int i = 0; i < RestorePath.size(); ++i) {
				cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
				//3.1数字当前位置不计算
				int front = i - 1;
				if (front < 0) continue;

				//3.3把数字当前位置设置为障碍点，这样0不允许与当前数字交换位置
				//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
				sites[RestorePath[front].first][RestorePath[front].second] = 1;

				int stepnum = NullMove(sites, RestorePath[i]);
				if (stepnum != 0) {
					//3.3.3 取消当前地图上的障碍点
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
					//如果没有路径就是遇到特殊情况，进行单独处理
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
				//重新生成图像显示
				DrawPuzzleMat(curposition, newposition);
				cv::waitKey(SleepTime);
			}
		}
		default:
			break;
		}
	}

	usetime = ((double)cv::getTickCount() - usetime) / cv::getTickFrequency();
	cout << "AI自动还原完成！用时：" << usetime << "秒！" << endl;

}

//生成序号容器
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

//获取当前随机序号
int ImgPuzzles::GetRandNum(std::vector<int>& nums)
{
	//初始化随机数种子
	srand((int)time(0));
	//右下角的最后一个赋值不对，所以取余数时不计算在内
	int index = nums.size() == 1 ? 0 : rand() % (nums.size() - 1);
	//获取到返回值
	int resint = nums[index];
	//容器中删除已经赋值的数字
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
					//设置状态为开始拼图
					Status = 1;
					PuzzleUseTime = (double)cv::getTickCount();
					std::cout << "开始还原计时" << std::endl;
				}
				else if (Status == 1 && finalCutMat == nullptr) {
					Status = 0;
				}
				else if (Status == 2) {
					PuzzleUseTime = ((double)cv::getTickCount() - PuzzleUseTime) / cv::getTickFrequency();
					std::cout << "还原完成！用时：" << PuzzleUseTime << "秒！" << std::endl;
				}
				//重新生成图像显示
				DrawPuzzleMat(curposition, newposition);
			}
		}

	}
	break;
	case cv::MouseEventTypes::EVENT_MBUTTONUP:
	{
		//自动还原
		RestoreGame();
	}
	break;
	case cv::MouseEventTypes::EVENT_RBUTTONDBLCLK:
	{
		//获取图像分割后的集合
		SplitMats(TmpSrc);

		//绘制图像
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
			std::cout << "请先移动右下格开始" << std::endl;
			res = false;
		}
		else {
			//移开最右下角的图片
			finalCutMat = vtsCutMat[row][col];
			newposition = vtsContours.size();
			finalCutMat->curposition = newposition;
			vtsCutMat[row][col] = nullptr;
		}
	}
	else if (Status == 1) {
		//计算可移动的区域
		//0.判断是不是结束位置
		if (row == 99 && col == 99) {
			int tmprow = vtsCutMat.size() - 1;
			int tmpcol = vtsCutMat[tmprow].size() - 1;
			if (vtsCutMat[tmprow][tmpcol] == nullptr) {
				//移开最右下角的图片
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
		//1.左边
		else if (col - 1 >= 0 && vtsCutMat[row][col - 1] == nullptr) {
			newposition = row * cols + col;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row][col - 1] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;

		}
		//2.右边
		else if (col + 1 <= vtsCutMat.size() - 1 && vtsCutMat[row][col + 1] == nullptr) {
			newposition = row * cols + col + 2;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row][col + 1] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;
		}
		//3.上边
		else if (row - 1 >= 0 && vtsCutMat[row - 1][col] == nullptr) {
			newposition = (row - 1) * cols + col + 1;
			vtsCutMat[row][col]->curposition = newposition;
			vtsCutMat[row - 1][col] = vtsCutMat[row][col];
			vtsCutMat[row][col] = nullptr;
		}
		//4.下边
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


//获取点击的行数和列数
void ImgPuzzles::GetMousePostion(cv::Point pt, int& row, int& col, int& curposition)
{
	for (int i = 0; i < vtsContours.size(); ++i) {
		//如果点在轮廓内，计算是哪一个区域
		if (pointPolygonTest(vtsContours[i], pt, true) > 0) {
			//判断是不是最后一个存放临时的格，如果是返回-99
			if (i == vtsContours.size() - 1) {
				row = 99;
				col = 99;
				curposition = vtsContours.size();
			}
			else {
				//计算当前顺序为二维数组的几行几列
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
	//计算总行数
	int rows = vts.size() - 1;
	//计算总列数
	int cols = vts[rows].size() - 1;

	int checknum = 1;
	for (int row = 0; row <= rows; ++row) {
		for (int col = 0; col <= cols; ++col) {
			//最后一格已经检测，直接退出
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

	//3.3.1 开始移动0到当前位置,从第一步开始，因为起点不动
	for (int k = 1; k < zeropath.size(); ++k) {
		int row = zeropath[k].first;
		int col = zeropath[k].second;
		int curposition = vtsCutMat[row][col]->curposition;
		int newposition = -1;

		if (ImageMove(row, col, curposition, newposition)) {
			//重新生成图像显示
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
		//1.先将0移动到当前要处理的行的下面格
		endPos = std::pair<int, int>(row + 1, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.解锁处理行前面的障碍点，用0的位置优先移动到计算点
		for (int i = 0; i < sites[row].size(); ++i) {
			sites[row][i] = 0;
		}
		endPos.first = row;
		endPos.second = 2;
		NullMove(sites, endPos, DirectFirst::Up);

		//3.数字0再和当前要处理的点进行位置互换，将我们3位置移动到对应后锁定
		endPos.first = row + 1;
		endPos.second = 2;
		//防止移动点是锁定的，将改为可移动
		sites[endPos.first][endPos.second] = 0;
		NullMove(sites, endPos, DirectFirst::Right);
		//设置为锁定障碍点
		sites[row][2] = 1;

		//4.将数字0移动到第二步位置后还原当前行前三个数字
		endPos.first = row;
		endPos.second = 1;
		NullMove(sites, endPos);

		//5.将0优先按左移的方式把原来行前面的数字还原回来
		endPos.first = row + 1;
		endPos.second = 0;
		NullMove(sites, endPos, DirectFirst::Left);
		break;
	case RestoreStep::Num5:
		sites[1][0] = 0;

		//获取开始结束的点位置

		GetPos(vtsCutMat, step, sPos, endPos);
		endPos.first = 1;
		endPos.second = 2;
		//获取应到达位置
		RestorePath = FindPath(sites, sPos, endPos, DirectFirst::Up);

		//3.遍历路径每一步处理移动路径
		for (int i = 0; i < RestorePath.size(); ++i) {
			cout << RestorePath[i].first << " " << RestorePath[i].second << endl;
			//3.1数字当前位置不计算
			int front = i - 1;
			if (front < 0) continue;

			//3.3把数字当前位置设置为障碍点，这样0不允许与当前数字交换位置
			//cout << "sites:" << restorepath[front].first << " " << restorepath[front].second << endl;
			sites[RestorePath[front].first][RestorePath[front].second] = 1;

			int stepnum = NullMove(sites, RestorePath[i]);
			if (stepnum != 0) {
				//3.3.3 取消当前地图上的障碍点
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

				//如果没有路径就是遇到特殊情况，进行单独处理
				DealStep(sites, step);
			}
		}
		break;
	case RestoreStep::Num6:
		row = step / sites.size() - 1;
		//1.先将0移动到当前要处理的行的下面格
		endPos = std::pair<int, int>(row + 1, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.解锁处理行前面的障碍点，用0的位置优先移动到计算点
		for (int i = 0; i < sites[row].size(); ++i) {
			sites[row][i] = 0;
		}
		endPos.first = row;
		endPos.second = 1;
		NullMove(sites, endPos, DirectFirst::Up);

		//3.将4和5锁定
		sites[row][0] = 1;
		sites[row + 1][0] = 1;
		sites[row][2] = 0;
		break;
	case RestoreStep::Step4and5:
		//1.将空白移动到5的位置
		endPos = std::pair<int, int>(1, 1);
		NullMove(sites, endPos, DirectFirst::Left);

		//2.解锁4和5并还原回去
		sites[1][0] = 0;
		sites[2][0] = 0;
		pair<int, int> endPos(2, 0);
		NullMove(sites, endPos, DirectFirst::Left);

		//3.锁定4和5
		sites[1][0] = 1;
		sites[1][1] = 1;
		break;
	}


}

//查找行动路径
std::vector<std::pair<int, int>> ImgPuzzles::FindPath(std::vector<std::vector<int>>& sites, std::pair<int, int>& startpos, std::pair<int, int>& endpos, int directfirst)
{
	CalcPathPlan plan = CalcPathPlan();
	plan.DirectFirst = directfirst;
	plan.InitSites(sites);

	return plan.GetPath(startpos, endpos);
}

