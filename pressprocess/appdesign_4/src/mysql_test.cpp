#include "mysql_test.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

#include <QString>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QThread>

MysqlTool::MysqlTool(const QString& host, int port, const QString& dbName, const QString& user, const QString& password) {
    // 将 QThread::currentThreadId() 转换为 uintptr_t，再格式化为字符串
    QString connectionName = QString("mysql_connection_%1").arg(reinterpret_cast<uintptr_t>(QThread::currentThreadId()));

    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName); // 移除旧连接
    }

    db_ = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db_.setHostName(host);
    db_.setPort(port);
    db_.setUserName(user);
    db_.setPassword(password);
    db_.setDatabaseName(dbName);

    // 打开数据库连接
    if (!db_.open()) {
        qDebug() << "Failed to connect to database server or database:" << db_.lastError().text();
    }
}

MysqlTool::~MysqlTool() {
    QString connectionName = db_.connectionName();
    db_.close();
    QSqlDatabase::removeDatabase(connectionName); // 完全移除连接
}

bool MysqlTool::createDatabase(const QString& dbName) {
    QSqlQuery query(db_);
    QString sql = QString("CREATE DATABASE IF NOT EXISTS %1").arg(dbName);
    if (!query.exec(sql)) {
        qDebug() << "Failed to create database:" << query.lastError().text();
        return false;
    }
    setDatabase(dbName);
    return true;
}

void MysqlTool::setDatabase(const QString& dbName) {
    if (db_.isOpen()) {
        db_.close();
    }
    db_.setDatabaseName(dbName);
    if (!db_.open()) {
        qDebug() << "Failed to connect to database:" << db_.lastError().text();
    }
}

bool MysqlTool::createTable(const QString& tableName, const QStringList& fields) {
    QString sql = QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(tableName).arg(fields.join(", "));
    QSqlQuery query(db_);
    if (!query.exec(sql)) {
        qDebug() << "Failed to create table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool MysqlTool::insertData(const QString& tableName, const QStringList& fields, const QList<QVariant>& values) {
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                      .arg(tableName)
                      .arg(fields.join(", "))
                      .arg(QString("?, ").repeated(values.size()).chopped(2));

    QSqlQuery query(db_);
    query.prepare(sql);

    for (const QVariant& value : values) {
        query.addBindValue(value);
    }

    if (!query.exec()) {
        qDebug() << "Failed to insert data:" << query.lastError().text();
        return false;
    }
    return true;
}

bool MysqlTool::selectData(const QString& tableName,
                           const QStringList& fields,
                           const QString& condition,
                           QList<QVariantList>& result) {
    QString queryStr = QString("SELECT %1 FROM %2").arg(fields.join(", ")).arg(tableName);
    if (!condition.isEmpty()) {
        queryStr += QString(" WHERE %1").arg(condition);
    }

    QSqlQuery query(db_);
    if (!query.exec(queryStr)) {
        qDebug() << "SQL Execution Failed:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        QVariantList row;
        for (int i = 0; i < fields.size(); ++i) {
            row.append(query.value(i));
        }
        result.append(row);
    }
    return true;
}

QString MysqlTool::serializeVector(const QVector<int>& vec) {
    QJsonArray jsonArray;
    for (int val : vec) {
        jsonArray.append(val);
    }
    return QString(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact));
}

QVector<int> MysqlTool::deserializeVector(const QString& jsonString) {
    QVector<int> vec;
    QJsonArray jsonArray = QJsonDocument::fromJson(jsonString.toUtf8()).array();
    for (const QJsonValue& val : jsonArray) {
        vec.append(val.toInt());
    }
    return vec;
}

void MysqlTool::mysqlmain(MysqlTool mysql, QString databaseName, int index, QVector<int> datas, QString tableName) {
    if (!mysql.createDatabase(databaseName)) {
        qDebug() << "Failed to create or select database!";
        return;
    }

    mysql.setDatabase(databaseName);

    QStringList fields = {"times INT", "data JSON"};
    if (!mysql.createTable(tableName, fields)) {
        qDebug() << "Failed to create table!";
        return;
    }

    QString dataString = mysql.serializeVector(datas);

    QStringList insertFields = {"times", "data"};
    QVariantList insertValues = {index, dataString};
    if (!mysql.insertData(tableName, insertFields, insertValues)) {
        qDebug() << "Failed to insert data!";
        return;
    }

    QStringList selectFields = {"times", "data"};
    QList<QVariantList> selectResult;
    if (!mysql.selectData(tableName, selectFields, "", selectResult)) {
        qDebug() << "Failed to select data!";
        return;
    }

    for (const auto& record : selectResult) {
        int times = record[0].toInt();
        QString data = record[1].toString();
        QVector<int> retrievedVector = mysql.deserializeVector(data);
        qDebug() << "times:" << times << ", data (vector):" << retrievedVector;
    }
}
