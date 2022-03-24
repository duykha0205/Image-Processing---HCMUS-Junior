#include"Blur.h"
# define M_PI           3.14159265358979323846
void swap(float* const x, float* const y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

void Sort(uchar* I)
{
	int n = sizeof(I);
	for (int i = 0; i < n - 1; i++)
		for (int j = i + 1; j < n; j++)
		{
			uchar temp;
			if (I[i] > I[j])
			{
				temp = I[i];
				I[i] = I[j];
				I[j] = temp;
			}
		}
}

void selectionSort(float* const array, const int size)
{
	int smallest; // index of smallest element

	for (int i = 0; i < size - 1; i++)
	{
		smallest = i; // first index of remaining array

		// loop to find index of smallest element
		for (int index = i + 1; index < size; index++)

			if (array[index] < array[smallest])
				smallest = index;

		swap(&array[i], &array[smallest]);
	}
}

Blur::Blur(){}
Blur::~Blur(){}
int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	if (sourceImage.data == NULL) {
		cout << "SrcImage is Null." << endl;
		return 0;
	}
	Convolution Convo;
	vector<float> kernel;
	vector<int> offset;

	switch (method)
	{
	case 0: { //average
		int n = kWidth * kHeight;
		for (int i = 0; i < n; i++)
			kernel.push_back(float(1.0 / n));
		Convo.SetKernel(kernel, kWidth, kHeight);
		return Convo.DoConvolution(sourceImage, destinationImage);
		break;
	}  
	case 1: {	//Median
		int height = sourceImage.rows;
		int width = sourceImage.cols;
		destinationImage = Mat::zeros(height, width, sourceImage.type());
		//Số kênh màu
		int nChannels = sourceImage.channels();
		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];

		//Con trỏ  lưu dữ liệu vùng nhớ ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		uchar* pSrc = (uchar*)sourceImage.data;

		int edgex = (kWidth / 2);
		int edgey = (kHeight / 2);
		float* window;
		window = new float[kHeight * kWidth];
		for (int x = edgex; x < width - edgex; x++) {
			for (int y = edgey; y < height - edgey; y++) {
				int i = 0;
				for (int fx = 0; fx < kWidth; fx++) {
					for (int fy = 0; fy < kHeight; fy++) {
						window[i] = pSrc[(x + fx - edgex) + (y + fy - edgey) * widthStep];
						i++;
					}
				}
				selectionSort(window,int( kHeight * kWidth));
				pDes[x + y * widthStep] = window[(kHeight * kWidth) / 2];
			}
		}
		break;
	}
	case 2: {	//Gaussian
		float sigma = 1.0;
		float temp = 0;
		float mean = kHeight / 2;
		float sum = 0.0; // For accumulating the kernel values
		for (int x = 0; x < kHeight; ++x)
			for (int y = 0; y < kHeight; ++y) {
				temp = exp(-0.5 * (pow((x - mean) / sigma, 2.0) + pow((y - mean) / sigma, 2.0)))
					/ (2 * M_PI * sigma * sigma);

				// Accumulate the kernel values
				sum += temp;
				kernel.push_back(temp);
			}

		//Đưa kernel về chuẩn
		for (int i = 0; i < kHeight*kWidth; i++)
			kernel[i] /= sum;

		Convo.SetKernel(kernel, kWidth, kHeight);
		return Convo.DoConvolution(sourceImage, destinationImage);
		break;
	}
	default:
		break;
	}

	return 1;
}