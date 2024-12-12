#ifndef GRID_LAYOUT_PANEL_H
#define GRID_LAYOUT_PANEL_H
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QPixmap>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <iostream>
#include<opencv2/opencv.hpp>
class GrideLayoutPanel : public QWidget {
    Q_OBJECT

public:
    explicit GrideLayoutPanel(QWidget *parent = nullptr);
    cv::Mat select_image_dir();
    int findMinMax(const QVector<int>& data, int& min, int& max);
    cv::Mat combineImages(QVector<double>& data);




private:

    QLabel *img_label;
    QVector<QLabel*> img_labels;
    cv::Mat getSingleImage(const QVector<int>& dataRow);

    void initUIs();
};

#endif // GRID_LAYOUT_PANEL_H

