// PlotWidget.h
#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include<set>
#include "mainwindow.h"

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget( Ui::MainWindow *ui, QWidget *parent = nullptr);
    PlotWidget* setPlotData(const QVector<QPointF> &plotdata,const QVector<QVector<int>> &data);

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
    QVector<QVector<double>> combinedPeaks;
    QVector<QVector<QVector<double>>> getDatas(QVector<QVector<int>> data,QVector<QVector<double>>result3);
    QVector<double> getData(const QVector<int>& data);
    int findMinMax(const QVector<int>& data, int& min, int& max);
    QString convertMatToBase64(const cv::Mat &mat);
    void updateImageLabel(QTextEdit *label, const QString &base64Image);
    QString onnxInfer(cv::Mat);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> points;
    QVector<QVector<int>> datas;
    void drawAxes(QPainter &painter, const QRect &plotArea);
    Ui::MainWindow *ui;
    QVector<QPointF> positivePeaks;
    QVector<QPointF> negativePeaks;

};

#endif // PLOTWIDGET_H
