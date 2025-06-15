#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent>
#include <QMessageBox>
#include "database.h"
#include "timer.h"
#include "airport_statistic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void rcv_StatusConnectionToDB(bool status);
    void rcv_StatusRequest(QSqlError err);
    void rcv_Airports(QSqlQueryModel* model);
    void rcv_ArrivalDeparture(QSqlQueryModel* model);
    void rcv_StatisticClosed();

signals:
    void sig_StartTimer();

private slots:
    void on_pb_GetShedule_clicked();
    void on_pb_ShowWorkload_clicked();

private:
    Ui::MainWindow *ui;
    Database* data_base;
    Timer* timer;
    QMessageBox *msg;
    QMap<QString, QString> airports;
    AirportStatistic* statistic;

    void ConnectToDB();
    void InitialSetup();
    void GetAirports();
};
#endif // MAINWINDOW_H
