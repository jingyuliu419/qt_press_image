#include "presssensor.h"
#include "packet_transmission.h"
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <iomanip>

PressSensor::PressSensor(QObject * /*parent*/) : QObject(nullptr) // 不设置父对象
{
    this->moveToThread(&PressSensor_Thread);
    PressSensor_Thread.start();
    connect(&PressSensor_Thread, &QThread::started, this, &PressSensor::new_slot);
}

void PressSensor::startListening()
{
    // 检查 myServer 是否已经被实例化
    if (!myServer) {
        myServer = new QTcpServer(this); // 实例化 myServer，指定父对象为当前类
    }

    // 启动 TCP 通信逻辑
    if (!myServer->listen(QHostAddress::Any, 8000)) {
        qDebug() << "Failed to start TCP server.";
    } else {
        qDebug() << "TCP server started on port 8000.";
    }
}



void PressSensor::isFinished(bool ok)
{
    if (ok) {
        qDebug() << "PressSensor is finished. Thread ID:" << QThread::currentThreadId();

        file.close();
        myServer->close();
        my_socket->close();

        myServer->deleteLater();
        my_socket->deleteLater();

        PressSensor_Thread.quit();
        PressSensor_Thread.wait();
    }
}
void PressSensor::handleAdoptCmd(char state)
{
    if (!my_socket || my_socket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Cannot send command. Socket is not connected.";
        return;
    }

    qint64 bytesWritten = my_socket->write(&state, 1);
    if (bytesWritten == 1) {
        qDebug() << "Command sent successfully. State:" << state;
    } else {
        qWarning() << "Failed to send command. State:" << state;
    }

    qDebug() << "handleAdoptCmd is called. Thread ID:" << QThread::currentThreadId();
}


void PressSensor::new_slot()
{
    static bool isInitialized = false;

    if (isInitialized) {
        qDebug() << "new_slot already initialized. Skipping.";
        return;
    }
    isInitialized = true;

    qDebug() << "new_slot called";

    qRegisterMetaType<QVariant>("QVariant");
    my_socket = new QTcpSocket;
    myServer = new QTcpServer;
    timestamp1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    initTcpServer();

    QString fileName("/home/ljy/Doc1/mycode/presssensor_test/build-pressDataTest-Desktop_Qt_5_14_2_GCC_64bit-Debug/pressData/pressSensorData.txt");
    if (file.isOpen()) {
        qDebug() << "Closing previously opened file.";
        file.close();
    }
    QFile::remove(fileName);
    file.setFileName(fileName);
    if (file.open(QIODevice::ReadWrite)) {
        qDebug() << "Data file opened successfully:" << fileName;
    } else {
        qWarning() << "Failed to open data file:" << fileName;
    }
}

void PressSensor::initTcpServer()
{
    quint16 port = 8000;
    QString ip = "192.168.2.100";
    QHostAddress address(ip);

    while (!myServer->listen(address, port)) {
        qWarning() << "PressSensor server failed to start on port:" << port
                   << "Error:" << myServer->errorString();
        port++; // 尝试下一个端口
        if (port > 8100) {
            qCritical() << "All ports in range are in use. Server cannot start.";
            return;
        }
    }

    qDebug() << "PressSensor server is now listening on" << ip << ":" << port;
    emit islisten(1);

    connect(myServer, &QTcpServer::newConnection, this, &PressSensor::newConnection_slot);
    qDebug() << "initTcpServer pressSensor thread ID:" << QThread::currentThreadId();
}

void PressSensor::newConnection_slot()
{
    my_socket = myServer->nextPendingConnection();

    if (my_socket) {
        QString clientIP = my_socket->peerAddress().toString();
        quint16 clientPort = my_socket->peerPort();
        qDebug() << "New connection established. Client IP:" << clientIP << ", Port:" << clientPort;

        connect(my_socket, &QTcpSocket::readyRead, this, &PressSensor::readRead_slot);
    } else {
        qWarning() << "Failed to retrieve pending connection.";
    }

    qDebug() << "PressSensor newConnection_slot thread ID:" << QThread::currentThreadId();
}

