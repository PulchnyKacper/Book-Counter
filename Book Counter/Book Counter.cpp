#include"opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<stdio.h>
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<limits>

char choice = 0;
int number_of_pictures = 0;

double safe_Input()
{
	int input;
	bool valid = false;
	while (!valid){
		std::cin >> input;
		if (std::cin.good())
		{
			valid = true;
		}
		else
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Enter the correct number, it's not too difficult \n ";
		}
	} 

	return (input);
}
void Interface() {
	std::cout << "What would you prefer: \n"; 
	std::cout << "(1) Input a file name (file must be present in Images_of_books file) \n";
	std::cout << "(2) Enter the number of images to be processed (images must be present in Images_of_books file and they must be named with consecutive numbers starting with 1)\n";
	choice = safe_Input();
	while (choice != 1 && choice != 2){
		
		std::cout << "Enter the correct number, it's not too difficult \n ";
		choice = safe_Input();
	}
}
std::string convertToString(char* a) {
	std::string s = a;
	return s;
}
void test() {
	cv::Mat test1 = cv::imread("Images_of_books/Test.jpg");
	cv::imshow("okienko", test1);
	cv::waitKey(0);
}
void counter(cv::Mat image) {
	float number_of_books = 0;
	//Gauss
	int ddepth = CV_16S, size_Gaus = 5, blur_power = 2;//5,2
	//Edges
	int low_Tresh = 30, max_Tresh = 80;
	//Erosion
	int numer_of_erosions = 3;//3
	cv::Mat kernel_erode = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 3));//1,3
	//Dilation
	cv::Mat kernel_dilate = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 4));//1,4
	int numer_of_dilatations = 6;//6

	cv::Mat gray, blur, edge, eroded, dilated;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	cv::imshow("okienko", gray);
	cv::waitKey(0);

	cv::GaussianBlur(gray, blur, cv::Size(size_Gaus, size_Gaus), blur_power);
	cv::imshow("okienko", blur);
	cv::waitKey(0);

	cv::Canny(blur, edge, low_Tresh, max_Tresh);
	cv::convertScaleAbs(edge, edge);
	cv::imshow("okienko", edge);
	cv::waitKey(0);

	erode(edge, eroded, kernel_erode, cv::Point(-1, -1), numer_of_erosions);
	cv::imshow("okienko", eroded);
	cv::waitKey(0);

	dilate(eroded, dilated, kernel_dilate, cv::Point(-1, -1), numer_of_dilatations);
	cv::imshow("okienko", dilated);
	cv::waitKey(0);
	/*
	std::vector<cv::Vec2f> lines;
	HoughLines(dilated, lines, 2, CV_PI / 180, 200, 0, 0);
	for (size_t i = 0; i < lines.size(); i++)
	{

		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x) * 180.0 / CV_PI;
		if (angle >= 87) {
			line(original, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
			number_of_books++;
		}

	}
	cv::imshow("okienko", original);
	cv::waitKey(0);
	*/
	std::vector<cv::Vec4i> linesP;
	std::vector<cv::Vec4i> linesP_Useful;
	std::vector<cv::Vec4i> linesP_Even_More_Useful;
	std::vector<cv::Vec4i> linesP_Final;
	HoughLinesP(dilated, linesP, 1, CV_PI / 180, 100, 100, 60);
	for (size_t i = 0; i < linesP.size(); i++)
	{
		cv::Vec4i l = linesP[i];
		double angle = atan2(l[3] - l[1], l[0] - l[2]) * 180.0 / CV_PI;
		if (angle <= 94) {
			linesP_Useful.push_back(l);
		}
	}
	for (int i = 0; i < linesP_Useful.size(); i++) {
		for (int j = i + 1; j < linesP_Useful.size(); j++)
		{
			struct temp {
				int x1;
				int x2;
				int y1;
				int y2;
			} temp;
			cv::Vec4i l = linesP_Useful[i];
			cv::Vec4i lnext = linesP_Useful[j];
			if (lnext[0] < l[0]) {
				temp.x1 = l[0];
				temp.x2 = l[2];
				temp.y1 = l[1];
				temp.y2 = l[3];
				lnext[0] = temp.x1;
				lnext[2] = temp.x2;
				lnext[1] = temp.y1;
				lnext[3] = temp.y2;
				linesP_Useful[i] = linesP_Useful[j];
				linesP_Useful[j] = lnext;
			}
		}
	}
	for (size_t i = 1; i < linesP_Useful.size(); i++)
	{
		cv::Vec4i l = linesP_Useful[i - 1];
		cv::Vec4i lnext = linesP_Useful[i];
		if (l[0] + 14 < lnext[0]) {

			linesP_Even_More_Useful.push_back(lnext);
		}

	}
	for (size_t i = 1; i < linesP_Even_More_Useful.size(); i++)
	{
		cv::Vec4i l = linesP_Even_More_Useful[i - 1];
		if (l[1] > 150) {

			linesP_Final.push_back(l);
		}

	}
	for (size_t i = 0; i < linesP_Final.size(); i++)
	{
		cv::Vec4i l = linesP_Final[i];
		line(image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
		number_of_books++;
	}
	system("cls");

	std::cout << number_of_books << std::endl;

	cv::imshow("okienko", image);
	cv::waitKey(0);
}
//Stages
cv::Mat image;
std::string name;

int main()
{

	//test();
	system("cls");
	Interface();
	switch (choice) {
	case 1:
		std::cout << "Enter file name \n";
		std::cin >> name;
		image = cv::imread("Images_of_books/"+name);
		counter(image);
		return 0;

	case 2:
		std::cout << "Enter file number \n";
		number_of_pictures = safe_Input();
		for (int i = 1; i < number_of_pictures+1; i++) {
			char buf[50];
			int n;
			n = sprintf_s(buf, "Images_of_books/%d.jpg", i);
			name = convertToString(buf);
			image = cv::imread(name);
			if (image.empty()) {
				std::cout << "Error loading image" << std::endl;
			}
			counter(image);
		}
		return 0;
	}

}
