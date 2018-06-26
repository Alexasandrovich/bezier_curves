
#define _CRT_SECURE_NO_WARNINGS


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <cv.h>
#include <math.h>
#include <float.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem>

using namespace cv;
using namespace std;

static int i = 0;
int get_index() {
	return i++;
}
void Replacing(Mat& input_picture)
{
	Mat canvas(input_picture.rows + 50, input_picture.cols + 50, CV_8UC3, Scalar(255, 255, 255));
	Rect roi(10, 10, input_picture.cols, input_picture.rows);
	Mat canvasROI(canvas(roi));
	input_picture.copyTo(canvasROI);
	cv::waitKey();
	string image_name = string("Replacing") + to_string(get_index()) + ".bmp";

	imwrite(image_name, canvas);
}
inline uchar Clamp(int n)
{
	n = n>255 ? 255 : n;
	return n<0 ? 0 : n;
}

bool AddGaussianNoise(const Mat mSrc, Mat &mDst, double Mean = 0.0, double StdDev = 10.0)
{
	if (mSrc.empty())
	{
		cout << "[Error]! Input Image Empty!";
		return 0;
	}

	Mat mGaussian_noise = Mat(mSrc.size(), CV_16SC3);
	randn(mGaussian_noise, Scalar::all(Mean), Scalar::all(StdDev));

	for (int Rows = 0; Rows < mSrc.rows; Rows++)
	{
		for (int Cols = 0; Cols < mSrc.cols; Cols++)
		{
			Vec3b Source_Pixel = mSrc.at<Vec3b>(Rows, Cols);
			Vec3b &Des_Pixel = mDst.at<Vec3b>(Rows, Cols);
			Vec3s Noise_Pixel = mGaussian_noise.at<Vec3s>(Rows, Cols);

			for (int i = 0; i < 3; i++)
			{
				int Dest_Pixel = Source_Pixel.val[i] + Noise_Pixel.val[i];
				Des_Pixel.val[i] = Clamp(Dest_Pixel);
			}
		}
	}

	return true;
}

bool AddGaussianNoise_Opencv(const Mat mSrc, Mat &mDst, double Mean = 0.0, double StdDev = 10.0)
{
	if (mSrc.empty())
	{
		cout << "[Error]! Input Image Empty!";
		return 0;
	}
	Mat mSrc_16SC;
	Mat mGaussian_noise = Mat(mSrc.size(), CV_16SC3);
	randn(mGaussian_noise, Scalar::all(Mean), Scalar::all(StdDev));

	mSrc.convertTo(mSrc_16SC, CV_16SC3);
	addWeighted(mSrc_16SC, 1.0, mGaussian_noise, 1.0, 0.0, mSrc_16SC);
	mSrc_16SC.convertTo(mDst, mSrc.type());

	return true;
}
void Noise(Mat& input_picture)
{
	Mat mColorNoise(input_picture.size(), input_picture.type());

	AddGaussianNoise(input_picture, mColorNoise, 0, 10.0);

	AddGaussianNoise_Opencv(input_picture, mColorNoise, 0, 30.0);
	string image_name = string("Noise") + to_string(get_index()) + ".bmp";
	imwrite(image_name, mColorNoise);
}
void Negative(Mat& input_picture)
{
	Mat new_image = Mat::zeros(input_picture.size(), input_picture.type());
	Mat sub_mat = Mat::ones(input_picture.size(), input_picture.type()) * 255;

	subtract(sub_mat, input_picture, new_image);
	string image_name = string("Negative") + to_string(get_index()) + ".bmp";
	imwrite(image_name, new_image);
}
void Increase_Bright(Mat& input_picture, int count)
{
	Mat img2;
	input_picture.copyTo(img2);
	if (count > 0)
		img2 = img2 * count;
	string image_name = string("Increase_Bright") + to_string(get_index()) + ".bmp";
	imwrite(image_name, img2);
}
void Decrease(Mat& input_picture, int count)
{
	Mat img3;
	input_picture.copyTo(img3);
	if (count > 0)
		img3 = img3 / count;
	string image_name = string("Decrease") + to_string(get_index()) + ".bmp";
	imwrite(image_name, img3);
}
int main()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	hf = FindFirstFile("E://Course/course_work/image*.bmp", &FindFileData);
	if (hf != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string YourPath;
			YourPath = FindFileData.cFileName;
			Mat image = imread(YourPath);
			Noise(image);
			Increase_Bright(image, 4);
			Decrease(image, 4);
			Negative(image);
			Replacing(image);
			i++;
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
}
