#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "ImgPuzzles.h"

using namespace std;
using namespace cv;

//����ͼƬ����·��
String FilePaths = "E:/DCIM/Resize";

int main(int argc, char** argv) {


	try
	{
		//��ȡĿ¼�µ������ļ�
		vector<String> files;
		glob(FilePaths, files);

		if (files.size() <= 0) {
			cout << "�Ҳ���ͼƬ�ļ�" << endl;
			waitKey(0);
			return -1;
		}

		for (int index = 0; index < files.size(); ++index) {
			//�ر�������ʾ����
			destroyAllWindows();

			String file = files[index];

			Mat src = imread(file);

			if (src.empty()) {
				cout << "ͼ�����ʧ�ܡ�������" << endl;
				waitKey(0);
				return -1;
			}

			//����ͼ�����ŵ�500*500
			Mat tmpsrc;
			resize(src, tmpsrc, Size(500, 500));

			imshow("src", src);
			imshow("tmpsrc", tmpsrc);

			//��ȡͼ��ָ��ļ���
			ImgPuzzles::SplitMats(tmpsrc);

			//����ͼ��
			ImgPuzzles::CreatePuzzleMat();

			cv::waitKey(0);
		}
		return 0;
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
		cv::waitKey(0);
		return -1;
	}
}