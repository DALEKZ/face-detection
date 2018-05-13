// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;
int main()
{
	Mat grayImg;	//灰度图
	Mat srcImg = imread("G:\\pic\\test.jpg");	//原始图
	imshow("显示原始图像", srcImg);
	cvtColor(srcImg, grayImg, CV_RGB2GRAY);		//颜色空间转换
	int rowNumber = grayImg.rows;
	int colNumber = grayImg.cols;
	int	sumNumber = rowNumber * colNumber;	//像素个数
	Mat dstImg(rowNumber, colNumber, CV_8UC1, Scalar(0, 0, 0));		//初始化直方图均衡化后的图
	double hist[256] = { 0,00 };	//直方图
	double dhist[256] = { 0,00 };	//直方图归一化图
	double Dhist[256] = { 0,00 };	//直方图积分图，每一个像素点
	for (int i = 0; i < rowNumber; i++)		//遍历原始图
	{
		uchar* data = grayImg.ptr<uchar>(i);
		for (int j = 0; j < colNumber; j++)
		{
			int temp = data[j];
			hist[temp] += 1;
		}
	}

	 for(int i = 0;i<256;i++)//遍历直方图，得到归一化直方图和积分图
    {       
        dhist[i] = hist[i]/sumNumber;//得到归一化图
        for(int j = 0;j<=i;j++)
        {
            Dhist[i] = Dhist[i] + dhist[j]; //得到积分图
        }
    }


	for (int i = 0; i<rowNumber; i++)//以积分图为查找表得到均衡化后的图
	{
		uchar* data1 = dstImg.ptr<uchar>(i);
		uchar* data2 = grayImg.ptr<uchar>(i);
		for (int j = 0; j<colNumber; j++)
		{
			int temp1 = data2[j]; //查找到原始图相应位置的像素值
			int temp2 = (int)(Dhist[temp1] * 255); //在积分图中找到相应像素值的映射值
			data1[j] = temp2;//将映射值赋值给目标图像相应值
		}
	}
	imshow("直方图", dstImg);
	waitKey(0);



	/*
	
	直方图均衡化

	Mat imageRGB[3];
	split(srcImg, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, srcImg);
	imshow("直方图均衡化图像增强效果", srcImg);
	waitKey();
	
	
	*/
    return 0;
}

