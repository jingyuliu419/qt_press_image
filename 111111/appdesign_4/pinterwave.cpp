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
#include <errno.h>
#include "include/qtts.h"
#include "include/msp_cmn.h"
#include "include/msp_errors.h"
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

// Ensure datas and points are properly initialized somewhere in your code

void PlotWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Ensure datas and points are initialized
    if (points.isEmpty() || datas.isEmpty()) {
        qDebug() << "Points or datas are not initialized!";
        return;
    }

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

    // Drawing
    drawGrid(painter, plotArea);
    drawAxes(painter, plotArea);
    drawTicks(painter, plotArea, minX, maxX, minY, maxY);
    combinedPeaks = drawDataPoints(painter, plotArea, minX, maxX, minY, maxY);

    // Image Processing
    std::vector<cv::Mat> combinedImage1(datas.size());
    std::vector<cv::Mat> dst1(datas.size());
    std::vector<QString> results(datas.size());

    auto myPanel2 = std::make_unique<PictureProcess>();
    auto myPanel1 = std::make_unique<GrideLayoutPanel>();

    // Peak detection logic might need further optimization
    std::set<double> pointSet;
    for (const auto& peak : positivePeaks) {
        pointSet.insert(peak.x());
    }

    QTextCursor cursor = ui->classtextEdit->textCursor();

    QTextCharFormat blackFormat;
//    blackFormat.setAlignment(Qt::AlignCenter);
    blackFormat.setFontPointSize(12);  // 设置字体大小
    blackFormat.setForeground(QBrush(QColor(0, 0, 0)));

    QTextCharFormat redFormat;
//    redFormat.setAlignment(Qt::AlignCenter);
    redFormat.setFontPointSize(40);  // 设置字体大小
    redFormat.setForeground(QBrush(QColor(255, 0, 0)));  // 设置文本颜色为红色

    for (int i = 0; i < datas.size(); i++) {
        QVector<double> normalizedData = getData(datas[i]);
        combinedImage1[i] = myPanel1->combineImages(normalizedData);
        if (combinedImage1[i].empty()) {
            qDebug() << "Unable to create combined image, index" << i;
            continue;
        }

        dst1[i] = myPanel2->processImage(combinedImage1[i], 2);
        QString base64Image = convertMatToBase64(combinedImage1[i]);
        if (!base64Image.isEmpty()) {
            updateImageLabel(ui->img_label_2, base64Image);
        }

        QString processedBase64Image = convertMatToBase64(dst1[i]);
        if (!processedBase64Image.isEmpty()) {
            updateImageLabel(ui->img_label_3, processedBase64Image);

            // Check if the point is in the set
            if (pointSet.find(datas[i][0]) != pointSet.end()) {
                QString resultText = onnxInfer(dst1[i]);

                // Save the image to the specified directory
                std::string filename = "/home/ljy/Doc1/mycode/4/appdesign_4/images/image_" + std::to_string(i) + ".png";
                cv::imwrite(filename, dst1[i]);

                // Use if-else statements instead of switch for QString
                if (resultText == "class1") {
                    cursor.insertText("正确\n", blackFormat);
                }
                else if (resultText == "class2") {
                    ui->classtextEdit->clear();
                    cursor.insertText("肘臂姿态错误\n", redFormat);
                    playAudioAndWait("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
                }
                else if (resultText == "class3") {
                    ui->classtextEdit->clear();
                    cursor.insertText("施力位置偏移\n", redFormat);
                    playAudioAndWait("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
                }
                else if (resultText == "class4") {
                    ui->classtextEdit->clear();
                    cursor.insertText("施力分散\n", redFormat);
                    playAudioAndWait("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
                }
                else if (resultText == "class5") {
                    ui->classtextEdit->clear();
                    cursor.insertText("手掌姿态错误\n", redFormat);
                    playAudioAndWait("/home/ljy/Doc1/mycode/CNN_train/ding_sound.wav");
                }
                else {
                    cursor.insertText("未知结果\n", blackFormat);
                }
            }
        }

        delay(1);  // Consider optimizing or removing this delay
    }

    // Scroll to the bottom after insertion to ensure latest text is visible
//    ui->classtextEdit->verticalScrollBar()->setValue(ui->classtextEdit->verticalScrollBar()->maximum());
}


void PlotWidget::playAudioAndWait(const QString& audioPath)
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
    QVector<QPointF> normalizedPoints1;
    QVector<QPointF> positivePeaks1;
    QVector<QPointF> negativePeaks1;

    // 平滑数据
    QVector<QPointF> smoothedPoints = smoothData(points, 5);

    // 将平滑后的点归一化到绘图区域
    for (const QPointF &point : smoothedPoints) {
        int x=point.x();
        double x1 = mapValueToPlotArea(point.x(), minX, maxX, plotArea.left(), plotArea.right());
        double y = mapValueToPlotArea(point.y(), minY, maxY, plotArea.bottom(), plotArea.top());
        normalizedPoints.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
        normalizedPoints1.append(QPointF(static_cast<int>(x1), static_cast<double>(y)));
    }


    // 绘制数据线
    for (int i = 0; i < normalizedPoints1.size() - 1; ++i) {
        painter.drawLine(normalizedPoints1[i], normalizedPoints1[i + 1]);
    }

    // 使用邻域比较法检测正峰值和负峰值

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

    for (const QPointF &positivePeak : positivePeaks) {
        double x = mapValueToPlotArea(positivePeak.x(), minX, maxX, plotArea.left(), plotArea.right());
        double y = positivePeak.y();
        positivePeaks1.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
    }
    for (const QPointF &negativePeak : negativePeaks) {
        double x = mapValueToPlotArea(negativePeak.x(), minX, maxX, plotArea.left(), plotArea.right());
        double y = negativePeak.y();
        negativePeaks1.append(QPointF(static_cast<int>(x), static_cast<double>(y)));
    }
    // 绘制正峰值
    painter.setPen(QPen(Qt::red, 2));
    for (const QPointF &peak : positivePeaks1) {
        painter.drawEllipse(peak, 2,2);
    }

    // 绘制负峰值
    painter.setPen(QPen(Qt::blue, 2));
    for (const QPointF &peak : negativePeaks1) {
        painter.drawEllipse(peak, 1, 1);
    }

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

// Function to process and normalize a single data vector
QVector<double> PlotWidget::getData(const QVector<int>& data) {
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

//#include <QString>
//#include <QDebug>
#include <onnxruntime/core/providers/cpu/cpu_provider_factory.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>

QString PlotWidget::onnxInfer(cv::Mat img) {
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
        const std::string modelPath = "/home/ljy/Doc1/mycode/CNN_train/pressCNN_net.onnx";
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
    qDebug() << "Raw model output:";
    for (int i = 0; i < 5; ++i) {
        qDebug() << "Class" << i << ":" << outputData[i];
    }

    // Apply softmax manually to interpret values as probabilities
    std::vector<float> softmaxOutput(5);
    float sum = 0.0;
    for (int i = 0; i < 5; ++i) {
        softmaxOutput[i] = std::exp(outputData[i]);
        sum += softmaxOutput[i];
    }
    for (int i = 0; i < 5; ++i) {
        softmaxOutput[i] /= sum;
        qDebug() << "Softmax probability for Class" << i << ":" << softmaxOutput[i];
    }

    // Find the class with the highest probability
    int predictedClass = std::distance(softmaxOutput.begin(), std::max_element(softmaxOutput.begin(), softmaxOutput.end()));

    // Return the predicted class as a QString
    QString predictedClassName = QString::fromStdString(classes[predictedClass]);
    qDebug() << "Predicted class:" << predictedClassName;
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

