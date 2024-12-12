#include "picture_process.h"
#include <QImage>
#include <QPixmap>
#include <iostream>
#include <QVBoxLayout>
#include <cmath>
#include <algorithm>

PictureProcess::PictureProcess(QWidget *parent) : QWidget(parent) {
    std::cout << "create MyCustomPanel instance..." << std::endl;
    initUIs();
}

void PictureProcess::initUIs() {
    imageDisplayLabel = new QLabel();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(imageDisplayLabel);
    setLayout(mainLayout);
}

cv::Mat PictureProcess::processImage(cv::Mat src, int times) {
    if (src.empty()) {
        std::cerr << "Failed to open image file." << std::endl;
        return cv::Mat();
    }

    // Resize the source image using bicubic interpolation
    cv::Mat block1 = gaosiImage(src, cv::Size(15, 15));
    cv::Mat resizedImage = biCubicInterp1(block1, times);
    cv::Mat block0 = gaosiImage(resizedImage, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
//    block0 = gaosiImage(block0, cv::Size(15, 15));
    cv::Mat jetBlock1 = applyJetColorMap(block0);





    // Return the combined image as a cv::Mat
    return jetBlock1;
}

double PictureProcess::W(double x) {
    const double a = -0.75;
    x = std::abs(x);
    if (x <= 1) {
        return (a + 2) * std::pow(x, 3) - (a + 3) * std::pow(x, 2) + 1;
    } else if (x < 2) {
        return a * std::pow(x, 3) - 5 * a * std::pow(x, 2) + 8 * a * x - 4 * a;
    }
    return 0.0;
}


cv::Mat PictureProcess::biCubicInterp1(const cv::Mat &src, double times){
    int width = src.cols;
    int height = src.rows;
    int width1 = width*times;
    int height1 = height*times;
    cv::Mat dst;
    cv::resize(src,dst,cv::Size(width1,height1),0,0,cv::INTER_CUBIC);
    return dst;
}

cv::Mat PictureProcess::gaosiImage(cv::Mat src, cv::Size kernelSize) {
    cv::Mat result;
    GaussianBlur(src, result, kernelSize, 0, 0,cv::BORDER_REFLECT);

//    cv::Mat sharpened;
//        cv::Laplacian(src, sharpened, CV_16S, 3);
//        cv::convertScaleAbs(sharpened, sharpened);
//        cv::Mat result = src - sharpened;

//        // 然后应用5x5高斯滤波
//        cv::Mat dst;
//        cv::GaussianBlur(result, dst, kernelSize, 0);
    return result;
}

void PictureProcess::jetColorMap(double gray, int &r, int &g, int &b) {
    if (gray >= 0 && gray < 63) {
        b = 255;
        g = static_cast<int>(4 * gray);
        r = 0;
    } else if (gray >= 63 && gray < 127) {
        b = static_cast<int>(255 - 4 * (gray - 63));
        g = 255;
        r = 0;
    } else if (gray >= 127 && gray < 191) {
        b = 0;
        g = 255;
        r = static_cast<int>(4 * (gray - 127));
    } else if (gray >= 191 && gray <= 255) {
        b = 0;
        g = static_cast<int>(255 - 4 * (gray - 191));
        r = 255;
    } else {
        r = 0;
        g = 0;
        b = 0;
    }
}

cv::Mat PictureProcess::applyJetColorMap(const cv::Mat &grayImage) {
    cv::Mat colorImage(grayImage.size(), CV_8UC3);

    for (int y = 0; y < grayImage.rows; ++y) {
        for (int x = 0; x < grayImage.cols; ++x) {
            double grayValue = grayImage.at<uchar>(y, x);
            int r, g, b;
            jetColorMap(grayValue, r, g, b);
            colorImage.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
        }
    }

    return colorImage;
}

