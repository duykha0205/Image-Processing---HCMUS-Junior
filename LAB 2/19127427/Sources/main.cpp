#include"GeometricTransformer.h"

int main(int argc, char* argv[]) {
	string agrv1 = argv[1];
	Mat srcImage, dstImage;
	GeometricTransformer* GT;
	GT = new GeometricTransformer();
	PixelInterpolate* Interpolate;

	string argv2 = argv[2];
	if (argv2 == "-bl")
		Interpolate = new BilinearInterpolate();
	else if (argv2 == "-nn")
		Interpolate = new NearestNeighborInterpolate();
	else {
		cout << "Wrong interp" << endl;
		return 0;
	}

	if (argc == 7) {
		srcImage = imread(string(argv[5]), cv::IMREAD_UNCHANGED);
	}
	if (argc == 6) {
		srcImage = imread(string(argv[4]), cv::IMREAD_UNCHANGED);
	}
	if (argc == 5) {
		srcImage = imread(string(argv[3]), cv::IMREAD_UNCHANGED);
	}
	cv::imshow("Display Image", srcImage);
	
	if (agrv1 == "-zoom") {
		float sx = stof(argv[3]);
		float sy = stof(argv[4]);

		if (GT->Scale(srcImage, dstImage, sx, sy, Interpolate)) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[6], dstImage);
		}
		else
		{
			cout << "Can't not resize" << endl;
		}
		
	}
	else if (agrv1 == "-resize")
	{
		int w = stoi(argv[3]);
		int h = stoi(argv[4]);

		if (GT->Resize(srcImage, dstImage, w, h, Interpolate)) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[6], dstImage);
		}
		else
		{
			cout << "Can't not resize" << endl;
		}
	}
	else if (agrv1 == "-rotK") {
		float angle = stof(argv[3]);
		int check = GT->RotateUnkeepImage(srcImage, dstImage, angle, Interpolate);
		
		if (check) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[5], dstImage);
		}
		else
		{
			cout << "Can't not -rotK" << endl;
		}

	}
	else if (agrv1 == "-rotP") {
		float angle = stof(argv[3]);
		if (GT->RotateKeepImage(srcImage, dstImage, angle, Interpolate)) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[5], dstImage);
		}
		else
		{
			cout << "Can't not -rotP" << endl;
		}
	}
	else if (agrv1 == "-flipV") {
		if (GT->Flip(srcImage, dstImage, 0, Interpolate)) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[4], dstImage);
		}
		else
		{
			cout << "Can't not -flipV" << endl;
		}
	}
	else if (agrv1 == "-flipH") {
		if (GT->Flip(srcImage, dstImage, 1, Interpolate)) {
			imshow("Display Image", dstImage);
			waitKey(0);
			imwrite(argv[4], dstImage);
		}
		else
		{
			cout << "Can't not -flipH" << endl;
		}
	}

	srcImage.release();
	dstImage.release();
	return 0;
}