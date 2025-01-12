#ifndef PRINTERWAVE1_H
#define PRINTERWAVE1_H
#include <QWidget>
#include <QVector>
#include <QPoint>
#include<set>
#include<QThread>

#include "mainwindow.h"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QDebug>
#include <QTimer>
#include <QVector>
#include "audioplayer.h"
#include "worker.h" // Worker class for ONNX inference and image update
#include <opencv2/opencv.hpp>
#include "mysql_test.h"

class PlotWidget1 : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget1( Ui::MainWindow *ui, QWidget *parent = nullptr);
    ~PlotWidget1();
    PlotWidget1* setPlotData(const QPointF &plotdata,const QVector<int> &data);

    double mapValueToPlotArea(double value, double minValue, double maxValue, double plotMin, double plotMax);
    QVector<QVector<double>> drawDataPoints(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY);
    void drawGrid(QPainter &painter, const QRect &plotArea);
    void drawTicks(QPainter &painter, const QRect &plotArea, double minX, double maxX, double minY, double maxY);
    double findMinMax1(const QVector<double>& data, double& min, double& max);
    QVector<QPointF> findPeaks(const QVector<QPointF> &data, bool positive,int minProminence,int minPeakDistance);
    void houghPeaks(cv::Mat &H,int numpeaks,double thresh,const int *nhood_size,QVector<double> &r,QVector<double> &c);
    QVector<QPointF> smoothData(const QVector<QPointF> &data, int windowSize);
    void findLocalMaximaAndMinima(const cv::Mat& src, std::vector<cv::Point>& maxima, std::vector<cv::Point>& minima, int neighborhoodSize = 3);
    void myMeshgrid(int q1, int q2, int p11, int p22, cv::Mat &qq, cv::Mat &pp);
    void mat2Cvector(const cv::Mat &mat, std::vector<int> &vec) ;
    void printPeaks(const QVector<QPointF>& peaks, const QString& label);
    QVector<QPointF> findMaxPeaks(const QVector<QPointF> &data, bool positive, int minProminence, int minPeakDistance);
    void drawEnvelope(QPainter &painter, const QVector<QPointF> &positivePeaks, const QVector<QPointF> &negativePeaks);
    QVector<QVector<double>> combineAndSortPeaks(const QVector<QPointF>& negativePeaks, const QVector<QPointF>& positivePeaks);
    QVector<QVector<double>> combineAndSortPeaks1(const QVector<QPointF>& negativePeaks, const QVector<QPointF>& positivePeaks,QVector<QPointF> &plotdata,QVector<QVector<double>>& result);
    QVector<QVector<double>> combinedPeaks;
    QVector<QVector<QVector<double>>> getDatas(QVector<QVector<int>> data,QVector<QVector<double>>result3);
    QVector<double> getData(const QVector<int>& data);
    int findMinMax(const QVector<int>& data, int& min, int& max);
    QString convertMatToBase64(const cv::Mat &mat);
    void updateImageLabel(QTextEdit *label, const QString &base64Image);
    QString onnxInfer(cv::Mat);
    QString onnxInferMultiChannel(const std::vector<cv::Mat>& images);
    void tts_kedax(const QString &text1);

    int text_to_speech(const char* src_text, const char* des_path, const char* params);
    void process(const char* text);

    void playAudioAndWait(const QString& audioPath);
    void delay(int milliseconds);


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF point;
    QVector<int> datas;

    void drawAxes(QPainter &painter, const QRect &plotArea);
    Ui::MainWindow *ui;
    QVector<QPointF> positivePeaks;
    QVector<QPointF> negativePeaks;
    // 音频线程和播放器
    QThread *audioThread;
    AudioPlayer *audioPlayer;
    // Threads
    QThread *onnxThread;  // Thread for ONNX inference
    QThread *imageThread; // Thread for image updates

    // Workers
    Worker *worker;           // Worker for ONNX inference and image updates

signals:
    // Signal to play audio in audio thread
    void playAudioSignal(const QString &audioPath);

    // Signal to run ONNX inference in onnxThread
    void runOnnxSignal(const cv::Mat &inputImage);

    // Signal to update image in imageThread
    void updateImageSignal(const QString &base64Image);

public slots:
    // Slot to handle ONNX inference result
    void handleOnnxResult(const QString &resultText);

    // Slot to handle image update completion
    void handleImageUpdate(const QString &base64Image);


};
#endif // PRINTERWAVE1_H
