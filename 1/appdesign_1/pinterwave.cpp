#include "pinterwave.h"
#include <QPainter>
#include <algorithm>
#include <iostream>
#include <limits>
#include <QDebug>
#include <QPair>
#include <QSlider>
#include <QTimer>
#include <QVector>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QHBoxLayout>
#include "picture_process.h"
#include "grid_layout_panel.h"
#include "findpeaks.h"
#include <opencv2/imgproc.hpp>  // 需要包含该头文件用于形态学操作
// PlotWidget Constructor
PlotWidget::PlotWidget(Ui::MainWindow *ui, QWidget *parent) : QWidget(parent), ui(ui) {}

// Set plot data
PlotWidget* PlotWidget::setPlotData(const QVector<QPointF> &plotdata, const QVector<QVector<int>> &data) {
    points = plotdata;
    datas = data;
    update();  // Trigger a repaint
    return this;
}

// Delay function
void delay(int milliseconds) {
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}

// Paint Event
void PlotWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int margin = 40;
    int width = this->width() - 2 * margin;
    int height = this->height() - 2 * margin;
    QRect plotArea(margin, margin, width, height);

    // Get min and max values of the points
    double minX = points[0].x(), maxX = points[0].x();
    double minY = points[0].y(), maxY = points[0].y();

    for (const QPointF& point : points) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // Draw grid, axes, and ticks
    drawGrid(painter, plotArea);
    drawAxes(painter, plotArea);
    drawTicks(painter, plotArea, minX, maxX, minY, maxY);
    combinedPeaks = drawDataPoints(painter, plotArea, minX, maxX, minY, maxY);

    // Image processing (move this to a separate method or thread)
    QVector<QVector<QVector<double>>> pixmapData = getDatas(datas, combinedPeaks);
    cv::Mat combinedImage[pixmapData.size()][3];  // Store combined images
    cv::Mat dst1[pixmapData.size()][3];

    auto myPanel2 = new PictureProcess();
    auto myPanel1 = new GrideLayoutPanel();
    int len = pixmapData.size();
    std::cout << "len****************" << len << std::endl;
    int index;
    // Suggestion: Move this block outside paintEvent() or into a separate thread
    for (int k = 0; k < len; k++) {
        for (int i = 0; i < 3; i++) {
            combinedImage[k][i] = myPanel1->combineImages(pixmapData[k][i]);
            if (combinedImage[k][i].empty()) {
                qDebug() << "Unable to create combined image, index" << i << "set" << k;
                continue;
            }

            dst1[k][i] = myPanel2->processImage(combinedImage[k][i], 2);

            QString base64Image = convertMatToBase64(combinedImage[k][i]);
            if (!base64Image.isEmpty()) {
                updateImageLabel(ui->img_label_2, base64Image);
            }

            QString processedBase64Image = convertMatToBase64(dst1[k][i]);
            if (!processedBase64Image.isEmpty()) {
                updateImageLabel(ui->img_label_3, processedBase64Image);
            }
            delay(120);  // Consider refactoring to avoid delays in paintEvent
            index=pixmapData[k][i][0];

        }
    }


    std::cout<<"************"<<index<<std::endl;
    // Clean up
    delete myPanel2;
    delete myPanel1;
}


// 绘制网格
void PlotWidget::drawGrid(QPainter &painter, const QRect &plotArea) {
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    int gridCount = 10;
    for (int i = 0; i <= gridCount; ++i) {
        int x = plotArea.left() + i * (plotArea.width() / gridCount);
        int y = plotArea.top() + i * (plotArea.height() / gridCount);
        painter.drawLine(x, plotArea.top(), x, plotArea.bottom());
        painter.drawLine(plotArea.left(), y, plotArea.right(), y);
    }
}

