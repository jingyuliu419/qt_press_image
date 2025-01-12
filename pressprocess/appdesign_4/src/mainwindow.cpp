#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
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
#include<pinterwave1.h>
#include<pinterwave.h>
#include "audioplayer.h"
#include "databaseworker.h"
#include<QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pressSensor(new PressSensor(this))  // 正确初始化 PressSensor
    , isConnected(false)  // 初始化状态为未连接
{
    ui->setupUi(this);
    setWindowTitle("压力可视化");

    // 获取按钮指针
    QPushButton *startListenBtn = ui->pushButton1;  // “事后分析”按钮
    QPushButton *startProgramBtn = ui->pushButton2; // 开始程序按钮

    // 配置 "开始程序" 按钮的功能
    connect(startProgramBtn, &QPushButton::clicked, this, [this]() {
        if (!isConnected) {
            index=0;
            // 连接信号槽
            connect(pressSensor, &PressSensor::updateUIpressData, this, &MainWindow::updatePressureUI);
            isConnected = true;
            qDebug() << "Program started: Connected signal-slot for pressure data.";
        } else {
            qDebug() << "Signal-slot already connected.";
        }
    });

    // 配置 "事后分析" 按钮的功能
    connect(startListenBtn, &QPushButton::clicked, this, [this]() {
        if (isConnected) {
            // 断开信号槽
            disconnect(pressSensor, &PressSensor::updateUIpressData, this, &MainWindow::updatePressureUI);
            isConnected = false;
            qDebug() << "Program stopped: Disconnected signal-slot for pressure data.";
        } else {
            qDebug() << "Signal-slot already disconnected.";
        }
        connect(this, &MainWindow::starting, this, &MainWindow::select_image_dir);
        qDebug() << "Connected to select_image_dir(). Emitting starting signal...";
        emit starting(1); // 触发信号来调用 select_image_dir()
    });
}
QString MainWindow::onStartButtonClicked()
{
    // 获取用户输入的表名
    QString tableName = ui->lineEdit1->text();

    // 检查是否为空
    if (tableName.isEmpty()) {
        qDebug() << "Table name is empty. Please enter a valid name.";
        QMessageBox::warning(this, "警告", "请输入有效的表名！");
        return QString();
    }

    // 输出表名，用于验证
    qDebug() << "Table name:" << tableName;

    // 在这里可以继续处理逻辑，比如将 tableName 传递给其他函数
    useTableName(tableName);
    return tableName;
}

void MainWindow::useTableName(const QString &tableName)
{
    // 示例：将表名输出到日志
    qDebug() << "Using table name:" << tableName;

    // 在这里可以连接数据库或处理与表名相关的逻辑
}



void MainWindow::updatePressureUI(const QVariant &data)
{
    QVector<int> pressureData = data.value<QVector<int>>();
    QString tbname=onStartButtonClicked();

    // 创建线程和工作对象
    QThread *mysqlThread = new QThread; // 将变量名改为 mysqlThread
    DatabaseWorker *worker = new DatabaseWorker;

    // 配置工作对象
    worker->setup("localhost", 3306, tbname, "root", "ljy895562", "testdb", index, pressureData);

    // 将工作对象移动到线程
    worker->moveToThread(mysqlThread);

    // 连接线程信号槽
    connect(mysqlThread, &QThread::started, worker, &DatabaseWorker::process);
    connect(worker, &DatabaseWorker::finished, mysqlThread, &QThread::quit);
    connect(worker, &DatabaseWorker::finished, worker, &DatabaseWorker::deleteLater);
    connect(mysqlThread, &QThread::finished, mysqlThread, &QThread::deleteLater);

    // 启动线程
    mysqlThread->start();

    // 以下部分是更新 UI 的逻辑
    QPointF plotData;
    QVector<int> plotData1;

    int sum1 = sum(pressureData);
    int time = index++;
    plotData1 = pressureData;

    plotData = QPoint(time, sum1);

    // 创建 PlotWidget 并设置绘图数据
    auto myPanel3 = new PlotWidget1(ui);
    myPanel3->setPlotData(plotData, plotData1);

    // 清除并更新 UI 的布局
    if (ui->plotWidget1->layout()) {
        clearLayout(ui->plotWidget1->layout());
        ui->plotWidget1->layout()->addWidget(myPanel3);
    } else {
        auto layout = new QVBoxLayout(ui->plotWidget1);
        layout->addWidget(myPanel3);
        ui->plotWidget1->setLayout(layout);
    }
}




