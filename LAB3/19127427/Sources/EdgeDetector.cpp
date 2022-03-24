#include"EdgeDetector.h"

EdgeDetector::EdgeDetector(){}
EdgeDetector::~EdgeDetector(){}
int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int method) {
	if (sourceImage.empty()) {
		cout << "SRC empty" << endl;
		return 0;
	}

	switch (method)
	{
	case 1: {
		//Lọc Sobel
		Convolution Convo;
		vector<float> Kernelx, Kernely;

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		//Destination image
		destinationImage = Mat::zeros(height, width, sourceImage.type());

		float Wx[9] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
		//		[1  0 -1]
		//	Wx =[2  0 -2] 
		//		[1  0 -1]
		float Wy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
		//		[-1  -2  -1]
		//	Wx =[ 0   0   0] 
		//		[ 1   2   1]
		
		//tạo 2 kernel
		for (int i = 0; i < 9; i++) {
			Kernelx.push_back(Wx[i]);
			Kernely.push_back(Wy[i]);
		}

		Mat imgx, imgy;
		Convo.SetKernel(Kernelx, 3, 3);
		Convo.DoConvolution(sourceImage, imgx);
		Convo.SetKernel(Kernely, 3, 3);
		Convo.DoConvolution(sourceImage, imgy);

		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];
		//Con trỏ lưu dữ liệu ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		uchar* pImgx = (uchar*)imgx.data;
		uchar* pImgy = (uchar*)imgy.data;

		float color = 0;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				color = sqrt((pImgx[y*widthStep + x] * pImgx[y * widthStep + x]) + (pImgy[y * widthStep + x] * pImgy[y * widthStep + x]));
				pDes[y * widthStep + x] = saturate_cast<uchar>(color);
			}
		}
		break;
	}
	case 2: {
		//Lọc Prewitt
		Convolution Convo;
		vector<float> Kernelx, Kernely;

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		//Destination image
		destinationImage.create(height, width, sourceImage.type());

		float Wx[9] = { 1, 0, -1, 1, 0, -1, 1, 0, -1 };
		//		[1  0 -1]
		//	Wx =[1  0 -1] 
		//		[1  0 -1]
		float Wy[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
		//		[-1 -1 -1]
		//	Wy =[ 0  0  0]
		//		[ 1  1  1]
		
		//tạo 2 kernel
		for (int i = 0; i < 9; i++) {
			Kernelx.push_back(Wx[i]);
			Kernely.push_back(Wy[i]);
		}

		Mat imgx, imgy;
		Convo.SetKernel(Kernelx, 3, 3);
		Convo.DoConvolution(sourceImage, imgx);
		Convo.SetKernel(Kernely, 3, 3);
		Convo.DoConvolution(sourceImage, imgy);

		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];
		//Con trỏ lưu dữ liệu ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		uchar* pImgx = (uchar*)imgx.data;
		uchar* pImgy = (uchar*)imgy.data;

		float color = 0;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				color = sqrt((pImgx[y * widthStep + x] * pImgx[y * widthStep + x]) + (pImgy[y * widthStep + x] * pImgy[y * widthStep + x]));
				pDes[y * widthStep + x] = saturate_cast<uchar>(color);
			}
		}
		break;
	}
	case 3: {
		//Lọc Laplace
		Convolution Convo;

		float Laplace[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
		//			  [0  1  0]
		//	Laplace = [1 -4  1]
		//			  [0  1  0]

		//Tạo Kernel
		vector<float> Kernel;
		for (int i = 0; i < 9; i++)
			Kernel.push_back(Laplace[i]);

		Convo.SetKernel(Kernel, 3, 3);
		Convo.DoConvolution(sourceImage, destinationImage);
		break;
	} 
	default:
		break;
	}
	return 1;
}