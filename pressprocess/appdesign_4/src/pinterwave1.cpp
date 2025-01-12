#include "pinterwave1.h"
#include <QPainter>
#include <algorithm>
#include <iostream>
#include <limits>
#include <QDebug>
#include <QPair>
#include <QSlider>
#include <QTimer>
#include <QVector>
#include<QMediaPlayer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QHBoxLayout>
#include<QTextCharFormat>
#include "picture_process.h"
#include "grid_layout_panel.h"
#include "findpeaks.h"
#include <opencv2/imgproc.hpp>  // 需要包含该头文件用于形态学操作
//#include "tts_thread.h"
#include <QCoreApplication>
#include <QThread>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// #include <errno.h>
// #include "include/qtts.h"
// #include "include/msp_cmn.h"
// #include "include/msp_errors.h"
#include "audioplayer.h"
#include <QMetaType>
// PlotWidget Constructor
//static bool isAudioThreadInitialized = false;

#include "worker.h" // Assuming Worker handles ONNX inference and image updates

PlotWidget1::PlotWidget1(Ui::MainWindow *ui, QWidget *parent)
    : QWidget(parent), ui(ui), audioThread(nullptr), audioPlayer(nullptr), onnxThread(nullptr), imageThread(nullptr), worker(nullptr) {
    qRegisterMetaType<cv::Mat>("cv::Mat");
    // Audio Thread Initialization
    if (!audioThread) {
        audioThread = new QThread(this);
        audioPlayer = new AudioPlayer();
        audioPlayer->moveToThread(audioThread);

        connect(audioThread, &QThread::started, []() {
            qDebug() << "Audio thread started.";
        });

        connect(audioThread, &QThread::finished, []() {
            qDebug() << "Audio thread finished.";
        });

        connect(this, &PlotWidget1::playAudioSignal, audioPlayer, &AudioPlayer::playAudio, Qt::QueuedConnection);

        audioThread->start();
        qDebug() << "Audio thread and player initialized in PlotWidget1.";
    }

    // ONNX Thread Initialization
    if (!onnxThread) {
        onnxThread = new QThread(this);
        worker = new Worker();
        worker->moveToThread(onnxThread);

        connect(onnxThread, &QThread::started, []() {
            qDebug() << "ONNX thread started.";
        });

        connect(onnxThread, &QThread::finished, []() {
            qDebug() << "ONNX thread finished.";
        });

        connect(this, &PlotWidget1::runOnnxSignal, worker, &Worker::runOnnxInfer, Qt::QueuedConnection);
        connect(worker, &Worker::inferenceCompleted, this, &PlotWidget1::handleOnnxResult);

        onnxThread->start();
        qDebug() << "ONNX thread initialized in PlotWidget1.";
    }

    // Image Thread Initialization
    if (!imageThread) {
        imageThread = new QThread(this);
        Worker* imageWorker = new Worker(); // Reusing the Worker for image updates
        imageWorker->moveToThread(imageThread);

        connect(imageThread, &QThread::started, []() {
            qDebug() << "Image thread started.";
        });

        connect(imageThread, &QThread::finished, []() {
            qDebug() << "Image thread finished.";
        });

        connect(this, &PlotWidget1::updateImageSignal, imageWorker, &Worker::updateImage, Qt::QueuedConnection);
        connect(imageWorker, &Worker::imageUpdateCompleted, this, &PlotWidget1::handleImageUpdate);

        imageThread->start();
        qDebug() << "Image thread initialized in PlotWidget1.";
    }
}

