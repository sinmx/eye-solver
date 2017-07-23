#pragma once
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "utility.h"

using namespace cv;
using std::string;

const string ORDER = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{}\|;:\"<>,./?`";

class OCR
{
public:

	void SetFont(string path,int width, int height){ 
		Mat font = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
		if (font.empty()) //check whether the image is loaded or not
		{
			std::cout << "Error : Image cannot be loaded..!!" << std::endl;
			system("pause"); //wait for a key press
		}

		int loc = 0;
		for (int j = 0; j < font.rows / height; j++)
		{
			for (int i = 0; i < font.cols / width; i++)
			{
				chars.push_back(font(Rect(width*i, height*j, width, height)));
				//imshow("font",chars.back());
				//waitKey(100);
			}
		}

		//imshow("font",font);
		//waitKey();
	}

	//give it a name and a color image and it will return the text using the set font
	string textReconition(string Display, Mat ColorImage)
	{

		namedWindow(Display, CV_WINDOW_KEEPRATIO);
		moveWindow(Display, 2050, 300);

		namedWindow("font", CV_WINDOW_KEEPRATIO);

		//image to read converted to binary
		Mat greytext(ColorImage.size(), CV_8U);
		cvtColor(ColorImage, greytext, COLOR_BGR2GRAY);
		Mat binarytext(greytext.size(), greytext.type());
		threshold(greytext, binarytext, 220, 230, THRESH_BINARY);//not in menu

		//FONT READING -------------------------------------//
		///threshold(font, font, 200, 230, THRESH_BINARY);//200,230

		//resize font to make it the same character size
		//int diff = 3;
		//Mat newfont;
		//Size size(font.cols*diff, font.rows*diff);
		//resize(font, newfont, size);


		resizeWindow(Display, greytext.cols, greytext.rows);
		imshow(Display, binarytext);
		waitKey(1);

		///displays font
		//namedWindow("font", CV_WINDOW_KEEPRATIO);
		//resizeWindow("font", font.cols, font.rows);
		//imshow("font", font);


		return "";
	}

private:
	std::vector<Mat> chars;
};



