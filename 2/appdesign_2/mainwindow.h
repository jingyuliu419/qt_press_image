#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"grid_layout_panel.h"
#include "picture_process.h"
#include<QString>
#include<QTextEdit>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void select_image_dir();
//    int findMinMax(const QVector<int>& data, int& min, int& max);
    int sum(const QVector<int>& data);
    cv::Mat QPixmapToCvMat(const QPixmap &image);
//    void updateImageLabel(QTextEdit *label, const QString &base64Image);
//    QString convertMatToBase64(const cv::Mat &mat);
    QVector<QVector<int>> loadData(const QString& fileName);
    void butterworthFilter(QVector<double>& data, int order, double cutoffFrequency, double samplePeriod);
    QVector<double> butterworthLowPassFilter(const QVector<double>& input, int order, double cutoffFrequency, double sampleRate);
    void clearLayout(QLayout *layout);
    QVector<double> fft(const QVector<double>& f);
    QVector<double> ifft(const QVector<double>& f);
    QVector<double> calculateSpectrumInDb(const QVector<double>& frequencyDomain);
    ~MainWindow();
//protected:
//    void paintEvent(QPaintEvent* event);
private:
    Ui::MainWindow *ui;
    QLabel *txt_label;
    QLabel *img_label;
    QVector<QLabel*> img_labels;
//    QVector<QPointF> points;
//    QPointF mapToPlotArea(const QPointF &point, const QRectF &plotArea,
//                              qreal minX, qreal maxX, qreal minY, qreal maxY);
};
#endif // MAINWINDOW_H
