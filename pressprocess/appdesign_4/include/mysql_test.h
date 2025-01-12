#ifndef MYSQL_TEST_H
#define MYSQL_TEST_H
#include <QtSql/QSqlDatabase>
    #include <QtSql/QSqlQuery>
    #include <QtSql/QSqlRecord>
    #include <QDebug>
#include<QVector>

class MysqlTool
{
public:
    MysqlTool(const QString& host, int port, const QString& dbName, const QString& user, const QString& password);

    ~MysqlTool();

    bool createTable(const QString& tableName, const QStringList& fields);

    bool insertData(const QString& tableName, const QStringList& fields, const QVariantList& values);

    bool deleteData(const QString& tableName, const QString& condition);

    bool updateData(const QString& tableName, const QString& setClause, const QString& condition);

    bool selectData(const QString& tableName, const QStringList& fields, const QString& condition, QList<QVariantList>& result);
    bool createDatabase(const QString& dbName); // 创建数据库
    void setDatabase(const QString& dbName);    // 切换到指定数据库

    QStringList getFields(const QString& tableName);
    bool execute(const QString& query);
    void mysqlmain(MysqlTool mysql, QString databaseName, int index, QVector<int> datas, QString tableName);

    QString serializeVector(const QVector<int>& vec);
    QVector<int> deserializeVector(const QString& jsonString);

private:
    QSqlDatabase db_;
};

#endif // MYSQL_TEST_H
