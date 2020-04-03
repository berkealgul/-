#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;

//�zellik say�s�n�n izin verilen en d���k de�er
#define MIN_FEAS 500

#define termCrit TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01)

//NOT:a�a��daki �zellikler kameradan kameraya de�i�ir

//kamera lensinin odak uzakl���
#define focal 718.8560
//kameran�n prensip noktas� (resmin orta noktas�)
#define pp cv::Point2d(607.1928, 185.2157)

/*
	<algoritman�n ana fonksiyonu>

	ald�klar�:
		frame - kamera g�r�nt�s�(renkli) (pointer)
		prevImg - �nceki g�r�nt�(siyah-beyaz) (pointer)
		prevFeatures - �nceki g�r�nt�deki k��eler (vector)

	verdikleri:
		R - iki g�r�nt� aras�ndaki rotasyon de�i�ikli�i
		T - iki g�r�nt� aras�ndaki konum de�i�ikli�i
*/
void processFrame(Mat *frame, Mat *prevImg, Mat *R, Mat*T, std::vector<Point2f> &prevFeatures);


//yard�mc� fonksiyonlar

void detectFeatures(Mat *inputImg, std::vector<Point2f> &points);
void tractFeatures(Mat *prevImg, std::vector<Point2f> &prevPts, Mat *currImg, std::vector<Point2f> &currPts, std::vector<uchar> &status);