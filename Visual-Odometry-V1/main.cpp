#include"vo.h"
#include<iostream>
#include <sstream>
#include <fstream>
#include <string>

#define TOTAL_FRAMES 4000

/*
	4 konumda dosya konumu de�i�ebilir
	(3 main , 1 getAbsoluteScale)
*/

double getAbsoluteScale(int frame_id);

int main()
{
	Mat T_f, R_f; //kameran�n g�ncel konum ve rotasyon matrisi
	Mat R, T; //kameran�n anl�k de�i�en konum ve rotasyonu

	double scale = 1.0; //hareketin boyutu

	//y�r�ngeyi g�sterecek resim
	Mat traj = Mat::zeros(600, 600, CV_8UC3);

	Mat prevImg;
	std::vector<Point2f> prevFeatures;


	//d�ng�ye girmeden tan�mlama i�lemleri yap�lmal�

	char filename1[200];
	char filename2[200];
	sprintf(filename1, "C:/Berke/VOdataset/00/image_0/%06d.png", 0);
	sprintf(filename2, "C:/Berke/VOdataset/00/image_0/%06d.png", 1);

	Mat img = imread(filename1);
	Mat img2 = imread(filename2);

	cvtColor(img, prevImg, COLOR_BGR2GRAY);

	detectFeatures(&prevImg, prevFeatures);
	processFrame(&img2, &prevImg, &R, &T, prevFeatures);

	T_f = T.clone();
	R_f = R.clone();

	char filename[200];

	for(int frameNum = 2; frameNum < TOTAL_FRAMES; frameNum++)
	{
		sprintf(filename, "C:/Berke/VOdataset/00/image_0/%06d.png", frameNum);

		Mat frame = imread(filename);

		//odometry algoritmas� devreye girer
		processFrame(&frame, &prevImg, &R, &T, prevFeatures);
		
		//hareket boyutunu  bul
		scale = getAbsoluteScale(frameNum);

		//e�er ge�erli bir hareket elde ediyor isek ve hareket boyutu d���k de�ilse
		//kameran�n mutlak konum ve rotasyonu hesapla
		if ((scale > 0.1) &&(T.at<double>(2) > T.at<double>(1)) && (T.at<double>(2) > T.at<double>(0)))
		{
			T_f = T_f + scale * (R_f * T);
			R_f = R * R_f;
		}
		else
			std::cout << "GECERSIZ HAREKET \n";
		

		//algoritman�n tahmin etti�i y�r�ngeyi �iz
		int x = int(T_f.at<double>(0)) + 300;
		int y = int(T_f.at<double>(2)) + 100;
		circle(traj, Point(x, y), 1, CV_RGB(255, 0, 0), 2);

		//resimleri g�ster
		imshow("video", frame);
		imshow("trajectory", traj);

		std::cout << T_f << std::endl;
		std::cout << prevFeatures.size();
		std::cout << "\n\n";    

		if (waitKey(1) == 27) //27 == esc
			break;
	}

	return 0;
}

double getAbsoluteScale(int frame_id)
{
	/*
		bu fonksiyonun yapt��� veri taban�ndan �ekti�i konum verileri ile 
		�imdiki ile �nceki resim aras�ndaki konum de�i�imini
		hesapl�yor (bir nevi dx dy dz) ve bu de�i�imlerden 
		hareket boyutunu ��kar�yor
		boyut = sprt(dx^2 + dy^2 + dz^2);
	*/

	std::string line;
	int i = 0;
	std::ifstream myfile("C:/Berke/VOdataset/00/times.txt"); //dosyan�n yeri de�i�ebilir

	double x = 0, y = 0, z = 0;
	double x_prev, y_prev, z_prev;

	if (myfile.is_open())
	{
		while ((getline(myfile, line)) && (i <= frame_id))
		{
			z_prev = z;
			x_prev = x;
			y_prev = y;
			std::istringstream in(line);

			for (int j = 0; j < 12; j++) {
				in >> z;
				if (j == 7) y = z;
				if (j == 3)  x = z;
			}

			i++;
		}
		myfile.close();
	}
	else
	{
		std::cout << "Unable to open file";
		return 0;
	}

	return 2*sqrt((x - x_prev)*(x - x_prev) + (y - y_prev)*(y - y_prev) + (z - z_prev)*(z - z_prev));
}