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

    typedef int SR_DWORD;
    typedef short int SR_WORD ;

    /* wav音频头部格式 */
    typedef struct _wave_pcm_hdr
    {
        char            riff[4];                // = "RIFF"
        int				size_8;                 // = FileSize - 8
        char            wave[4];                // = "WAVE"
        char            fmt[4];                 // = "fmt "
        int				fmt_size;				// = 下一个结构体的大小 : 16

        short int       format_tag;             // = PCM : 1
        short int       channels;               // = 通道数 : 1
        int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
        int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
        short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
        short int       bits_per_sample;        // = 量化比特数: 8 | 16

        char            data[4];                // = "data";
        int				data_size;              // = 纯数据长度 : FileSize - 44
    } wave_pcm_hdr;

    /* 默认wav音频头部数据 */
    wave_pcm_hdr default_wav_hdr =
    {
        { 'R', 'I', 'F', 'F' },
        0,
        {'W', 'A', 'V', 'E'},
        {'f', 'm', 't', ' '},
        16,
        1,
        1,
        16000,
        32000,
        2,
        16,
        {'d', 'a', 't', 'a'},
        0
    };

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
    void tts_kedax(const QString &text1);

    int text_to_speech(const char* src_text, const char* des_path, const char* params);
    void process(const char* text);

    void playAudioAndWait(const QString& audioPath);

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
