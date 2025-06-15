#ifndef AIRPORT_STATISTIC_H
#define AIRPORT_STATISTIC_H

#include <QWidget>
#include <QtCharts>
#include <QSqlQueryModel>
#include <QSqlRecord>

#define NUM_GRAPHS 2

namespace Ui {
class AirportStatistic;
}

class AirportStatistic : public QWidget
{
    Q_OBJECT

public:
    explicit AirportStatistic(QWidget *parent = nullptr);
    ~AirportStatistic();

    void SetAirportName(QString name_);

public slots:
    void rcv_DataPerYear(QSqlQueryModel* model);
    void rcv_DataPerMonth(QSqlQueryModel* model);

private slots:
    void on_pb_Close_clicked();
    void on_cb_Months_highlighted(int index);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void sig_Closed();

private:
    Ui::AirportStatistic *ui;
    QString name;

    QChart* year_chart;
    QChartView* year_chart_view;
    QBarSeries* year_series;


    QChart* month_chart;
    QChartView* month_chart_view;
    QLineSeries* month_series;
    QMap<int, QVector<QPointF>>* months_data;

    void InitialSetup();
    void ChartsSetup();
    void UpdateYearGraph();
    void UpdateMonthGraph(int month_index);
    void PrintStoredData();

    QStringList categories  {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
};

#endif // AIRPORT_STATISTIC_H
