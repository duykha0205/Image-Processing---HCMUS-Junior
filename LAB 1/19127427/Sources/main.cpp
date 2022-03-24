#include"Converter.h"
#include "ColorTransformer.h"
#include<string>

int main(int argc, char** argv )
{
    Converter ct;
    ColorTransformer tf;
    Mat destinationImage;

    std::string argv_1 = argv[1];
	if (argv_1 == "-rgb2gray") {
		Mat sourceImage = imread(argv[2]);
		ct.Convert(sourceImage, destinationImage, 0);
        imshow("Display Image", destinationImage);
        waitKey(0);
		imwrite(argv[3], destinationImage);
	}
	else if (argv_1 == "-rgb2hsv") {
        Mat sourceImage = imread(argv[2]);
		ct.Convert(sourceImage, destinationImage, 1);
        imshow("Display Image", destinationImage);
        waitKey(0);
        imwrite(argv[3], destinationImage);
	}
	else if (argv_1 == "-bright") {
        int b = std::stoi(argv[2]);
        Mat sourceImage = imread(argv[3]);
        tf.ChangeBrighness(sourceImage, destinationImage, b);
        imshow("Display Image", destinationImage);
        waitKey(0);
        imwrite(argv[4], destinationImage);
	}
	else if (argv_1 == "-contrast") {
        float c = std::stof(argv[2]);
        Mat sourceImage = imread(argv[3]);
        tf.ChangeContrast(sourceImage, destinationImage, c);
        imshow("Display Image", destinationImage);
        waitKey(0);
        imwrite(argv[4], destinationImage);
	}
    else if (argv_1 == "-drawhist") {
        Mat sourceImage = imread(argv[2]);
        Mat histMatrix;
        tf.CalcHistogram(sourceImage, histMatrix);
        tf.DrawHistogram(histMatrix, destinationImage);
        imshow("Display Image", destinationImage);
        waitKey(0);
        imwrite(argv[3], destinationImage);
    }
    else if (argv_1 == "-equalhist") {
        Mat sourceImage = imread(argv[2]);
        tf.HistogramEqualization(sourceImage, destinationImage);
        imshow("Display Image", destinationImage);
        waitKey(0);
        imwrite(argv[3], destinationImage);
    }
	else {
		std::cout << "Error: Invalid arguments" << std::endl;
		return 1;
	}
    
    return 0;
}
