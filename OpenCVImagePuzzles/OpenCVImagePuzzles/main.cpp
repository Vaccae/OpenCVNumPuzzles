#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "ImgPuzzles.h"

using namespace std;
using namespace cv;

//设置图片所以路径
String FilePaths = "E:/DCIM/Resize";

int main(int argc, char** argv) {


	try
	{
		//获取目录下的所有文件
		vector<String> files;
		glob(FilePaths, files);

		if (files.size() <= 0) {
			cout << "找不到图片文件" << endl;
			waitKey(0);
			return -1;
		}

		for (int index = 0; index < files.size(); ++index) {
			//关闭所有显示窗口
			destroyAllWindows();

			String file = files[index];

			Mat src = imread(file);

			if (src.empty()) {
				cout << "图像加载失败。。。。" << endl;
				waitKey(0);
				return -1;
			}

			//设置图像缩放到500*500
			Mat tmpsrc;
			resize(src, tmpsrc, Size(500, 500));

			imshow("src", src);
			imshow("tmpsrc", tmpsrc);

			//获取图像分割后的集合
			ImgPuzzles::SplitMats(tmpsrc);

			//绘制图像
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