// 绘制坐标轴
void PlotWidget::drawAxes(QPainter &painter, const QRect &plotArea) {
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(plotArea.bottomLeft(), plotArea.bottomRight());  // X轴
    painter.drawLine(plotArea.bottomLeft(), plotArea.topLeft());      // Y轴

    painter.drawText(plotArea.right() + 10, plotArea.bottom() + 10, "X");
    painter.drawText(plotArea.left() - 20, plotArea.top() - 10, "Y");
}

// 绘制刻度
void PlotWidget::drawTicks(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY) {
    painter.setPen(QPen(Qt::black, 1));

    int tickSize = 5;
    int tickCount = 10;

    // X轴刻度
    for (int i = 0; i <= tickCount; ++i) {
        double value = minX + i * (maxX - minX) / tickCount;
        int x = plotArea.left() + i * (plotArea.width() / tickCount);
        painter.drawLine(x, plotArea.bottom(), x, plotArea.bottom() + tickSize);
        painter.drawText(x - 10, plotArea.bottom() + 20, QString::number(value, 'f', 1));
    }

    // Y轴刻度
    for (int i = 0; i <= tickCount; ++i) {
        double value = minY + i * (maxY - minY) / tickCount;
        int y = plotArea.bottom() - i * (plotArea.height() / tickCount);
        painter.drawLine(plotArea.left() - tickSize, y, plotArea.left(), y);
        painter.drawText(plotArea.left() - 30, y + 5, QString::number(value, 'f', 1));
    }
}

