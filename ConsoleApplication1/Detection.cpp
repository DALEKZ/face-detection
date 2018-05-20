// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

Mat ellipse_detect(Mat& src);

int main()
{
 	Mat srcImg = imread("G:\\pic\\test6.jpg");	//原始图
												// 定义7种颜色，用于标记人脸  
	Scalar colors[] =
	{
		// 红橙黄绿青蓝紫  
		CV_RGB(255, 0, 0),
		CV_RGB(255, 97, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(0, 255, 0),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 0, 255),
		CV_RGB(160, 32, 240)
	};

	////加载作弊器
	//CascadeClassifier cascade;
	//cascade.load("F:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt2.xml");

	
	//Mat output_image;
	//Mat grayImg,binaryImg,ycrcbImg;
	//cvtColor(srcImg, ycrcbImg, CV_BGR2YCrCb); //首先转换成到YCrCb空间， 此步骤效果更好娃!

	//imshow("显示原始图像", srcImg);

	////直方图均衡化
	//Mat imageRGB[3];
	//split(ycrcbImg, imageRGB);
	//for (int i = 0; i < 3; i++)
	//{
	//	equalizeHist(imageRGB[i], imageRGB[i]);
	//}
	//merge(imageRGB, 3, ycrcbImg);
	////imshow("直方图均衡化图像增强效果", ycrcbImg);

	////二值化
	//cvtColor(ycrcbImg, grayImg, CV_RGB2GRAY);    //转为灰度图
	//threshold(grayImg, binaryImg, 150, 255, CV_THRESH_BINARY);
	//cascade.detectMultiScale(grayImg, rect, 1.1, 3, 1);  // 作弊法~ 



	//画脸
	vector<Rect> rect;				//坐标
	imshow("[hualian]", ellipse_detect(srcImg));	//main


	//矩形框标出
	for (int i = 0; i < rect.size(); i++)
	{
		rectangle(srcImg, Rect(rect[i].x, rect[i].y, rect[i].width, rect[i].height), colors[i], 3);
	}

	waitKey();
    return 0;
}


/*基于椭圆皮肤模型的皮肤检测*/
Mat ellipse_detect(Mat& src)
{
	Mat img = src.clone();
	//预处理
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0,2.9, 0, 0, -1, 0);
	filter2D(img, img, CV_8UC3, kernel);
	imshow("拉普拉斯算子图像增强效果", img);

	Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
	//利用opencv自带的椭圆生成函数先生成一个肤色椭圆模型
	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 0, 0), -1);
	Mat ycrcb_image;
	Mat output_mask = Mat::zeros(img.size(), CV_8UC1);
	cvtColor(img, ycrcb_image, CV_BGR2YCrCb); //首先转换成到YCrCb空间
	for (int i = 0; i < img.cols; i++)   //利用椭圆皮肤模型进行皮肤检测
		for (int j = 0; j < img.rows; j++)
		{
			Vec3b ycrcb = ycrcb_image.at<Vec3b>(j, i);
			if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0)   //如果该落在皮肤模型椭圆区域内，该点就是皮肤像素点
				output_mask.at<uchar>(j, i) = 255;
		}
	Mat eelement = getStructuringElement(MORPH_RECT, Size(2 * 1, 2 * 1), Point(0, 0));
	Mat delement = getStructuringElement(MORPH_RECT, Size(2 * 1, 2 * 1), Point(0, 0));
	erode(output_mask, output_mask, eelement, Point(-0.5, -0.5), 2);		//腐蚀
	dilate(output_mask, output_mask, delement, Point(-0.5, -0.5),3);		//膨胀

	Mat detect;
	img.copyTo(detect, output_mask);  //返回肤色图
	return detect;
}