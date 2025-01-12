#include "databaseworker.h"
#include "mysql_test.h"

void DatabaseWorker::setup(const QString &host, int port, const QString &dbname, const QString &user, const QString &password,
           const QString &databaseName, int index, const QVector<int> &pressureData)
{
    this->host = host;
    this->port = port;
    this->dbname = dbname;
    this->user = user;
    this->password = password;
    this->databaseName = databaseName;
    this->index = index;
    this->pressureData = pressureData;
}

void DatabaseWorker::process()
{
    // 线程中执行数据库操作
    MysqlTool mysqlTool(host, port, databaseName, user, password);

    if (!mysqlTool.createDatabase(databaseName)) {
        qDebug() << "Failed to create or select database!";
        return;
    }

    mysqlTool.setDatabase(databaseName);

    // 调用 mysqlmain
    mysqlTool.mysqlmain(mysqlTool, databaseName, index, pressureData, dbname);

    qDebug() << "Database operation completed.";
    emit finished();
}