PlotWidget1::~PlotWidget1() {
    // Cleanup for audio thread
    if (audioThread) {
        if (audioThread->isRunning()) {
            qDebug() << "Stopping audio thread...";
            audioThread->quit();
            audioThread->wait();
        }
        delete audioThread;
        audioThread = nullptr;
    }
    if (audioPlayer) {
        delete audioPlayer;
        audioPlayer = nullptr;
    }

    // Cleanup for ONNX thread
    if (onnxThread) {
        if (onnxThread->isRunning()) {
            qDebug() << "Stopping ONNX thread...";
            onnxThread->quit();
            onnxThread->wait();
        }
        delete onnxThread;
        onnxThread = nullptr;
    }
    if (worker) {
        delete worker;
        worker = nullptr;
    }

    // Cleanup for image thread
    if (imageThread) {
        if (imageThread->isRunning()) {
            qDebug() << "Stopping image thread...";
            imageThread->quit();
            imageThread->wait();
        }
        delete imageThread;
        imageThread = nullptr;
    }

    qDebug() << "All threads and resources released in PlotWidget1.";
}
// Set plot data
PlotWidget1* PlotWidget1::setPlotData(const QPointF &plotdata, const QVector<int> &data) {
    point = plotdata;
    datas = data;
    update();  // Trigger a repaint
    return this;
}
// Delay function
void PlotWidget1::delay(int milliseconds) {
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}
void PlotWidget1::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int margin = 40;
    int width = this->width() - 2 * margin;
    int height = this->height() - 2 * margin;
    QRect plotArea(margin, margin, width, height);

    cv::Mat combinedImage1;
    cv::Mat dst1;

    auto myPanel2 = std::make_unique<PictureProcess>();
    auto myPanel1 = std::make_unique<GrideLayoutPanel>();

    QVector<double> normalizedData = getData(datas);
    combinedImage1 = myPanel1->combineImages(normalizedData);
    if (combinedImage1.empty()) {
        qDebug() << "Unable to create combined image, index";
    }

    dst1 = myPanel2->processImage(combinedImage1, 2);

    // Emit signal to update image in the image thread
    QString base64Image = convertMatToBase64(combinedImage1);
    if (!base64Image.isEmpty()) {
        emit updateImageSignal(base64Image); // Send image update to image thread
        // delay(10);
    }

    QString processedBase64Image = convertMatToBase64(dst1);
    if (!processedBase64Image.isEmpty()) {
        emit updateImageSignal(processedBase64Image); // Send processed image update to image thread
        delay(10);
    }

    // Emit signal to run ONNX inference in the ONNX thread
    if (point.y() > 25000) {
        qDebug() << "正在发射 runOnnxSignal";
        emit runOnnxSignal(dst1);
        delay(10);
    }

}