MainWindow::~MainWindow()
{
//    audioThread->quit();
//    audioThread->wait();
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
    // 获取用户输入的表名
    QString tableName = ui->lineEdit1->text();

    // 检查表名是否为空
    if (tableName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入有效的表名！");
        return;
    }

    // 设置 MySQL 连接参数
    QString hostName = "localhost";
    int port = 3306;
    QString userName = "root"; // 替换为您的用户名
    QString password = "ljy895562"; // 替换为您的密码
    QString databaseName = "testdb";

    // 创建 MySQL 数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostName);
    db.setPort(port);
    db.setUserName(userName);
    db.setPassword(password);
    db.setDatabaseName(databaseName);

    // 尝试连接数据库
    if (!db.open()) {
        QMessageBox::critical(this, "错误", "无法连接到数据库");
        return;
    }

    // 查询数据
    QVector<QVector<int>> fullData; // 存储时间和 256 数据点的完整数据
    QSqlQuery query;
    query.prepare("SELECT times, data FROM " + tableName); // 查询 times 和 data 列

    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败");
        return;
    }

    // 处理查询结果
    QVector<QPointF> plotData; // 绘图数据
    while (query.next()) {
        int time = query.value("times").toInt();
        QString dataString = query.value("data").toString(); // 获取 data 列为字符串

        // 将 dataString 转换为整数列表
        QStringList dataList = dataString.split(',', Qt::SkipEmptyParts); // 假设数据以逗号分隔
        QVector<int> data;
        data.append(time); // 第一个元素为时间
        for (const QString &numStr : dataList) {
            data.append(numStr.toInt());
        }

        // 检查数据是否有 256 个点
        if (data.size() != 257) {
            QMessageBox::warning(this, "警告", "数据格式错误，列数量不匹配！");
            return;
        }

        fullData.append(data);

        // 用时间和值的总和生成绘图数据点
        int dataSum = std::accumulate(data.begin() + 1, data.end(), 0); // 从第二个元素开始求和
        plotData.append(QPointF(time, dataSum));
    }

    // 检查数据是否为空
    if (fullData.isEmpty()) {
        QMessageBox::warning(this, "警告", "表中没有数据或表不存在！");
        return;
    }

    // 更新 UI 的标签显示表名
    ui->txt_label->setText("Table: " + tableName);
    std::for_each(fullData.begin(),fullData.end(),[](QVector<int>data){
        std::for_each(data.begin(),data.end(),[](int val){
                std::cout<<val<<" ";
            });
            std::cout<<std::endl;
            std::cout<<"****************************************"<<std::endl;
    });
    std::cout<<"plotData:**********************************************************"<<std::endl;
    std::for_each(plotData.begin(),plotData.end(),[](QPointF val){
        std::cout<<val.x()<<" "<<val.y()<<std::endl;
    });
    std::cout<<"fullData:**********************************************************"<<std::endl;
    std::for_each(fullData.begin(),fullData.end(),[](QVector<int> vals){
        std::for_each(vals.begin(),vals.end(),[](int val){
            std::cout<<val<<" ";
        });
        std::cout<<std::endl;
    });
    // 创建新的绘图控件并设置数据
    auto myPanel3 = new PlotWidget(ui);
    myPanel3->setPlotData(plotData, fullData); // 传递绘图数据和完整数据向量

    // 更新 UI 的布局
    if (ui->plotWidget1->layout()) {
        clearLayout(ui->plotWidget1->layout());
        ui->plotWidget1->layout()->addWidget(myPanel3);
    } else {
        auto layout = new QVBoxLayout(ui->plotWidget1);
        layout->addWidget(myPanel3);
        ui->plotWidget1->setLayout(layout);
    }

    // 关闭数据库连接
    db.close();
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
QVector<double> MainWindow::loadData1(QVector<int> pressdata) {
//     Filtering parameters
    int order = 4;
    double cutoffFrequency = 3.0;  // 3Hz cutoff frequency for the low-pass filter
    double sampleRate = 20; // Assume sampling rate as 1 sample per 3 seconds
    QVector<double> columnData;
    for(int i=0;i<256;i++){
        columnData[i]=pressdata[i];
    }

    for (int i = 0; i < 256; ++i) {



        // Apply time-domain Butterworth low-pass filtering
        QVector<double> filteredData = butterworthLowPassFilter(columnData, order, cutoffFrequency, sampleRate);

        // Write the filtered data back to the original data structure

    }

    return columnData;
}