// 绘制数据点并检测峰值
QVector<QVector<double>> PlotWidget::drawDataPoints(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY) {
    painter.setPen(QPen(Qt::green, 1));
    QVector<QPointF> normalizedPoints;

    // 平滑数据
    QVector<QPointF> smoothedPoints = smoothData(points, 5);

    // 将平滑后的点归一化到绘图区域
    for (const QPointF &point : smoothedPoints) {
//        double x = mapValueToPlotArea(point.x(), minX, maxX, plotArea.left(), plotArea.right());
//        double x = mapValueToPlotArea(point.x(), minX, maxX, plotArea.left(), plotArea.right());
        int x=point.x();
        double y = mapValueToPlotArea(point.y(), minY, maxY, plotArea.bottom(), plotArea.top());
        normalizedPoints.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
    }

    // 绘制数据线
    for (int i = 0; i < normalizedPoints.size() - 1; ++i) {
        painter.drawLine(normalizedPoints[i], normalizedPoints[i + 1]);
    }

    // 使用邻域比较法检测正峰值和负峰值
    QVector<QPointF> positivePeaks;
    QVector<QPointF> negativePeaks;
    for (int i = 1; i < normalizedPoints.size() - 1; ++i) {
        QPointF currentPoint = normalizedPoints[i];
        QPointF prevPoint = normalizedPoints[i - 1];
        QPointF nextPoint = normalizedPoints[i + 1];

        if (currentPoint.y() > prevPoint.y() && currentPoint.y() > nextPoint.y()) {
            // 当前点是局部最大值，检测为正峰值
            negativePeaks.append(currentPoint);
        }

        if (currentPoint.y() < prevPoint.y() && currentPoint.y() < nextPoint.y()) {
            // 当前点是局部最小值，检测为负峰值

            positivePeaks.append(currentPoint);
        }
    }

//    for (int i=1;i<negativePeaks.size();i++) {
//        if(negativePeaks[i].x()==negativePeaks[i-1].x()) negativePeaks.removeAt(i-1);
//        if(negativePeaks[i].x()==negativePeaks[i+1].x()) negativePeaks.removeAt(i);
//    }

//    for (int i=1;i<positivePeaks.size();i++) {
//        if(positivePeaks[i].x()==positivePeaks[i-1].x()) positivePeaks.removeAt(i-1);
//        if(positivePeaks[i].x()==positivePeaks[i+1].x()) positivePeaks.removeAt(i);
//    }

    std::for_each(negativePeaks.begin(),negativePeaks.end(),[](QPointF val){
        std::cout<<'('<<val.x()<<","<<val.y()<<')';
    });
    std::cout<<negativePeaks.size()<<std::endl;
    std::for_each(positivePeaks.begin(),positivePeaks.end(),[](QPointF val){
        std::cout<<'('<<val.x()<<","<<val.y()<<')';
    });
    std::cout<<positivePeaks.size()<<std::endl;
    for (int i = 0; i < negativePeaks.size(); ) {
        bool removed = false;  // 记录是否进行了删除操作

        for (int j = 0; j < positivePeaks.size(); ) {
            if (positivePeaks[j].x() == negativePeaks[i].x()) {
//                negativePeaks.removeAt(i);  // 删除负峰值
                positivePeaks.removeAt(j);  // 删除正峰值
                removed = true;  // 标记删除
                break;  // 跳出内层循环，重新检查下一个负峰值
            } else {
                ++j;  // 只有在没有删除时才增加j
            }
        }

        if (!removed) {
            ++i;  // 只有在没有删除时才增加i
        }
    }
    for (int i = 0; i < negativePeaks.size(); ) {
        bool removed = false;  // 记录是否进行了删除操作

        for (int j = 0; j < positivePeaks.size(); ) {
            if (abs(positivePeaks[j].y() - negativePeaks[i].y())<2) {
//                negativePeaks.removeAt(i);  // 删除负峰值
                positivePeaks.removeAt(j);  // 删除正峰值
                removed = true;  // 标记删除
                break;  // 跳出内层循环，重新检查下一个负峰值
            } else {
                ++j;  // 只有在没有删除时才增加j
            }
        }

        if (!removed) {
            ++i;  // 只有在没有删除时才增加i
        }
    }
    std::for_each(negativePeaks.begin(),negativePeaks.end(),[](QPointF val){
        std::cout<<'('<<val.x()<<","<<val.y()<<')';
    });
    std::cout<<negativePeaks.size()<<std::endl;
    std::for_each(positivePeaks.begin(),positivePeaks.end(),[](QPointF val){
        std::cout<<'('<<val.x()<<","<<val.y()<<')';
    });
    std::cout<<positivePeaks.size()<<std::endl;
    // 绘制正峰值
    painter.setPen(QPen(Qt::red, 2));
    for (const QPointF &peak : positivePeaks) {
        painter.drawEllipse(peak, 2,2);
    }

    // 绘制负峰值
    painter.setPen(QPen(Qt::blue, 2));
    for (const QPointF &peak : negativePeaks) {
        painter.drawEllipse(peak, 1, 1);
    }

    // 绘制包络线
//    painter.setPen(QPen(Qt::green, 1, Qt::DashLine));
//    drawEnvelope(painter, positivePeaks, negativePeaks);

    // 返回峰值组合
    return combineAndSortPeaks(negativePeaks, positivePeaks);
}
// 绘制包络线
void PlotWidget::drawEnvelope(QPainter &painter, const QVector<QPointF> &positivePeaks, const QVector<QPointF> &negativePeaks) {
    // 绘制连接正峰值的上包络线
    if (!positivePeaks.isEmpty()) {
        QPointF prevPoint = positivePeaks.first();
        for (int i = 1; i < positivePeaks.size(); ++i) {
            painter.drawLine(prevPoint, positivePeaks[i]);
            prevPoint = positivePeaks[i];
        }
    }

    // 绘制连接负峰值的下包络线
    if (!negativePeaks.isEmpty()) {
        QPointF prevPoint = negativePeaks.first();
        for (int i = 1; i < negativePeaks.size(); ++i) {
            painter.drawLine(prevPoint, negativePeaks[i]);
            prevPoint = negativePeaks[i];
        }
    }
}

