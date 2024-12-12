#ifndef PRESSSENSOR_H
#define PRESSSENSOR_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTime>
#include <QVector>
#include <QFile>
#include <QMetaType>
#include <QVariant>
#include <QMetaType>
#include<iostream>
#include <cstring>
Q_DECLARE_METATYPE(QVector<int>);

class PressSensor : public QObject
{
    Q_OBJECT
public:
    explicit PressSensor(QObject *parent = nullptr);
    ~PressSensor();
    void initTcpServer();
    QTcpServer* myServer; //放到public中，使得再其他cpp中可以访问
    QThread PressSensor_Thread;
private:
     QFile file;

    void saveData(QByteArray &data);
   QTcpSocket* my_socket;
   QByteArray recvData;
   QByteArray pressSensorBuf;
   QTime Qt;
   qint64 timestamp1;
   qint64 timestamp2;

signals:
   void islisten(int isOK);
   void updateUIpressData(QVariant);

public slots:
   void newConnection_slot();         //由newConnection信号触发，创建新的socket
   void readRead_slot();              //由readyRead信号触发，开始读取数据
    void isFinished(bool ok);         //主动资源释放，释放子线程与堆中对象
    void new_slot();                  //由自己线程started信号触发，在线程中实例化server等对象
    void handleAdoptCmd(char state); //处理来自UI线程 采集起停的槽函数


   void closeFile();//处理UI的信号，关闭文件，结束采集与记录

};

#endif // PRESSSENSOR_H
