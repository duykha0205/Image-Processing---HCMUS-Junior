#include"Blur.h"
#include"Convolution.h"
#include"EdgeDetector.h"


int main(int argc, char** argv) {
	Mat sourceImage, destinationImage;
	Blur* blur = new Blur;
	EdgeDetector* edge = new EdgeDetector;
	int method_blur = -1;
	int method_edge = -1;

	// Gán giá trị tương ứng cho từng phép biến đổi
	if (string(argv[1]).compare("-avg") == 0) {
		method_blur = 0;
	}
	else if (string(argv[1]).compare("-med") == 0)
	{
		method_blur = 1;
	}
	else if (string(argv[1]).compare("-gau") == 0)
	{
		method_blur = 2;
	}
	else if (string(argv[1]).compare("-sobel") == 0)
	{
		method_edge = 1;
	}
	else if (string(argv[1]).compare("-prew") == 0)
	{
		method_edge = 2;
	}
	else if (string(argv[1]).compare("-lap") == 0)
	{
		method_edge = 3;
	}

	// Lấy ảnh từ input
	if (argc == 6) {
		sourceImage = imread(string(argv[4]));
	}
	else if (argc == 4) {
		sourceImage = imread(string(argv[2]));
	}

	// convert sang grayscale nếu là RGB
	if (sourceImage.channels() != 1) {
		cout << "cvtColor" << endl;
		cvtColor(sourceImage, sourceImage, COLOR_BGR2GRAY);
	}
	imshow("Display SrcImage", sourceImage);
	waitKey(0);

	// Thực hiện phép biến đổi
	if (argc == 6) {
		int kWidth = stoi(argv[2]);
		int kHeight = stoi(argv[3]);
		if (blur->BlurImage(sourceImage, destinationImage, kWidth, kHeight, method_blur)) {
			imshow("Display Image", destinationImage);
			waitKey(0);
			imwrite(argv[5], destinationImage);
		}
		else
		{
			cout << "Can't blur" << endl;
		}
	}
	else if (argc == 4) {
		if (edge->DetectEdge(sourceImage, destinationImage, method_edge)) {
			imshow("Display Image", destinationImage);
			waitKey(0);
			imwrite(argv[3], destinationImage);
		}
		else
		{
			cout << "Can't detect edge" << endl;
		}
	}

	return 0;
}

int main1() {
	Mat sourceImage, destinationImage;
	Blur* blur = new Blur;
	sourceImage = imread("img.jpg");
	
	if (sourceImage.channels() != 1) {
		cout << "cvtColor" << endl;
		cvtColor(sourceImage, sourceImage, COLOR_BGR2GRAY);
	}

	imshow("Display SrcImage", sourceImage);
	waitKey(0);

	if (blur->BlurImage(sourceImage, destinationImage, 5, 5, 1)) {
		imshow("Display Image", destinationImage);
		waitKey(0);
	}
	else
	{
		cout << "Can't" << endl;
	}

	return 1;
}