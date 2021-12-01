#include "ImgPuzzles.h"
#include "..\..\Utils\CalcReverseNum.h"

//�����ʾͼ��
cv::Mat ImgPuzzles::PuzzleMat = cv::Mat();
int ImgPuzzles::SleepTime = 200;
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
		vtsCutMat[vtsCutMat.size() - 1][0]->curposition = tmpcurposition;
	}
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

