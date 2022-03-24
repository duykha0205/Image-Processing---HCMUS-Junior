#include "ColorTransformer.h"
#include "Converter.h"

ColorTransformer::ColorTransformer() {

}
ColorTransformer::~ColorTransformer() {

}

int ColorTransformer:: ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 0;
    }

    int width = sourceImage.cols, height = sourceImage.rows;
    
    if (sourceImage.channels() == 1) {
        destinationImage = Mat(height, width, CV_8UC1);
    }
    else
    {
        destinationImage = Mat(height, width, CV_8UC3);
    }

    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();

    

    for (int y = 0; y < height; y++) {
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        uchar* pDstRow = destinationImage.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pSrcRow += srcChanels, pDstRow += dstChanels) {
            for (int z = 0; z < dstChanels; z++)
            {
                //pDstRow[z] = saturate_cast<uchar>(pSrcRow[z] + b);
                if (pSrcRow[z] + b > 255) {
                    pDstRow[z] = 255;
                }
                else {
                    pDstRow[z] = pSrcRow[z] + b;
                }
            }
        }
    }
    return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 0;
    }

    int width = sourceImage.cols, height = sourceImage.rows;

    if (sourceImage.channels() == 1) {
        destinationImage = Mat(height, width, CV_8UC1);
    }
    else
    {
        destinationImage = Mat(height, width, CV_8UC3);
    }

    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();

    for (int y = 0; y < height; y++) {
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        uchar* pDstRow = destinationImage.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pSrcRow += srcChanels, pDstRow += dstChanels) {
            for (int z = 0; z < dstChanels; z++)
            {
                /*if (pSrcRow[z] * c > 255) {
                    pDstRow[z] = 255;
                }
                else {
                    pDstRow[z] = pSrcRow[z] * c;
                }*/

                pDstRow[z] = saturate_cast<uchar>(pSrcRow[z] * c);
            }
        }
    }
    return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 0;
    }

    int width = sourceImage.cols, height = sourceImage.rows;

    if (sourceImage.channels() == 1)
        histMatrix = Mat::zeros(1, 256, CV_32F);
    else
        histMatrix = Mat::zeros(3, 256, CV_32F);

    int srcChanels = sourceImage.channels();
    int histChanels = histMatrix.channels();


    double value = 0;
    for (int y = 0; y < height; y++) {
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        //uchar* hisrRow = histMatrix.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pSrcRow += srcChanels) {
            for (int z = 0; z < srcChanels; z++)
            {
                histMatrix.at<float>(z, pSrcRow[z]) += 1;
            }
        }
    }

    //std::cout << cv::sum(histMatrix)[0] << std::endl;
    //std::cout << height * width << std::endl;

    /*for (int i = 0; i < histMatrix.rows; i++) {
        for (int j = 0; j < histMatrix.cols; j++) {
            std::cout << "\t\t" << "(" << i << " , " << j << "): " << histMatrix.at<float>(i, j) << "\t\t";
        }
        std::cout << "\n";
    }*/

    return 1;
}

Mat Equalization(const Mat& sourceImage) {
    Mat hist, weighted_hist;

    // calculate histogram of the image
    int hsize = 256;
    float range[] = { 0, 256 };  // the upper boundary is exclusive
    const float* histranges = range;
    calcHist(&sourceImage, 1, 0, Mat(), hist, 1, &hsize, &histranges);

    // calculate weighted histogram
    weighted_hist = hist / sum(hist);

    // calculate cumulative histogram
    Mat acc_hist = Mat::zeros(weighted_hist.size(), weighted_hist.type());
    acc_hist.at<float>(0) = weighted_hist.at<float>(0);
    for (int i = 1; i < 256; i++)
    {
        acc_hist.at<float>(i) = weighted_hist.at<float>(i) + acc_hist.at<float>(i - 1);
    }
    acc_hist = acc_hist * 255;

    // Mapping
    Mat imgClone = Mat::zeros(sourceImage.size(), CV_32FC1);
    sourceImage.convertTo(imgClone, CV_32FC1);
    Mat output = Mat::zeros(sourceImage.size(), CV_32FC1);
    for (int m = 0; m < sourceImage.rows; m++)
    {
        for (int n = 0; n < sourceImage.cols; n++)
        {
            output.at<float>(m, n) = acc_hist.at<float>(imgClone.at<float>(m, n));
        }
    }

    // quantize output
    output.convertTo(output, CV_8UC1);
    return output;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 0;
    }

    int width = sourceImage.cols, height = sourceImage.rows;

    if (sourceImage.channels() == 1) {
        destinationImage = Mat(height, width, CV_8UC1);
    }
    else
    {
        destinationImage = Mat(height, width, CV_8UC3);
    }

    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();

    if (sourceImage.channels() == 1) {
        destinationImage = Equalization(sourceImage);
    }
    else {
        Mat B(sourceImage.size(), CV_8UC1);
        Mat G(sourceImage.size(), CV_8UC1);
        Mat R(sourceImage.size(), CV_8UC1);

        for (int y = 0; y < height; y++) {
            const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
            for (int x = 0; x < width; x++, pSrcRow += srcChanels) {
                B.at<uchar>(y, x) = pSrcRow[0];
                G.at<uchar>(y, x) = pSrcRow[1];
                R.at<uchar>(y, x) = pSrcRow[2];
            }
        }

        B = Equalization(B);
        G = Equalization(G);
        R = Equalization(R);

        for (int y = 0; y < height; y++) {
            uchar* pDstRow = destinationImage.ptr<uchar>(y);
            for (int x = 0; x < width; x++, pDstRow += dstChanels) {
                pDstRow[0] = B.at<uchar>(y, x);
                pDstRow[1] = G.at<uchar>(y, x);
                pDstRow[2] = R.at<uchar>(y, x);
            }
        }
    }
    Mat histMatrix;
    this->CalcHistogram(destinationImage, histMatrix);
    this->DrawHistogram(histMatrix, destinationImage);
    return 1;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage) {
    if (histMatrix.data == NULL)
        return 0;

    int histSize = 256;
    int hist_w = 540, hist_h = 480;
    Scalar scalar[] = { Scalar(0, 0, 255) ,Scalar(0, 255, 0),Scalar(255, 0, 0) }; // red - green - blue

    double bin_w = cvRound((double)hist_w / histSize);
    if (histMatrix.rows == 1) {
        histImage = Mat(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
    }
    else {
        histImage = Mat(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
    }

    int histImagechannel = histImage.channels();

    normalize(histMatrix, histMatrix, 0, hist_h, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++)
    {
        for (int j = 0; j < histImagechannel; j++) {
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(histMatrix.at<float>(j, (i - 1)))),
                Point(bin_w * (i), hist_h - cvRound(histMatrix.at<float>(j, (i)))),
                scalar[j], 1, cv::LINE_AA);
        }

    }

    return 1;
}


