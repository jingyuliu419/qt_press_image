#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QHBoxLayout>
#include "picture_process.h"
#include "grid_layout_panel.h"
#include <QDebug>
#include <QBuffer>
#include <QVector>
#include<QPainter>
#include<QPen>
#include<QBrush>
#include<pinterwave.h>
#include <fftw3.h>
#include <rtfilter.h>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("压力可视化");

//    auto myPanel1 = new GrideLayoutPanel();

    QPushButton *selectBtn = ui->pushButton1;


    connect(selectBtn, &QPushButton::clicked, this, [this]() {
        this->select_image_dir();
    });

}


MainWindow::~MainWindow()
{
    delete ui;
}

cv::Mat MainWindow::QPixmapToCvMat(const QPixmap &pixmap)
{
    QImage image = pixmap.toImage();  // Convert QPixmap to QImage
    cv::Mat mat;

    switch (image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4,
                      const_cast<uchar*>(image.bits()), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3,
                      const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);  // Convert RGB to BGR
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1,
                      const_cast<uchar*>(image.bits()), image.bytesPerLine());
        break;
    default:
        qWarning("QImage format not handled in conversion");
        break;
    }

    return mat.clone(); // Ensure proper memory management
}

void MainWindow::select_image_dir() {
    //打开.txt文件
    auto file_path = QFileDialog::getOpenFileName(this, "Select a text file", "/home/liu/Doc/Doc1/mycode/original_data", "Text Files (*.txt)");
    if (file_path.isEmpty()) {
        return;
    }

    ui->txt_label->setText("File: " + file_path);

    // Load data into cv::Mat
    QVector<QVector<int>> data = loadData(file_path);
    if (data.empty()) {
        qWarning("Data could not be loaded or is empty!");
    } else {
        // Process your data or pass it to other functions
    }

    QVector<QPointF> plotData;
    QVector<QVector<int>> plotData1;
    for (int i = 0; i <data.size(); ++i) {
//        std::for_each(data[i].begin(),data[i].end(),[](int val){std::cout<<val<<" ";});
//        std::cout<<std::endl;
        int sum1=sum(data[i]);
        int time = data[i][0];
        plotData1.append({time,sum1});
//        std::for_each(plotData1[i].begin(),plotData1[i].end(),[](int val){std::cout<<val<<" ";});
//        std::cout<<std::endl;
        plotData.append(QPoint(time, sum1));
    }


    // Create a new PlotWidget instance and set the plot data
    auto myPanel3 = new PlotWidget(ui);  // Create the widget
    myPanel3->setPlotData(plotData,data);
     // Now chaining works

    if (ui->plotWidget1->layout()) {
        clearLayout(ui->plotWidget1->layout());
        ui->plotWidget1->layout()->addWidget(myPanel3);
    } else {
        // If there is no layout, set a new one
        auto layout = new QVBoxLayout(ui->plotWidget1);
        layout->addWidget(myPanel3);
        ui->plotWidget1->setLayout(layout);
    }
}

//int MainWindow::findMinMax(const QVector<int>& data, int& min, int& max) {
//    if (data.isEmpty()) {
//        return -1;
//    }
//    min = std::numeric_limits<int>::max();
//    max = std::numeric_limits<int>::min();
//    QVector<int> data1(data.begin() + 1, data.end());
//    for (const auto &value : data1) {
//        if (value < min) {
//            min = value;
//        }
//        if (value > max) {
//            max = value;
//        }
//    }
//    return 0;
//}
int MainWindow::sum(const QVector<int>& data) {
    // 如果你希望从data的第一个元素开始，你可以这样初始化data1
    QVector<double> data1(data.begin()+1, data.end());
    int sum1=0;
    for (const auto &value : data1) {
        sum1+=value;
    }
    return sum1;
}

void MainWindow::clearLayout(QLayout *layout) {
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}


//QString MainWindow::convertMatToBase64(const cv::Mat &mat) {
//    std::vector<uchar> buf;
//    cv::imencode(".png", mat, buf);
//    return QString::fromLatin1(QByteArray::fromRawData(reinterpret_cast<const char*>(buf.data()), buf.size()).toBase64());
//}

//void MainWindow::updateImageLabel(QTextEdit *label, const QString &base64Image) {
//    QString html = "<img src='data:image/png;base64," + base64Image + "' />";
//    label->setHtml(html);
//}

// Helper function to apply time-domain Butterworth low-pass filter
QVector<double> MainWindow::butterworthLowPassFilter(const QVector<double>& input, int order, double cutoffFrequency, double sampleRate) {
    int N = input.size();
    QVector<double> output(N);

    // Pre-calculate filter coefficients for a 4th-order Butterworth filter
    double PI = 3.14159265358979323846;
    double wc = 2.0 * PI * cutoffFrequency / sampleRate;  // Normalize cutoff frequency
    double K = wc / tan(wc / 2.0);

    double a0, a1, a2, b1, b2;

    // These are the filter coefficients derived for a 4th-order Butterworth filter
    a0 = 1.0 / (1.0 + sqrt(2.0) * K + K * K);
    a1 = 2 * a0;
    a2 = a0;
    b1 = 2.0 * (1.0 - K * K) * a0;
    b2 = (1.0 - sqrt(2.0) * K + K * K) * a0;

    QVector<double> x1(N, 0), x2(N, 0), y1(N, 0), y2(N, 0);

    // Perform the filtering using the difference equation
    for (int i = 0; i < N; i++) {
        output[i] = a0 * input[i] + a1 * x1[i] + a2 * x2[i] - b1 * y1[i] - b2 * y2[i];

        // Update history terms
        if (i < N - 1) {
            x2[i + 1] = x1[i];
            x1[i + 1] = input[i];
            y2[i + 1] = y1[i];
            y1[i + 1] = output[i];
        }
    }

    return output;
}

QVector<QVector<int>> MainWindow::loadData(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading.";
        return QVector<QVector<int>>();
    }
    QTextStream in(&file);
    QVector<QVector<int>> data;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (values.size() != 257) {
            qWarning() << "Invalid data format. Each line must contain 257 numbers.";
            continue;
        }

        QVector<int> row;
        for (const QString &value : values) {
            row.append(value.toInt());
        }
        data.append(row);
    }
    file.close();

//     Filtering parameters
    int order = 4;
    double cutoffFrequency = 3.0;  // 3Hz cutoff frequency for the low-pass filter
    double sampleRate = 20; // Assume sampling rate as 1 sample per 3 seconds

    for (int i = 1; i < 257; ++i) {
        QVector<double> columnData;
        for (const auto& row : data) {
            columnData.append(static_cast<double>(row[i]));
        }

        // Apply time-domain Butterworth low-pass filtering
        QVector<double> filteredData = butterworthLowPassFilter(columnData, order, cutoffFrequency, sampleRate);

        // Write the filtered data back to the original data structure
        for (int j = 0; j < data.size(); ++j) {
            data[j][i] = static_cast<int>(filteredData[j]);
        }
    }

    return data;
}
