// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

Mat ellipse_detect(Mat& src);
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode);
//加载作弊器
CascadeClassifier cascade;
int main()
{
 	Mat srcImg = imread("G:\\pic\\test2.jpg");	//原始图
	imshow("[srcImg]", srcImg);
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

	Mat face_img = ellipse_detect(srcImg);	//step1
	Mat dst_img = Mat::zeros(srcImg.size(), CV_8UC1);
	Mat detection;
	RemoveSmallRegion(face_img, dst_img,11000,1,0);		//去小白
	RemoveSmallRegion(dst_img, dst_img, 2000, 0, 0);	//去小黑

	vector<Rect> rect;				//坐标
	cascade.load("F:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt2.xml");
	srcImg.copyTo(detection, dst_img);  //返回肤色图
	cascade.detectMultiScale(detection, rect, 1.1, 3, 1);  // 定位人脸作弊法~ 

	//矩形框标出
	for (int i = 0; i < rect.size(); i++)
	{
		rectangle(srcImg, Rect(rect[i].x, rect[i].y, rect[i].width, rect[i].height), colors[i], 3);
	}
	imshow("[Binary]", dst_img);
	imshow("[result]", srcImg);

	waitKey();
    return 0;
}


/*

	基于椭圆皮肤模型的皮肤检测
	如果将皮肤信息映射到YCrCb空间，则在CrCb二维空间中这些皮肤像素点近似成一个椭圆分布。

*/
Mat ellipse_detect(Mat& src)
{
	Vec3b ycrcb;
	vector<Rect> rect;				//坐标
	Mat img = src.clone();
	//预处理 拉普拉斯算子图像增强效果
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0,2.9, 0, 0, -1, 0);
	filter2D(img, img, CV_8UC3, kernel);
	Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);

	//利用opencv自带的椭圆生成函数先生成一个肤色椭圆模型
	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);

	Mat ycrcb_image;
	Mat output_mask = Mat::zeros(img.size(), CV_8UC1);
	cvtColor(img, ycrcb_image, CV_BGR2YCrCb); //首先转换成到YCrCb空间
	/*                   
	正常黄种人的Cr分量大约在133至173之间，
	Cb分量大约在77至127之间。

	*/
	for (int i = 0; i < img.cols; i++)   //利用椭圆皮肤模型进行皮肤检测
		for (int j = 0; j < img.rows; j++)
		{
			ycrcb = ycrcb_image.at<Vec3b>(j, i);			//ycrcb[0]B,ycrcb[1]G Cr,ycrcb[2]R Cb
			if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0)   //椭圆区域内部为白色，其它地方为黑色。所以当其需要判断其它像素点时，只需将该像素点转换成Cr，Cb两个坐标，然后在上面的椭圆中找到该坐标的值，如果大于0，则为皮肤，反之亦然。
			{
				output_mask.at<uchar>(j, i) = 255;				//如直接使用RGB,则在if中直接定RGB值区间
			}
		}
//	Mat eelement = getStructuringElement(MORPH_RECT, Size(2 * 1, 2 * 1), Point(0, 0));
//	Mat delement = getStructuringElement(MORPH_RECT, Size(2 * 1, 2 * 1), Point(0, 0));
//	erode(output_mask, output_mask, eelement, Point(-0.5, -0.5), 2);		//腐蚀
//	dilate(output_mask, output_mask, delement, Point(-0.5, -0.5),3);		//膨胀
	return output_mask;
}
  
//去除小连通域
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;       //记录除去的个数  
							   //记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查  
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);

	if (CheckMode == 1)
	{
		//去除小区域.
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] < 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}
	else
	{
		//除孔洞. 
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] > 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}

	vector<Point2i> NeihborPos;  //记录邻域点位置  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//Neighbor mode: 8邻域.
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	//Neighbor mode: 4邻域
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;

	//开始检测  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********  
				vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点  
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出  

				for (int z = 0; z<GrowBuffer.size(); z++)
				{

					for (int q = 0; q<NeihborCount; q++)                                      //检查四个邻域点  
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界  
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer  
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (GrowBuffer.size()>AreaLimit) CheckResult = 2;                 //判断结果（是否超出限定的大小），1为未超出，2为超出  
				else { CheckResult = 1;   RemoveCount++; }
				for (int z = 0; z<GrowBuffer.size(); z++)                         //更新Label记录  
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					Pointlabel.at<uchar>(CurrY, CurrX) += CheckResult;
				}
				//********结束该点处的检查**********  


			}
		}
	}

	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iData = Src.ptr<uchar>(i);
		uchar* iDstData = Dst.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 2)
			{
				iDstData[j] = CheckMode;
			}
			else if (iLabel[j] == 3)
			{
				iDstData[j] = iData[j];
			}
		}
	}

	//cout << RemoveCount << " objects removed." << endl;
}


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