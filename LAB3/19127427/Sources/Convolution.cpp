#include"Convolution.h"

Convolution::Convolution(){
	this->_kernel = vector<float>(0, 0);
	this->_kernelHeight = 0;
	this->_kernelWidth = 0;
}
Convolution::~Convolution() {}
vector<float> Convolution::GetKernel() {
	return this->_kernel;
}
void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight) {
	this->_kernel = kernel;
	this->_kernelWidth = kWidth;
	this->_kernelHeight = kHeight;
}
int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL) {
		cout << "SrcImg null in DoConvolution fuction" << endl;
		return 0;
	}

	int height = sourceImage.rows;
	int width = sourceImage.cols;
	int padding = (this->_kernelHeight - 1 ) / 2;
	//số kênh màu
	int nChannels = sourceImage.channels();
	//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
	int widthStep = sourceImage.step[0];
	// Tạo ảnh đích 
	destinationImage = Mat::zeros(height, width, sourceImage.type());

	int xStart = this->_kernelWidth / 2;
	int xEnd = width - 1 - xStart;
	int yStart = this->_kernelHeight / 2;
	int yEnd = height - 1 - yStart;

	//con trỏ quản lý vùng nhớ ảnh
	uchar* pDes = (uchar*)destinationImage.data + widthStep + xStart;
	uchar* pSrc = (uchar*)sourceImage.data + widthStep + xStart;

	//_kernelWidth, _kernelHeight là kích thước vùng lân cận
	int kHalfWidth = this->_kernelWidth / 2;
	int kHalfHeight = this->_kernelHeight / 2;
	vector<int> offset;
	//Tạo bảng offset
	for (int y = -kHalfHeight; y <= kHalfHeight; y++)
		for (int x = -kHalfWidth; x <= kHalfWidth; x++)
			offset.push_back(y * widthStep + x);

	float sum;
	for (int i = yStart; i <= yEnd; i++, pDes += widthStep, pSrc += widthStep) {
		uchar* psRow = pSrc;
		uchar* pdRow = pDes;
		for (int j = xStart; j <= xEnd; j++, psRow += nChannels, pdRow += nChannels) {
			sum = 0;
			for (int k = 0; k < offset.size(); k++)
				sum += psRow[offset[k]] * this->_kernel[k];
			pdRow[0] = saturate_cast<uchar>(sum);
		}
	}

	return 1;
}