// Combine and Sort Peaks
QVector<QVector<double>> PlotWidget::combineAndSortPeaks(const QVector<QPointF>& negativePeaks, const QVector<QPointF>& positivePeaks) {
    QVector<QVector<double>> result;
    int negSize = negativePeaks.size();
    int posSize = positivePeaks.size();

    for(int i=0,j=0;i<posSize&&j<negSize;j++){
            if(positivePeaks[i].x()>negativePeaks[j+1].x())continue;
            QVector<double> result1;
            result1.push_back(negativePeaks[j].x());
            result1.push_back(positivePeaks[i].x());
            result.append(result1);
            i++;
        }

    std::for_each(result.begin(),result.end(),[](QVector<double>vals){
        std::for_each(vals.begin(),vals.end(),[](double val){
            std::cout<<val<<" ";
        });
        std::cout<<std::endl;
    });
//    std::cout<<"________________"<<result.size()<<std::endl;
    return result;
}

// 平滑数据
QVector<QPointF> PlotWidget::smoothData(const QVector<QPointF> &data, int windowSize) {
    QVector<QPointF> smoothedData = data;
    for (int i = windowSize; i < data.size() - windowSize; ++i) {
        int sumY = 0;
        for (int j = -windowSize; j <= windowSize; ++j) {
            sumY += data[i + j].y();
        }
        int avgY = sumY / (2 * windowSize + 1);
        smoothedData[i].setY(avgY);
    }
    return smoothedData;
}

// Map Value to Plot Area
double PlotWidget::mapValueToPlotArea(double value, double minValue, double maxValue, double plotMin, double plotMax) {
    if (maxValue == minValue) return plotMin;  // Avoid division by zero
    return plotMin + ((value - minValue) / (maxValue - minValue)) * (plotMax - plotMin);
}

// Data processing for images (example implementation)
QVector<QVector<QVector<double>>> PlotWidget::getDatas(QVector<QVector<int>> data, QVector<QVector<double>> result3) {
    int len = result3.size();

    QVector<QVector<QVector<double>>> normalizedData(len, QVector<QVector<double>>(3, QVector<double>(data[0].size())));
    QVector<QVector<int>> ids;

    for (int k = 0; k < len; k++) {
        QVector<int> id = {int(result3[k][0]),
                           int(result3[k][0] + (result3[k][1] - result3[k][0]) / 2),
                           int(result3[k][1])};

        ids.append(id);
        for (int j = 0; j < 3; j++) {
            int min, max;

            if (findMinMax(data[ids[k][j]], min, max) != 0) {
                qWarning() << "Failed to find min and max values.";
                return QVector<QVector<QVector<double>>>();
            }

            if (max == min) {
                qWarning() << "Max and Min are equal, normalization is not possible.";
                return QVector<QVector<QVector<double>>>();
            }

            normalizedData[k][j][0] = data[ids[k][j]][0];

            for (int i = 1; i < 257; ++i) {
                normalizedData[k][j][i] = double(data[ids[k][j]][i] - min) / double(max - min);
            }
        }
    }
//    std::cout<<"________________"<<normalizedData.size()<<std::endl;
    return normalizedData;
}

// Find Min and Max
int PlotWidget::findMinMax(const QVector<int>& data, int& min, int& max) {
    if (data.isEmpty()) {
        return -1;
    }
    min = std::numeric_limits<int>::max();
    max = std::numeric_limits<int>::min();

    for (int value : data) {
        if (value < min) {
            min = value;
        }
        if (value > max) {
            max = value;
        }
    }
    return 0;
}

// Convert Mat to Base64 (example implementation)
QString PlotWidget::convertMatToBase64(const cv::Mat &mat) {
    if (mat.empty()) {
        qDebug() << "Empty image encountered, skipping conversion.";
        return "";  // Return an empty string if the image is empty
    }

    std::vector<uchar> buf;
    cv::imencode(".png", mat, buf);
    return QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(buf.data()), buf.size()).toBase64());
}

// Update Image Label
void PlotWidget::updateImageLabel(QTextEdit *label, const QString &base64Image) {
    QString html = "<img src='data:image/png;base64," + base64Image + "' />";
    label->setHtml(html);
}
