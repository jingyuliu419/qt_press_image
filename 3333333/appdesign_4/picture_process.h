#ifndef PICTURE_PROCESS_H
#define PICTURE_PROCESS_H

#include <QWidget>
#include <QLabel>
#include <opencv2/opencv.hpp>


class PictureProcess : public QWidget {
    Q_OBJECT

public:
    explicit PictureProcess(QWidget *parent = nullptr);
    cv::Mat processImage(cv::Mat src, int times);
    cv::Mat gaosiImage(cv::Mat src, cv::Size kernelSize);
    void jetColorMap(double gray, int &r, int &g, int &b);
    cv::Mat applyJetColorMap(const cv::Mat &grayImage);
    cv::Mat biCubicInterp(const cv::Mat &src, double times);
    cv::Mat biCubicInterp1(const cv::Mat &src, double times);
private:
    void initUIs();
    double W(double x);


    QLabel *imagePathLabel;
    QLabel *imageDisplayLabel;
};

#endif // PICTURE_PROCESS_H
