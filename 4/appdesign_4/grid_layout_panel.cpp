#include "grid_layout_panel.h"
#include <limits>
#include <algorithm>
#include <opencv2/opencv.hpp>



GrideLayoutPanel::GrideLayoutPanel(QWidget *parent) : QWidget(parent) {
    std::cout << "create MyCustomPanel instance..." << std::endl;
    initUIs();
}

void GrideLayoutPanel::initUIs(){


    QHBoxLayout *hbox = new QHBoxLayout();
    QWidget *panel1 = new QWidget();
    hbox->addStretch(1);
    panel1->setLayout(hbox);

    img_label = new QLabel(this);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(panel1);
    vbox->addWidget(img_label);
    vbox->addStretch(1);
    this->setLayout(vbox);
}

cv::Mat GrideLayoutPanel::getSingleImage(const QVector<int>& dataRow) {
    const int blockSize = 22;
    const int gridSize = 16;
    const int imageSize = blockSize * gridSize;
    //    std::for_each(dataRow.begin(),dataRow.end(),[](int val){
    //        std::cout<<val<<" ";
    //    });
    cv::Mat grayImage(imageSize, imageSize, CV_8UC1); // 创建灰度图像

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int index = i * gridSize + j;
            if (index < dataRow.size()) {
                int grayValue = dataRow[index];

                cv::Rect blockRect(j * blockSize, i * blockSize, blockSize, blockSize);
                grayImage(blockRect).setTo(cv::Scalar(grayValue)); // 设置灰度值
            }
        }
    }
    return grayImage;
}

#include <opencv2/opencv.hpp>
#include <QVector>

cv::Mat GrideLayoutPanel::combineImages(QVector<double>& data) {
    // 创建一个 16x22 格的图像，每格是1像素大小
    int gridSize = 16 * 22;
    cv::Mat combinedImage(gridSize, gridSize, CV_8UC1, cv::Scalar(255)); // 初始化为白色背景 (255)

    // 将 data 归一化到 0-255 的灰度范围
    QVector<int> intData(data.size());
    for (int i = 1; i < data.size(); ++i) { // Index should start from 0
        intData[i] = static_cast<int>(data[i] * 255.0); // 归一化
    }

    // 获取单个图像 (假设 getSingleImage 返回一个 cv::Mat)
    cv::Mat singleImage = getSingleImage(intData);

    // 将单个图像放置到 combinedImage 的指定位置
    cv::Rect roi(0, 0, singleImage.cols, singleImage.rows); // 定义放置的位置
    singleImage.copyTo(combinedImage(roi));

    // 旋转 combinedImage 180 度
    cv::Mat rotatedImage;
    cv::rotate(combinedImage, rotatedImage, cv::ROTATE_180);

    // 返回旋转后的图像
    return rotatedImage;
//    return combinedImage;
}




