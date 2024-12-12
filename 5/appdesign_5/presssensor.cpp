#include "presssensor.h"
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QTime>
#include <QDateTime>

PressSensor::PressSensor(QObject *parent) : QObject(parent)
{
//    my_socket = new QTcpSocket;
//    myServer  = new QTcpServer;
//    timestamp1 =  QDateTime::currentDateTime().toMSecsSinceEpoch();

//    initTcpServer();
    this->moveToThread(&PressSensor_Thread);
//    my_socket->moveToThread(&PressSensor_Thread);
//    myServer->moveToThread(&PressSensor_Thread);
    PressSensor_Thread.start();
    connect(&PressSensor_Thread,&QThread::started,this,&PressSensor::new_slot);
}

void PressSensor::new_slot()
{
//    qDebug()<<"new_slot called";

    qRegisterMetaType<QVariant>("QVariant");
    my_socket = new QTcpSocket;
    myServer  = new QTcpServer;
    timestamp1 =  QDateTime::currentDateTime().toMSecsSinceEpoch();

    initTcpServer();
    QString fileName("D:/projectData/pressSensorData.txt");
    QFile::remove(fileName);
    file.setFileName("D:/projectData/pressSensorData.txt");
    file.open(QIODevice::ReadWrite);//读写，追加模式
    if(file.isOpen())
    {

    }





}

void PressSensor::handleAdoptCmd(char state)
{

    my_socket->write(&state,1);
    qDebug()<<"handleAdoptState is called "<<QThread::currentThreadId();
}

PressSensor::~PressSensor()
{
qDebug()<<"~PressSensor()";
    myServer->close();
    my_socket->close();
    myServer->deleteLater();
    my_socket->deleteLater();
    PressSensor_Thread.quit();
    PressSensor_Thread.wait();
    PressSensor_Thread.deleteLater();
}

void PressSensor::initTcpServer()
{
    quint16 port=8000;
    QString ip="127.0.0.1";
    QHostAddress adress(ip);
    myServer->listen(QHostAddress::Any,port);
    if(myServer->isListening())
    {

        qDebug()<<"PressSensor isListening true";
        emit islisten(1);
    }
//    connect(myServer,SIGNAL(newConnection()),this,SLOT(newConnection_slot()));
    connect(myServer,&QTcpServer::newConnection,this,&PressSensor::newConnection_slot);
//    qDebug()<<"initTcpServer()";
    qDebug()<<"initTcpServer pressSenor线程ID "<<QThread::currentThreadId();
}



void PressSensor::newConnection_slot()
{
    my_socket = myServer->nextPendingConnection();
    connect(my_socket,&QTcpSocket::readyRead,this,&PressSensor::readRead_slot);
    qDebug()<<"PressSensor newConnection_slot"<<QThread::currentThreadId();
}


void PressSensor::readRead_slot()
{
    static uint32_t t = 0;

//    QTcpSocket* tcpsocket=static_cast<QTcpSocket*>(sender());
//    QString ip = tcpsocket->peerAddress().toString();
//    quint16 port = tcpsocket->peerPort();
//    qDebug()<<"ip:"<<ip<<"port"<<port;

//    qDebug()<<"readRead_slot"<<QThread::currentThreadId();
    QByteArray recv=my_socket->readAll();
    QByteArray end;
    end.resize(3);
    end[0]=0xfd;end[1]=0xfe; end[2]=0xff;
    if(!recv.isEmpty())
    {
        pressSensorBuf.append(recv);
        while (pressSensorBuf.size()>=517)
        {
            QByteArray temp;
            uint16_t count;
            if(pressSensorBuf.contains(end))
            {
                temp = pressSensorBuf.left(pressSensorBuf.indexOf(end));
                pressSensorBuf=pressSensorBuf.right(pressSensorBuf.length()-pressSensorBuf.indexOf(end)-3);
            }
            else
            {
                qWarning()<<"data more than 515 but without tail";
            }
            if(temp.length()==514)
            {
                t++;
                timestamp2 = QDateTime::currentDateTime().toMSecsSinceEpoch(); //毫秒级
                //qDebug()<<"timestamp: "<<timestamp2-timestamp1;
                uint8_t count_H= temp.at(512);
                uint8_t count_L= temp.at(513);
                count = (count_H<<8)+count_L;
                //qDebug()<<"MCU sequence "<<count;
                saveData(temp);
            }
            else
            {
                qWarning()<<"data loss"<<"---temp.length()---"<<temp.length();
            }
        }
        //qDebug()<<"剩余数据 "<<pressSensorBuf.size()<<"t ="<<t;
    }
}

void PressSensor::saveData(QByteArray &data)
{
    uint16_t vaildData[257]={0};
    QVector<int> toUI(257);
    for(int i=0;i<257;++i)
    {
        //向uint8_t的显示转换是必要的，因为at返回的是char型数据，属于无符号
        vaildData[i]=((uint8_t)data.at(2*i)<<8)+(uint8_t)data.at(2*i+1);
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out<<vaildData[256]<<" ";//采样序列数，打印在首列
    for(int i=0;i<256;++i)
    {
        out<<vaildData[i]<<" ";
        toUI[i]=vaildData[i];
    }
    out<<"\n";


    //刷新UI，30ms*8刷新一次即可
    if(vaildData[256]%4 == 0)
    {
        QVariant dataVar;
        dataVar.setValue(toUI);
        emit updateUIpressData(dataVar);
    }
}

void PressSensor::isFinished(bool ok)
{
    if(ok == true)
    {
        qDebug()<<"PressSensor is finished :" <<"QThread iD:" <<QThread::currentThreadId();

        file.close();
        myServer->close();
        my_socket->close();
        myServer->deleteLater();
        my_socket->deleteLater();

        PressSensor_Thread.quit();
        PressSensor_Thread.wait();
    }
}


void PressSensor::closeFile()
{

}