void PlotWidget1::handleOnnxResult(const QString& resultText) {
    QTextCursor cursor = ui->classtextEdit->textCursor();

    QTextCharFormat blackFormat;
    blackFormat.setFontPointSize(12);
    blackFormat.setForeground(QBrush(QColor(0, 0, 0))); // Black text

    QTextCharFormat redFormat;
    redFormat.setFontPointSize(40);
    redFormat.setForeground(QBrush(QColor(255, 0, 0))); // Red text

    if (resultText == "class1") {
        ui->classtextEdit->clear();
        cursor.insertText("正确\n", blackFormat);
    } else if (resultText == "class2") {
        ui->classtextEdit->clear();
        cursor.insertText("肘臂姿态错误\n", redFormat);
        emit playAudioSignal("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
    } else if (resultText == "class3") {
        ui->classtextEdit->clear();
        cursor.insertText("施力位置偏移\n", redFormat);
        emit playAudioSignal("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
    } else if (resultText == "class4") {
        ui->classtextEdit->clear();
        cursor.insertText("施力分散\n", redFormat);
        emit playAudioSignal("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
    } else if (resultText == "class5") {
        ui->classtextEdit->clear();
        cursor.insertText("手掌姿态错误\n", redFormat);
        emit playAudioSignal("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
    } else {
        cursor.insertText("未知结果\n", blackFormat);
    }
}

void PlotWidget1::handleImageUpdate(const QString& base64Image) {
    updateImageLabel(ui->img_label_3, base64Image);
}


void PlotWidget1::playAudioAndWait(const QString& audioPath)
{
    QMediaPlayer* player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(audioPath));
    player->setVolume(50); // Set volume to 50%

    QEventLoop loop;
    QObject::connect(player, &QMediaPlayer::stateChanged, &loop, [&](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            loop.quit();  // When audio stops, quit the event loop
        }
    });

    player->play();
    loop.exec();  // Wait until audio stops
    delete player; // Clean up player
}

// 绘制网格
void PlotWidget1::drawGrid(QPainter &painter, const QRect &plotArea) {
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
void PlotWidget1::drawAxes(QPainter &painter, const QRect &plotArea) {
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(plotArea.bottomLeft(), plotArea.bottomRight());  // X轴
    painter.drawLine(plotArea.bottomLeft(), plotArea.topLeft());      // Y轴

    painter.drawText(plotArea.right() + 10, plotArea.bottom() + 10, "X");
    painter.drawText(plotArea.left() - 20, plotArea.top() - 10, "Y");
}

// 绘制刻度
void PlotWidget1::drawTicks(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY) {
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
//QVector<QVector<double>> PlotWidget1::drawDataPoints(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY) {
//    painter.setPen(QPen(Qt::green, 1));
//    QVector<QPointF> normalizedPoints;
//    QVector<QPointF> normalizedPoints1;
//    QVector<QPointF> positivePeaks1;
//    QVector<QPointF> negativePeaks1;

//    // 平滑数据
//    QVector<QPointF> smoothedPoints = smoothData(points, 5);

//    // 将平滑后的点归一化到绘图区域
//    for (const QPointF &point : smoothedPoints) {
//        int x=point.x();
//        double x1 = mapValueToPlotArea(point.x(), minX, maxX, plotArea.left(), plotArea.right());
//        double y = mapValueToPlotArea(point.y(), minY, maxY, plotArea.bottom(), plotArea.top());
//        normalizedPoints.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
//        normalizedPoints1.append(QPointF(static_cast<int>(x1), static_cast<double>(y)));
//    }


//    // 绘制数据线
//    for (int i = 0; i < normalizedPoints1.size() - 1; ++i) {
//        painter.drawLine(normalizedPoints1[i], normalizedPoints1[i + 1]);
//    }

//    // 使用邻域比较法检测正峰值和负峰值

//    for (int i = 1; i < normalizedPoints.size() - 1; ++i) {
//        QPointF currentPoint = normalizedPoints[i];
//        QPointF prevPoint = normalizedPoints[i - 1];
//        QPointF nextPoint = normalizedPoints[i + 1];

//        if (currentPoint.y() > prevPoint.y() && currentPoint.y() > nextPoint.y()) {
//            // 当前点是局部最大值，检测为正峰值
//            negativePeaks.append(currentPoint);
//        }

//        if (currentPoint.y() < prevPoint.y() && currentPoint.y() < nextPoint.y()) {
//            // 当前点是局部最小值，检测为负峰值

//            positivePeaks.append(currentPoint);
//        }
//    }


//    for (int i = 0; i < negativePeaks.size(); ) {
//        bool removed = false;  // 记录是否进行了删除操作

//        for (int j = 0; j < positivePeaks.size(); ) {
//            if (positivePeaks[j].x() == negativePeaks[i].x()) {
////                negativePeaks.removeAt(i);  // 删除负峰值
//                positivePeaks.removeAt(j);  // 删除正峰值
//                removed = true;  // 标记删除
//                break;  // 跳出内层循环，重新检查下一个负峰值
//            } else {
//                ++j;  // 只有在没有删除时才增加j
//            }
//        }

//        if (!removed) {
//            ++i;  // 只有在没有删除时才增加i
//        }
//    }
//    for (int i = 0; i < negativePeaks.size(); ) {
//        bool removed = false;  // 记录是否进行了删除操作

//        for (int j = 0; j < positivePeaks.size(); ) {
//            if (abs(positivePeaks[j].y() - negativePeaks[i].y())<2) {
////                negativePeaks.removeAt(i);  // 删除负峰值
//                positivePeaks.removeAt(j);  // 删除正峰值
//                removed = true;  // 标记删除
//                break;  // 跳出内层循环，重新检查下一个负峰值
//            } else {
//                ++j;  // 只有在没有删除时才增加j
//            }
//        }

//        if (!removed) {
//            ++i;  // 只有在没有删除时才增加i
//        }
//    }

//    for (const QPointF &positivePeak : positivePeaks) {
//        double x = mapValueToPlotArea(positivePeak.x(), minX, maxX, plotArea.left(), plotArea.right());
//        double y = positivePeak.y();
//        positivePeaks1.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
//    }
//    for (const QPointF &negativePeak : negativePeaks) {
//        double x = mapValueToPlotArea(negativePeak.x(), minX, maxX, plotArea.left(), plotArea.right());
//        double y = negativePeak.y();
//        negativePeaks1.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
//    }
//    // 绘制正峰值
//    painter.setPen(QPen(Qt::red, 2));
//    for (const QPointF &peak : positivePeaks1) {
//        painter.drawEllipse(peak, 2,2);
//    }

//    // 绘制负峰值
//    painter.setPen(QPen(Qt::blue, 2));
//    for (const QPointF &peak : negativePeaks1) {
//        painter.drawEllipse(peak, 1, 1);
//    }

//    // 返回峰值组合
//    return combineAndSortPeaks(negativePeaks, positivePeaks);
//}
// 绘制包络线
void PlotWidget1::drawEnvelope(QPainter &painter, const QVector<QPointF> &positivePeaks, const QVector<QPointF> &negativePeaks) {
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
QVector<QVector<double>> PlotWidget1::combineAndSortPeaks(const QVector<QPointF>& negativePeaks, const QVector<QPointF>& positivePeaks) {
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
    std::cout<<"________________"<<result.size()<<std::endl;
    return result;
}

QVector<QVector<double>> PlotWidget1::combineAndSortPeaks1(const QVector<QPointF>& negativePeaks, const QVector<QPointF>& positivePeaks, QVector<QPointF>& plotdata,QVector<QVector<double>>& result) {


    QVector<QVector<double>> percentagePoints1; // 用于保存各百分比点的 X 值

    // 遍历每个区间，提取 Y 值的百分比对应的 X 值
    for (const QVector<double>& resul : result) {
        double start = resul[0];
        double end = resul[1];

        // 提取 plotdata 中 X 在 [start, end] 范围内的点
        QVector<QPointF> rangeData;
        for (const QPointF& point : plotdata) {
            if (point.x() >= start && point.x() <= end) {
                rangeData.append(point);
            }
        }

        if (rangeData.size() < 2) {
            std::cout << "Not enough points in range [" << start << ", " << end << "]" << std::endl;
            continue; // 跳过无效区间
        }

        // 对 rangeData 按 Y 值排序
        std::sort(rangeData.begin(), rangeData.end(), [](const QPointF& a, const QPointF& b) {
            return a.y() < b.y(); // 升序排列
        });

        int size = rangeData.size();
        QVector<double> percentagePoints = {
            rangeData[static_cast<int>(size * 0.4)].x(), // 20% Y 对应的 X 值
            rangeData[static_cast<int>(size * 0.5)].x(), // 50% Y 对应的 X 值
            rangeData[static_cast<int>(size * 0.8)].x(), // 80% Y 对应的 X 值
            rangeData[size - 1].x()                      // 100% Y 对应的 X 值
        };

        // 打印区间和百分比点
        std::cout << "Range [" << start << ", " << end << "]:" << std::endl;
        std::cout << "20% Y对应的X: " << percentagePoints[0]
                  << ", 50% Y对应的X: " << percentagePoints[1]
                  << ", 80% Y对应的X: " << percentagePoints[2]
                  << ", 100% Y对应的X: " << percentagePoints[3] << std::endl;

        percentagePoints1.push_back(percentagePoints);
    }

    // 打印所有的百分比点
    std::for_each(percentagePoints1.begin(), percentagePoints1.end(), [](QVector<double> vals) {
        std::for_each(vals.begin(), vals.end(), [](double val) {
            std::cout << val << " ";
        });
        std::cout << std::endl;
    });
    std::cout << "________________ percentagePoints: " << percentagePoints1.size() << std::endl;

    return percentagePoints1; // 返回每个区间的百分比点
}

// 平滑数据
QVector<QPointF> PlotWidget1::smoothData(const QVector<QPointF> &data, int windowSize) {
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
double PlotWidget1::mapValueToPlotArea(double value, double minValue, double maxValue, double plotMin, double plotMax) {
    if (maxValue == minValue) return plotMin;  // Avoid division by zero
    return plotMin + ((value - minValue) / (maxValue - minValue)) * (plotMax - plotMin);
}

// Data processing for images (example implementation)
QVector<QVector<QVector<double>>> PlotWidget1::getDatas(QVector<QVector<int>> data, QVector<QVector<double>> result3) {
    int len = result3.size();

    QVector<QVector<QVector<double>>> normalizedData(len, QVector<QVector<double>>(4, QVector<double>(data[0].size())));
    QVector<QVector<int>> ids;

    for (int k = 0; k < len; k++) {
        QVector<int> id = {int(result3[k][0]),
                           int(result3[k][1]),
                           int(result3[k][2]),
                           int(result3[k][3])};

        ids.append(id);
        for (int j = 0; j < 4; j++) {
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

// Function to process and normalize a single data vector
QVector<double> PlotWidget1::getData(const QVector<int>& data) {
    QVector<double> normalizedData(data.size());

    // Find the minimum and maximum values in the data
    int min, max;
    if (findMinMax(data, min, max) != 0) {
        qWarning() << "Failed to find min and max values.";
        return QVector<double>();
    }

    // Check if normalization is possible
    if (max == min) {
        qWarning() << "Max and Min are equal, normalization is not possible.";
        return QVector<double>();
    }

    // Normalize each value in the data vector
    for (int i = 0; i < data.size(); ++i) {
        normalizedData[i] = double(data[i] - min) / double(max - min);
    }

    return normalizedData;
}


// Find Min and Max
int PlotWidget1::findMinMax(const QVector<int>& data, int& min, int& max) {
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
QString PlotWidget1::convertMatToBase64(const cv::Mat &mat) {
    if (mat.empty()) {
        qDebug() << "Empty image encountered, skipping conversion.";
        return "";  // Return an empty string if the image is empty
    }

    std::vector<uchar> buf;
    cv::imencode(".png", mat, buf);
    return QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(buf.data()), buf.size()).toBase64());
}

// Update Image Label
void PlotWidget1::updateImageLabel(QTextEdit *label, const QString &base64Image) {
    QString html = "<img src='data:image/png;base64," + base64Image + "' />";
    label->setHtml(html);
}

//#include <QString>
//#include <QDebug>
#include <onnxruntime/core/providers/cpu/cpu_provider_factory.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>

QString PlotWidget1::onnxInfer(cv::Mat img) {
    // Define the classes
    const std::vector<std::string> classes = {"class1", "class2", "class3", "class4", "class5"};

    // Initialize ONNX Runtime
    static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeModel");
    static Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // Load the ONNX model only once if it's not loaded yet
    static std::unique_ptr<Ort::Session> session;
    if (!session) {
        const std::string modelPath = "home/ljy/Doc1/mycode/CNN_train_2/best_model_fold_2.onnx";////home/ljy/Doc1/mycode/CNN_train_2/best_model_fold_2.onnx
        ///home/ljy/Doc1/mycode/CNN_train/pressCNN_net.onnx
        session = std::make_unique<Ort::Session>(env, modelPath.c_str(), sessionOptions);
    }

    // Check if the input image is empty
    if (img.empty()) {
        qWarning() << "Failed to read input image!";
        return "Invalid Image";
    }

    // Preprocess the image
    cv::Mat resizedImg, floatImg;
    cv::resize(img, resizedImg, cv::Size(64, 64)); // Resize to 64x64

    // Convert BGR to RGB
    cv::cvtColor(resizedImg, resizedImg, cv::COLOR_BGR2RGB);

    // Convert to float32 and normalize to [-1, 1] using (img - 0.5) / 0.5
    resizedImg.convertTo(floatImg, CV_32F, 1.0 / 255.0);
    floatImg = (floatImg - 0.5) / 0.5;

    // Convert to channels-first format (1, 3, 64, 64) for ONNX Runtime
    std::vector<float> inputTensorValues;
    inputTensorValues.reserve(1 * 3 * 64 * 64); // Reserve memory in advance
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < 64; ++i) {
            for (int j = 0; j < 64; ++j) {
                inputTensorValues.push_back(floatImg.at<cv::Vec3f>(i, j)[c]);
            }
        }
    }

    // Prepare input tensor dimensions
    std::vector<int64_t> inputDims = {1, 3, 64, 64}; // Batch size: 1, Channels: 3, Height: 64, Width: 64
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputDims.data(), inputDims.size());

    // Run the inference
    Ort::AllocatorWithDefaultOptions allocator;
    const char* inputName = session->GetInputName(0, allocator);
    const char* outputName = session->GetOutputName(0, allocator);
    auto outputTensors = session->Run(Ort::RunOptions{nullptr}, &inputName, &inputTensor, 1, &outputName, 1);

    // Get the output tensor
    float* outputData = outputTensors.front().GetTensorMutableData<float>();

    // Print raw output for debugging
//    qDebug() << "Raw model output:";
//    for (int i = 0; i < 5; ++i) {
//        qDebug() << "Class" << i << ":" << outputData[i];
//    }

    // Apply softmax manually to interpret values as probabilities
    std::vector<float> softmaxOutput(5);
    float sum = 0.0;
    for (int i = 0; i < 5; ++i) {
        softmaxOutput[i] = std::exp(outputData[i]);
        sum += softmaxOutput[i];
    }
//    for (int i = 0; i < 5; ++i) {
//        softmaxOutput[i] /= sum;
//        qDebug() << "Softmax probability for Class" << i << ":" << softmaxOutput[i];
//    }

    // Find the class with the highest probability
    int predictedClass = std::distance(softmaxOutput.begin(), std::max_element(softmaxOutput.begin(), softmaxOutput.end()));

    // Return the predicted class as a QString
    QString predictedClassName = QString::fromStdString(classes[predictedClass]);
    qDebug() << "Predicted class:" << predictedClassName;
    return predictedClassName;
}


QString PlotWidget1::onnxInferMultiChannel(const std::vector<cv::Mat>& images) {
    // Define the classes
    const std::vector<std::string> classes = {"class1", "class2", "class3", "class4", "class5"};

    // Initialize ONNX Runtime
    static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeModel");
    static Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // Load the ONNX model only once
    static std::unique_ptr<Ort::Session> session;
    if (!session) {
        const std::string modelPath = "/home/ljy/Doc1/mycode/MULti_channels_CNN_train/model_fold_5.onnx";
        session = std::make_unique<Ort::Session>(env, modelPath.c_str(), sessionOptions);
    }

    // Check input validity
    if (images.size() != 4) {
        qWarning() << "Expected 4 input images for multi-channel processing!";
        return "Invalid Input";
    }

    for (const auto& img : images) {
        if (img.empty()) {
            qWarning() << "One or more input images are empty!";
            return "Invalid Input";
        }
    }

    // Preprocess images and concatenate into a single 12-channel tensor
    std::vector<float> inputTensorValues;
    inputTensorValues.reserve(1 * 12 * 64 * 64); // Batch: 1, Channels: 12, Height: 64, Width: 64
    for (const auto& img : images) {
        // Resize image to 64x64
        cv::Mat resizedImg, floatImg;
        cv::resize(img, resizedImg, cv::Size(64, 64));

        // Convert BGR to RGB
        cv::cvtColor(resizedImg, resizedImg, cv::COLOR_BGR2RGB);

        // Convert to float32 and normalize to [-1, 1]
        resizedImg.convertTo(floatImg, CV_32F, 1.0 / 255.0);
        floatImg = (floatImg - 0.5) / 0.5;

        // Add the channels-first data to inputTensorValues
        for (int c = 0; c < 3; ++c) {
            for (int i = 0; i < 64; ++i) {
                for (int j = 0; j < 64; ++j) {
                    inputTensorValues.push_back(floatImg.at<cv::Vec3f>(i, j)[c]);
                }
            }
        }
    }

    // Prepare input tensor dimensions
    std::vector<int64_t> inputDims = {1, 12, 64, 64}; // Batch: 1, Channels: 12, Height: 64, Width: 64
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputDims.data(), inputDims.size());

    // Run inference
    Ort::AllocatorWithDefaultOptions allocator;
    const char* inputName = session->GetInputName(0, allocator);
    const char* outputName = session->GetOutputName(0, allocator);
    auto outputTensors = session->Run(Ort::RunOptions{nullptr}, &inputName, &inputTensor, 1, &outputName, 1);

    // Get the output tensor
    float* outputData = outputTensors.front().GetTensorMutableData<float>();

    // Debug: Print raw output values
    std::vector<float> rawOutput(classes.size());
    for (size_t i = 0; i < classes.size(); ++i) {
        rawOutput[i] = outputData[i];
        std::cout << "Class " << i << " score: " << outputData[i] << std::endl;
    }

    // Apply softmax for probabilities
    std::vector<float> softmaxOutput(classes.size());
    float sum = 0.0f;
    for (float val : rawOutput) {
        sum += std::exp(val);
    }
    for (size_t i = 0; i < softmaxOutput.size(); ++i) {
        softmaxOutput[i] = std::exp(rawOutput[i]) / sum;
        std::cout << "Softmax probability for Class " << i << ": " << softmaxOutput[i] << std::endl;
    }

    // Find the class with the highest probability
    int predictedClassIndex = std::distance(softmaxOutput.begin(), std::max_element(softmaxOutput.begin(), softmaxOutput.end()));
    QString predictedClassName = QString::fromStdString(classes[predictedClassIndex]);

    std::cout << "Predicted class: " << predictedClassName.toStdString() << std::endl;
    return predictedClassName;
}



//void PlotWidget::tts_kedax(const QString &text1) {
//    // Step 1: Get the instance of TTSThread
//    TTSThread* ttsThread = TTSThread::getInstance();

//    // Step 2: Set up the audio output device for TTS (if not already set)
//#if defined(Q_OS_LINUX)
//    ttsThread->setDevice(QAudioDeviceInfo::defaultOutputDevice());

//#elif defined(Q_OS_WIN)
//    // On Windows, set specific devices if required
//    for (const QAudioDeviceInfo &deviceInfo : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
//        if (deviceInfo.deviceName().contains("USB PnP Sound Device") ||
//            deviceInfo.deviceName().contains("BT-Speaker") ||
//            deviceInfo.deviceName().contains("HUAWEI FreeLace")) {
//            ttsThread->setDevice(deviceInfo);
//            break;
//        }
//    }
//#endif

//    // Step 3: Set optional parameters for speed and volume
//    ttsThread->setSpeed(50);  // Speech speed (0-100)

//    ttsThread->setVolume(80); // Volume (0-100)这里进不去

//    // Step 4: Prepare the text in a format required by textToSpeech function
//    QByteArray textBytes = text1.toUtf8(); // Convert QString to QByteArray in UTF-8 encoding
//    const char* text = textBytes.constData(); // Get const char* from QByteArray

//    // Step 5: Synthesize text to speech
//    bool success = ttsThread->textToSpeech(text);
//    if (success) {
//        qDebug() << "Text-to-speech synthesis succeeded.";
//        // Automatically play the audio when synthesis is finished
//        connect(ttsThread, &TTSThread::ttsFinished, ttsThread, &TTSThread::play, Qt::UniqueConnection);
//    } else {
//        qDebug() << "Text-to-speech synthesis failed.";
//    }
//}
//typedef int SR_DWORD;
//typedef short int SR_WORD ;

///* wav音频头部格式 */
//typedef struct _wave_pcm_hdr
//{
//    char            riff[4];                // = "RIFF"
//    int				size_8;                 // = FileSize - 8
//    char            wave[4];                // = "WAVE"
//    char            fmt[4];                 // = "fmt "
//    int				fmt_size;				// = 下一个结构体的大小 : 16

//    short int       format_tag;             // = PCM : 1
//    short int       channels;               // = 通道数 : 1
//    int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
//    int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
//    short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
//    short int       bits_per_sample;        // = 量化比特数: 8 | 16

//    char            data[4];                // = "data";
//    int				data_size;              // = 纯数据长度 : FileSize - 44
//} wave_pcm_hdr;

///* 默认wav音频头部数据 */
//wave_pcm_hdr default_wav_hdr =
//{
//    { 'R', 'I', 'F', 'F' },
//    0,
//    {'W', 'A', 'V', 'E'},
//    {'f', 'm', 't', ' '},
//    16,
//    1,
//    1,
//    16000,
//    32000,
//    2,
//    16,
//    {'d', 'a', 't', 'a'},
//    0
//};
///* 文本合成 */
//#include <QMediaPlayer>
//#include <QUrl>
//#include <QCoreApplication>
//#include <QDir>

//#include <QMediaPlayer>
//#include <QEventLoop>
//#include <QUrl>
//#include <QDir>
//#include <QObject>
//#include <iostream>
//using namespace std;

//int PlotWidget::text_to_speech(const char* src_text, const char* des_path, const char* params)
//{
//    int          ret          = -1;
//    FILE*        fp           = NULL;
//    const char*  sessionID    = NULL;
//    unsigned int audio_len    = 0;
//    wave_pcm_hdr wav_hdr      = default_wav_hdr;
//    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

//    if (NULL == src_text || NULL == des_path)
//    {
//        printf("params is error!\n");
//        return ret;
//    }
//    fp = fopen(des_path, "wb");
//    if (NULL == fp)
//    {
//        printf("open %s error.\n", des_path);
//        return ret;
//    }

//    /* Start TTS synthesis */
//    sessionID = QTTSSessionBegin(params, &ret);
//    if (MSP_SUCCESS != ret)
//    {
//        printf("QTTSSessionBegin failed, error code: %d.\n", ret);
//        fclose(fp);
//        return ret;
//    }
//    ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
//    if (MSP_SUCCESS != ret)
//    {
//        printf("QTTSTextPut failed, error code: %d.\n",ret);
//        QTTSSessionEnd(sessionID, "TextPutError");
//        fclose(fp);
//        return ret;
//    }
//    printf("Synthesizing...\n");
//    fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); // Write WAV header

//    while (1)
//    {
//        /* Get synthesized audio data */
//        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
//        if (MSP_SUCCESS != ret)
//            break;
//        if (NULL != data)
//        {
//            fwrite(data, audio_len, 1, fp);
//            wav_hdr.data_size += audio_len; // Update data_size
//        }
//        if (MSP_TTS_FLAG_DATA_END == synth_status)
//            break;
//    }

//    if (MSP_SUCCESS != ret)
//    {
//        printf("QTTSAudioGet failed, error code: %d.\n",ret);
//        QTTSSessionEnd(sessionID, "AudioGetError");
//        fclose(fp);
//        return ret;
//    }

//    /* Update WAV header with final sizes */
//    wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

//    /* Write the updated header back to the file */
//    fseek(fp, 4, 0);
//    fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp);
//    fseek(fp, 40, 0);
//    fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp);
//    fclose(fp);
//    fp = NULL;

//    /* End the TTS session */
//    ret = QTTSSessionEnd(sessionID, "Normal");
//    if (MSP_SUCCESS != ret)
//    {
//        printf("QTTSSessionEnd failed, error code: %d.\n",ret);
//        return ret;
//    }

//    // Audio playback using QMediaPlayer
//    QMediaPlayer* player = new QMediaPlayer;
//    player->setMedia(QUrl::fromLocalFile(QDir::currentPath() + "/" + des_path));
//    player->setVolume(50); // Set volume to 50%

//    // 创建一个事件循环来等待音频播放完成
//    QEventLoop loop;
//    QObject::connect(player, &QMediaPlayer::stateChanged, &loop, [&](QMediaPlayer::State state) {
//        if (state == QMediaPlayer::StoppedState) {
//            loop.quit(); // 音频播放完成后退出事件循环
//        }
//    });

//    player->play(); // 开始播放音频
//    loop.exec();    // 阻塞，直到音频播放完成

//    delete player;  // 播放完成后清理 QMediaPlayer 对象
//    return ret;
//}


//void PlotWidget::process(const char* text)
//{
//    int         ret                  = MSP_SUCCESS;
//    const char* login_params         = "appid = 1bb452bd, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
//    /*
//    * rdn:           合成音频数字发音方式
//    * volume:        合成音频的音量
//    * pitch:         合成音频的音调
//    * speed:         合成音频对应的语速
//    * voice_name:    合成发音人
//    * sample_rate:   合成音频采样率
//    * text_encoding: 合成文本编码格式
//    *
//    */
//    const char* session_begin_params = "engine_type = local,voice_name=xiaoyan, text_encoding = UTF8, tts_res_path = fo|/home/liu/Doc/Doc1/mycode/4/appdesign_4/msc/res/tts/xiaoyan.jet;fo|/home/liu/Doc/Doc1/mycode/4/appdesign_4/msc/res/tts/common.jet, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
//    const char* filename             = "tts_sample.wav"; //合成的语音文件名称
////	const char* text                 = "科大讯飞作为智能语音技术提供商，在智能语音技术领域有着长期的研究积累，并在中文语音合成、语音识别、口语评测等多项技术上拥有技术成果。科大讯飞是我国以语音技术为产业化方向的国家863计划产业化基地"; //合成文本
//    /* 用户登录 */

//    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取

//    if (MSP_SUCCESS != ret)
//    {
//        printf("MSPLogin failed, error code: %d.\n", ret);
////        goto exit ;//登录失败，退出登录
//    }

////    printf("\n###########################################################################\n");
////    printf("## 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的 ##\n");
////    printf("## 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务的  ##\n");
////    printf("## 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求。  ##\n");
////    printf("###########################################################################\n\n");

////    /* 文本合成 */
////    printf("开始合成 ...\n");
//    ret = text_to_speech(text, filename, session_begin_params);

//    if (MSP_SUCCESS != ret)
//    {
//        printf("text_to_speech failed, error code: %d.\n", ret);
//    }
//    printf("合成完毕\n");



////	return 0;
//}

