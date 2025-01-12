#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>
#include <QVector>
#include <QThread>
#include <QString>
#include <QDebug>
#include "mysql_test.h"

class DatabaseWorker : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseWorker(QObject *parent = nullptr) : QObject(parent) {}

    void setup(const QString &host, int port, const QString &dbname, const QString &user, const QString &password,
               const QString &databaseName, int index, const QVector<int> &pressureData);

public slots:
    void process();

signals:
    void finished();

private:
    QString host;
    int port;
    QString dbname;
    QString user;
    QString password;
    QString databaseName;
    int index;
    QVector<int> pressureData;
};

#endif // DATABASEWORKER_H
