#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QLabel>
#include <QVector>
#include <QVariant>
#include <opencv2/opencv.hpp>
#include "grid_layout_panel.h"
#include "picture_process.h"
#include "presssensor.h"
#include "audioplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 功能函数
    void select_image_dir();
    QString onStartButtonClicked();
    void useTableName(const QString &tableName);
    void updatePressureUI(const QVariant &data);
    int sum(const QVector<int>& data);
    cv::Mat QPixmapToCvMat(const QPixmap &image);
    QVector<QVector<int>> loadData(const QString &fileName);
    QVector<double> loadData1(QVector<int> pressdata);
    void butterworthFilter(QVector<double>& data, int order, double cutoffFrequency, double samplePeriod);
    QVector<double> butterworthLowPassFilter(const QVector<double>& input, int order, double cutoffFrequency, double sampleRate);
    QVector<double> fft(const QVector<double>& f);
    QVector<double> ifft(const QVector<double>& f);
    QVector<double> calculateSpectrumInDb(const QVector<double>& frequencyDomain);
    void clearLayout(QLayout *layout);

signals:
    void starting(int num);

private:
    Ui::MainWindow *ui;
    QLabel *txt_label;
    QLabel *img_label;
    QVector<QLabel*> img_labels;
    PressSensor *pressSensor;
    int index;         // 当前绘图索引
    bool isConnected;  // 信号槽连接状态
};

#endif // MAINWINDOW_H
