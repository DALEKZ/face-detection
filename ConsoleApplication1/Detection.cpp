// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;
int main()
{
	Mat output_image;
	/*椭圆皮肤模型*/
	Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
	
	Mat grayImg,binaryImg,ycrcbImg;
	Mat srcImg = imread("G:\\pic\\test7.jpg");	//原始图
	cvtColor(srcImg, ycrcbImg, CV_BGR2YCrCb); //首先转换成到YCrCb空间， 此步骤效果更好娃!

	imshow("显示原始图像", ycrcbImg);

	//直方图均衡化
	Mat imageRGB[3];
	split(ycrcbImg, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, ycrcbImg);
	imshow("直方图均衡化图像增强效果", ycrcbImg);

	//二值化
	cvtColor(ycrcbImg, grayImg, CV_RGB2GRAY);    //转为灰度图
	threshold(grayImg, binaryImg, 150, 255, CV_THRESH_BINARY);
	imshow("二值图2", binaryImg);


	//椭圆肤色模型
	IplImage* src = &IplImage(srcImg);

	for (int i = 0; i < binaryImg.cols; i++)   //利用椭圆皮肤模型进行皮肤检测
		for (int j = 0; j < binaryImg.rows; j++) {
			CvScalar s = cvGet2D(src, i, j);
			Vec3b ycrcb = srcImg.at<Vec3b>(j, i);
			if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0)
			//	binaryImg.at<uchar>(j, i) = 255;	
			if (s.val[0] > 60 &&s.val[0] < 190 
					&& s.val[1] > 100 && s.val[1]<200
					&& s.val[2] > 150 && s.val[2] < 240)	//BGR
			{
				binaryImg.at<uchar>(j, i) = 255;

			}
		}
	srcImg.copyTo(output_image, binaryImg); //？？
	imshow("output image", output_image);		//思考怎么把肤色区域搞出来(getRGB值)
	//imshow("binaryImg", binaryImg);

	output_image.setTo(0);
	
	waitKey();
    return 0;
}

