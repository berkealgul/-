#include"vo.h"

void processFrame(Mat *frame, Mat *prevImg, Mat *R, Mat*T, std::vector<Point2f> &prevFeatures)
{
	Mat currImg;
	//al�nan resmi siyah-beyaz formata d�n��t�r
	cvtColor(*frame, currImg, COLOR_BGR2GRAY);

	std::vector<Point2f> currFeatures;
	std::vector<uchar>status;

	Mat mask;

	tractFeatures(prevImg, prevFeatures, &currImg, currFeatures, status);
	
	Mat E = findEssentialMat(currFeatures, prevFeatures, focal, pp, RANSAC, 0.999, 1.0, mask);
	recoverPose(E, currFeatures, prevFeatures, *R, *T, focal, pp, mask);

	//zaman ge�tik�e �zellik say�s� d��ecektir
	//e�er �zellik say�s� belli bir s�nr�n alt�na d��er ise tekrardan arama yapaca��z
	if (prevFeatures.size() < MIN_FEAS)
	{
		detectFeatures(prevImg, prevFeatures);
		tractFeatures(prevImg, prevFeatures, &currImg, currFeatures, status);
	}
	
	*prevImg = currImg.clone();
	prevFeatures = currFeatures;
}

void detectFeatures(Mat *inputImg, std::vector<Point2f> &points)
{
	std::vector<KeyPoint> keyPoints;
	FAST(*inputImg, keyPoints, 10, true);

	//ileriki a�ama i�in "keypoint" t�r� "point2f" t�r�ne d�n��t�r�lmeli,
	KeyPoint::convert(keyPoints, points, std::vector<int>());
}

void tractFeatures(Mat *prevImg, std::vector<Point2f> &prevPts, Mat *currImg, std::vector<Point2f> &currPts, std::vector<uchar> &status)
{
	std::vector<float> err;
	calcOpticalFlowPyrLK(*prevImg, *currImg, prevPts, currPts, status, err, Size(21, 21), 3, termCrit, 0, 0.001);

	//hatal� e�le�melerden kurtulur lakin
	//bu k�s�m bana gizemli geldi o y�zden ayr�nt�lar ile ilgili bir �ey diyemeyece�im
	int indexCorrection = 0;
	for (int i = 0; i < status.size(); i++)
	{
		Point2f pt = currPts.at(i - indexCorrection);
		if ((status.at(i) == 0) || (pt.x < 0) || (pt.y < 0)) {
			if ((pt.x < 0) || (pt.y < 0)) {
				status.at(i) = 0;
			}
			currPts.erase(currPts.begin() + (i - indexCorrection));
			prevPts.erase(prevPts.begin() + (i - indexCorrection));
			indexCorrection++;
		}
	}
}