#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QTableWidget>
#include <QSqlError>
#include <QSqlDatabase>
#include <QtSql>
#include <QSqlQueryModel>
#include <QtConcurrent>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "demo"

class Database : public QObject
{
    Q_OBJECT

public:
    enum fieldsForConnect{
        hostName = 0,
        dbName = 1,
        login = 2,
        pass = 3,
        port = 4
    };

    Database(QObject *parent = nullptr);
    ~Database();

    void AddDataBase(QString driver, QString nameDB = "");
    QSqlError GetLastError();
    void ConnectToDB();
    void DisconnectFromDataBase(QString nameDb = "");
    void GetAirports();
    void GetArrivals(const QString& airport_code, const QString& date);
    void GetDepartures(const QString& airport_code, const QString& date);
    void GetDataPerYear(const QString &airport_code);
    void GetDataPerMonth(const QString &airport_code);

signals:
    void sig_SendStatusConnection(bool status_connection);
    void sig_SendStatusRequest(QSqlError err);
    void sig_SendAirports(QSqlQueryModel* model);
    void sig_SendArrivals(QSqlQueryModel* model);
    void sig_SendDepartures(QSqlQueryModel* model);
    void sig_SendDataPerYear(QSqlQueryModel* model);
    void sig_SendDataPerMonth(QSqlQueryModel* model);

private:
    QSqlDatabase* data_base;
    bool status_connection;

    QString ConvertDate(const QString& date);
};

#endif // DATABASE_H
