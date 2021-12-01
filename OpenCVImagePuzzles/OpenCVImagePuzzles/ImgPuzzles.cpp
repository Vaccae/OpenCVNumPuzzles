#include "ImgPuzzles.h"

//输出显示图像
cv::Mat ImgPuzzles::PuzzleMat = cv::Mat();
int ImgPuzzles::SleepTime = 200;
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
	//根据行和列直接设置容器的个数
	vtsCutMat = std::vector<std::vector<CutMat*>>(rows, std::vector<CutMat*>(cols, nullptr));

	//计算平均分的格数的width和height
	int width = img.cols / cols;
	int height = img.rows / rows;

	//生成序号列表
	std::vector<int> nums = GetVtsPos(cols, rows);

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
		}
	}

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

	}
	break;
	case cv::MouseEventTypes::EVENT_RBUTTONDBLCLK:
	{

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

