#include"GeometricTransformer.h"

PixelInterpolate::PixelInterpolate() {}
PixelInterpolate::~PixelInterpolate() {}
//----------------

BilinearInterpolate::BilinearInterpolate() {}
BilinearInterpolate::~BilinearInterpolate() {}

uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {

	int x1 = static_cast<int>(floor(tx));
	int y1 = static_cast<int>(floor(ty));

	float a = tx - x1;
	float b = ty - y1;
	
	float f_xy = (1 - a) * (1 - b) * pSrc[srcWidthStep * y1 + nChannels * x1]
		+ a * (1 - b) * pSrc[srcWidthStep * y1 + nChannels * (x1 + 1)]
		+ b * (1 - a) * pSrc[srcWidthStep * (y1 + 1)+nChannels * (x1)]
		+ a * b * pSrc[srcWidthStep * (y1 + 1) + nChannels * (x1 + 1)];

	f_xy = saturate_cast<uchar>(f_xy);
	
	return uchar(round(f_xy));
}
//----------------

NearestNeighborInterpolate::NearestNeighborInterpolate() {}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels) {
	
	int x = int(round(tx));
	int y = int(round(ty));
	
	int index = y * srcWidthStep + x * nChannels;
	return pSrc[index];
}
//----------------

AffineTransform::AffineTransform() {
	this->_matrixTransform = Mat::eye(3, 3, CV_32FC1);
}
AffineTransform::~AffineTransform() {}

void AffineTransform::Translate(float dx, float dy)
{
	/* matrix cần sử dụng
		1 0 dx
		0 1 dy
		0 0 1
	*/
	Mat matrix = Mat::eye(3, 3, CV_32FC1); // tạo identity matrix
	matrix.at<float>(0, 2) = dx;
	matrix.at<float>(1, 2) = dy;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}
void AffineTransform::Rotate(float angle)//xây dựng matrix transform cho phép xoay 1 góc angle
{
	/* matrix cần sử dụng
		cos  sin  0
		-sin cos  0
		 0    0   1
	*/

	Mat matrix = Mat::zeros(3, 3, CV_32FC1);
	matrix.at<float>(0, 0) = cos(angle);
	matrix.at<float>(0, 1) = -sin(angle);
	matrix.at<float>(1, 0) = sin(angle);
	matrix.at<float>(1, 1) = cos(angle);
	matrix.at<float>(2, 2) = 1;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}
void AffineTransform::Scale(float sx, float sy)//xây dựng matrix transform cho phép tỉ lệ theo hệ số 
{
	/* matrix cần sử dụng
		sx 0 0
		0 sy 0
		0  0 1
	*/

	Mat matrix = Mat::zeros(3, 3, CV_32FC1);
	matrix.at<float>(0, 0) = sx;
	matrix.at<float>(1, 1) = sy;
	matrix.at<float>(2, 2) = 1;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}
void AffineTransform::TransformPoint(float& x, float& y)//transform 1 điểm (x,y) theo matrix transform đã có
{
	// Nhân vector [x, y, 1] với _matrixTransform
	float res_x = 0, res_y = 0;
	res_x = _matrixTransform.at<float>(0, 0) * x + _matrixTransform.at<float>(0, 1) * y + _matrixTransform.at<float>(0, 2);
	res_y = _matrixTransform.at<float>(1, 0) * x + _matrixTransform.at<float>(1, 1) * y + _matrixTransform.at<float>(1, 2);

	x = res_x;
	y = res_y;
}
//----------------

GeometricTransformer::GeometricTransformer() {}
GeometricTransformer::~GeometricTransformer() {}

int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, 
AffineTransform* transformer,PixelInterpolate* interpolator)
{
	if (beforeImage.data == NULL || beforeImage.channels() != afterImage.channels())
	{
		cout << "Image Null in transform fuction" << endl;
		return 0;
	}

	// afterImage
	int dstRow = afterImage.rows;
	int dstCol = afterImage.cols;
	int dstWidthStep = static_cast<int>(afterImage.step1());

	// beforeImage
	int srcRow = beforeImage.rows;
	int srcCol = beforeImage.cols;
	int srcWidthStep = static_cast<int>(beforeImage.step1());

	int nChannel = beforeImage.channels();

	uchar* pSrcData = (uchar*)beforeImage.data;
	uchar* pDstData = (uchar*)afterImage.data;

	//Làm trên ảnh afterImage (destinateImage)
	for (int y = 0; y < dstRow; y++)
	{
		for (int x = 0; x < dstCol; x++)
		{
			// ánh xạ ngược tìm tọa độ ảnh gốc
			float tx = static_cast<float>(x), ty = static_cast<float>(y);
			transformer->TransformPoint(tx, ty);

			// Nội suy
			for (int k = 0; k < nChannel; k++)
			{
				uchar color = 0;
				if (tx >= 0 && tx <= srcCol && ty >= 0 && ty <= srcRow)
				{
					color = interpolator->Interpolate(tx, ty, pSrcData+k, srcWidthStep, nChannel);
				}
				// Gán màu cho ảnh after
				int index = y * dstWidthStep + x * nChannel + k;
				pDstData[index] = color;
			}
		}
	}
	
	return 1;
}