void PressSensor::readRead_slot()
{
    QByteArray recv = my_socket->readAll();
    uint16_t unpacked_payload[256];
    size_t unpacked_payload_len;
    uint8_t buffer[sizeof(Packet)];

    if (!recv.isEmpty()) {
        std::memcpy(buffer, recv.data(), recv.size());
//        std::cout << "******************************************" << std::endl;
//        for (int i = 0; i < sizeof(buffer); i++) {
//            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)buffer[i] << " ";
//        }
//        std::cout << std::endl;

        if (unpack_packet(buffer, sizeof(Packet), (uint8_t *)unpacked_payload, &unpacked_payload_len) == 0) {
//            std::cout << "Packet unpacked successfully." << std::endl;
//            std::cout << "Payload size: " << std::dec << unpacked_payload_len << std::endl;

            // 转换 unpacked_payload 为 QVector<int>
            QVector<int> buffer1;
            for (size_t i = 0; i < 256; ++i) {
                buffer1.append(static_cast<int>(unpacked_payload[i]));
            }

//            // 调试打印
//            std::cout << "Processed Payload: "<<unpacked_payload_len;

//            for (int value : buffer1) {
//                std::cout << value << " ";
//            }
//            std::cout << std::endl;

            // 将 buffer1 转换为 QVariant 并发送信号
            QVariant dataVar;
            dataVar.setValue(buffer1);
            emit updateUIpressData(dataVar);

        } else {
            std::cout << "Packet unpack failed." << std::endl;
        }
    }
}


void PressSensor::saveData(QByteArray &data)
{
    qDebug() << "saveData called with data size:" << data.size();

    // 检查文件是否打开
    if (!file.isOpen()) {
        qWarning() << "File is not open. Cannot save data.";
        return;
    }

    // 将数据解析为整数
    QList<QByteArray> dataList = data.split(' ');
    QVector<int> parsedData;
    for (const QByteArray &value : dataList) {
        parsedData.append(value.toInt());
    }

    // 写入文件
    QTextStream out(&file);
    out.setCodec("UTF-8");
    for (int i = 0; i < parsedData.size(); ++i) {
        out << parsedData[i] << " ";
    }
    out << "\n";

    // 发送信号更新 UI
    QVariant dataVar;
    dataVar.setValue(parsedData);
    emit updateUIpressData(dataVar);

    qDebug() << "Data saved to file and UI updated.";
}


void PressSensor::closeFile()
{
    if (file.isOpen()) {
        file.close();
        qDebug() << "File successfully closed.";
    } else {
        qDebug() << "File was not open. Nothing to close.";
    }
}


PressSensor::~PressSensor()
{
    qDebug() << "~PressSensor() destructor called.";

    // 停止并清理服务器
    if (myServer) {
        if (myServer->isListening()) {
            myServer->close();
            qDebug() << "Server stopped listening.";
        }
        myServer->deleteLater();
        myServer = nullptr;
    }

    // 停止并清理套接字
    if (my_socket) {
        if (my_socket->state() == QAbstractSocket::ConnectedState) {
            my_socket->close();
            qDebug() << "Socket connection closed.";
        }
        my_socket->deleteLater();
        my_socket = nullptr;
    }

    // 停止线程
    if (PressSensor_Thread.isRunning()) {
        stopRequested = true;  // 请求线程退出
        PressSensor_Thread.quit();
        if (!PressSensor_Thread.wait(3000)) {
            qWarning() << "Thread did not quit in time. Terminating...";
            PressSensor_Thread.terminate();  // 强制终止线程
            PressSensor_Thread.wait();       // 等待线程完成
        }
    }

    qDebug() << "PressSensor resources released successfully.";
}

