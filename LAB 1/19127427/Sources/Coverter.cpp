#include "Converter.h"
#include <algorithm>

Converter::Converter(){

}
Converter::~Converter(){
    
}

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage){
    if ( !sourceImage.data )
    {
        printf("No image data \n");
        return 1;
    }
    
    int width = sourceImage.cols, height = sourceImage.rows;
    destinationImage = Mat(height, width, CV_8UC1);
    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();
    
    for(int y = 0; y < height; y++){
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        uchar* pDstRow = destinationImage.ptr<uchar>(y);
        for(int x = 0; x < width; x++, pSrcRow += srcChanels, pDstRow += dstChanels){
            uchar B = pSrcRow[0];
            uchar G = pSrcRow[1];
            uchar R = pSrcRow[2];
            pDstRow[0] = (B * 0.114f + G * 0.587f + R * 0.299f);
        }
    }
    return 0;
}

int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 1;
    }

    int width = sourceImage.cols, height = sourceImage.rows;
    destinationImage = Mat(height, width, CV_8UC3);
    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();

    for (int y = 0; y < height; y++) {
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        uchar* pDstRow = destinationImage.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pSrcRow += srcChanels, pDstRow += dstChanels) {
            uchar B = pSrcRow[0];
            uchar G = pSrcRow[1];
            uchar R = pSrcRow[2];
            double b = B / 255.0;
            double g = G / 255.0;
            double r = R / 255.0;

            // h, s, v = hue, saturation, value
            double cmax = std::max(r, std::max(g, b)); // maximum of r, g, b
            double cmin = std::min(r, std::min(g, b)); // minimum of r, g, b
            double diff = cmax - cmin; // diff of cmax and cmin.
            double h = -1, s = -1;

            //v
            double v = cmax;

            // if cmax equal zero
            if (cmax == 0)
                s = 0;
            else
                s = (diff / cmax);

            // if cmax equal r then compute h
            if (cmax == r) {
                h = 60 * (g - b) / diff;
            }
            // if cmax equal g then compute h
            else if (cmax == g) {
                h = 120 + 60 * (b - r) / diff;
            }
            // if cmax equal b then compute h
            else if (cmax == b) {
                h = 240 + 60 * (r - g) / diff;
            }

            if (h < 0) h += 360;
            
            pDstRow[0] = (uchar)(h / 2);
            pDstRow[1] = (uchar)(255 * s);
            pDstRow[2] = (uchar)(255 * v);
        }
    }
    return 0;
}

int HSV2RGB(const Mat& sourceImage, Mat& destinationImage) {

    if (!sourceImage.data)
    {
        printf("No image data \n");
        return 1;
    }

    int width = sourceImage.cols, height = sourceImage.rows;
    destinationImage = Mat(height, width, CV_8UC3);
    int srcChanels = sourceImage.channels();
    int dstChanels = destinationImage.channels();

    for (int y = 0; y < height; y++) {
        const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
        uchar* pDstRow = destinationImage.ptr<uchar>(y);
        for (int x = 0; x < width; x++, pSrcRow += srcChanels, pDstRow += dstChanels) {
            float h = pSrcRow[0] * 2.0f; // 0-360
            float s = pSrcRow[1] / 255.0f; // 0.0-1.0
            float v = pSrcRow[2] / 255.0f; // 0.0-1.0

            float r, g, b; // 0.0-1.0

            int   hi = (int)(h / 60.0f) % 6;
            float f = (h / 60.0f) - hi;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));

            switch (hi) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
            }

            pDstRow[2] = (unsigned char)(r * 255); // dst_r : 0-255
            pDstRow[1] = (unsigned char)(g * 255); // dst_r : 0-255
            pDstRow[0] = (unsigned char)(b * 255); // dst_r : 0-255
        }
    }
    return 0;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type){
    
    if (type)
        return this->RGB2HSV(sourceImage, destinationImage);
    else
        return this->RGB2GrayScale(sourceImage, destinationImage);
}