int GeometricTransformer::RotateKeepImage(
	const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		cout << "Wrong srcImg" << endl;
		return 0;
	}

	// srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// dstRow, dstCol
	float w = float(srcCol);
	float h = float(srcRow);

	float abs_cos = abs(cos(angle));
	float abs_sin = abs(sin(angle));

	// New width and height
	float bound_w = int(srcRow * abs_sin + srcCol * abs_cos);
	float bound_h = int(srcRow * abs_cos + srcCol * abs_sin);

	int dstRow = int(ceil(bound_h));
	int dstCol = int(ceil(bound_w));

	// tạo dstImg
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-bound_w / 2, -bound_h / 2);
	affineTrans->Rotate(angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;

	return 1;
}

int GeometricTransformer::RotateUnkeepImage(
	const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		cout << "Null Img at RotateUnkeepImage fuction" << endl;
		return 0;
	}
	
	// srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// dstRow, dstCol
	int dstRow = srcRow;
	int dstCol = srcCol;
	
	// Tạo dstImg
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-srcCol / 2.0f, -srcRow / 2.0f);
	affineTrans->Rotate(angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage,
	float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || sx < 0.001 || sy < 0.001)
	{
		cout << "Check your srcImage data or sx < 0.001 or sy < 0.001" << endl;
		return 0;
	}

	// srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// dstRow, dstCol
	int dstRow = int(ceil(srcRow * sy));
	int dstCol = int(ceil(srcCol * sx));

	// Tạo dstImg
	dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Scale(1 / sx, 1 / sy);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}
int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight,
	PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || newWidth <= 0 || newHeight <= 0)
	{
		cout << "Check your srcImage data or newWidth < 0 or newHeight < 0.0" << endl;
		return 0;
	}
	
	// srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Thông số cho hàm scale
	float sx = float(newWidth) / srcCol;
	float sy = float(newHeight) / srcRow;

	// Dùng hàm Scale để resize ảnh
	int ret = this->Scale(srcImage, dstImage, sx, sy, interpolator);

	return ret;
}
int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage,
	bool direction, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
		return 0;

	int height = srcImage.rows;
	int width = srcImage.cols;
	int nChannels = srcImage.channels();

	// Tạo dstImg
	dstImage = Mat(height, width, CV_8UC3, Scalar(0));

	//Lấy widthStep của srcImage và dstImage
	int dstWidthStep = static_cast<int>(dstImage.step1());
	int srcWidthStep = static_cast<int>(srcImage.step1());

	uchar* pSrcData = srcImage.data;
	uchar* pDstData = dstImage.data;

	//Horizontal
	if (direction == 1) {
		for (int y = 0; y < height; y++)
			for (int x1 = 0, x2 = width - 1; x1 <= x2; x1++, x2--) {
				dstImage.at<cv::Vec3b>(y, x1) = srcImage.at<cv::Vec3b>(y, x2);
				dstImage.at<cv::Vec3b>(y, x2) = srcImage.at<cv::Vec3b>(y, x1);
			}
	}
	//Verticle
	else {
		for (int x = 0; x < width; x++)
			for (int y1 = 0, y2 = height - 1; y1 <= y2; y1++, y2--) {
				dstImage.at<cv::Vec3b>(y1, x) = srcImage.at<cv::Vec3b>(y2, x);
				dstImage.at<cv::Vec3b>(y2, x) = srcImage.at<cv::Vec3b>(y1, x);
			}
	}

	return 1;
}

//----
Mat matrixDot(const Mat& mat1, const Mat& mat2)
{
	Mat result = Mat(3, 3, CV_32FC1, Scalar(0));

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float sum = 0;
			for (int k = 0; k < 3; k++)
			{
				sum += mat1.at<float>(i, k) * mat2.at<float>(k, j);
			}
			result.at<float>(i, j) = sum;
		}
	}
	return result;
}