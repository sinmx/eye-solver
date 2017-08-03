#pragma once
#include "Menu.h"
#include "graphic.h"
#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using std::cout;
using namespace cv;

void Menu::SizeWindow(RECT * area)
{
	HWND handle;
	Mat blank = Mat::zeros({100,200},1 );
	SetWindow("Size Me", blank, 100, 100);
	cout << "Set the size of the \"Size Me\" window and press enter";
	imshow("Size Me", blank);
	waitKey();

	if (handle = FindWindow(NULL, TEXT("Size Me"))) {
		//std::cout << "Found!"; 
		GetWindowRect(handle, area);
		DestroyWindow(handle);
	}
	//else {std::cout << "not found";}
}

state Menu::mainMenu(RECT * area, Nnet *&ref)
{
	int chose = 0;//SAFTY not working
	while (true)//LOAD / NEW
	{
		if (yesNoPromt("Would you like to load a Network?"))
		{//Load Network

			Nnet n("first.bin");
			ref = &n;

			if (yesNoPromt("Do you want to train the Network?"))
			{//Train Network
				SizeWindow(area);
				return CONTINUE;
			}
			else//Test Network
				return LOAD;
		}
		else//New Network
		{
			SizeWindow(area);
			return NEW;
		}
	}
}

state Menu::StartMenu(RECT * area,Nnet *&ref)
{
	string version = "0.6.1";
	cout << "Solution Finder " << version << ":" << std::endl;

	return mainMenu(area,ref);
}

void Menu::MenuSizeWindow(RECT * area)
{
	HWND handle;
	//string window;
	//cout << "Select name of window to watch: ";
	//cin >> window;
	Mat blank;
	namedWindow("Size Me", WINDOW_FREERATIO);
	//imshow("Size Me",blank);
	cout << "Set the size of the \"Size Me\" window and press enter";
	waitKey();

	if (handle = FindWindow(NULL, TEXT("Size Me"))) {
		//std::cout << "Found!"; 
		GetWindowRect(handle, area);
		DestroyWindow(handle);
	}
	//else {std::cout << "not found";}
}

state Menu::FinishTrainMenu(RECT* area,Nnet *&ref)
{
	cout << "Done Generating Network" << std::endl;
	return mainMenu(area,ref);
}