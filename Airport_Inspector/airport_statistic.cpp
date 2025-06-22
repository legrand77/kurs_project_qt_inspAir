#include "airport_statistic.h"
#include "ui_airport_statistic.h"
/*!
 * @brief Конструктор
 */
AirportStatistic::AirportStatistic(QWidget *parent)
    : QWidget(parent), ui(new Ui::AirportStatistic) {
    ui->setupUi(this);
    InitialSetup();
    ChartsSetup();

    months_data = new QMap<int, QVector<QPointF>>();  //контейнер для данных по месяцам
}
/*!
 * @brief Деструктор
 */
AirportStatistic::~AirportStatistic() {
    delete months_data;
    delete ui;
}
/*!
 * @brief Установить имя выбранного аэропорта
 * @param name_ имя
 */
void AirportStatistic::SetAirportName(QString name_) {
    this->name = name_;
    ui->lb_Airport->setText("Загруженность аэропорта " + name);
}
/*!
 * @brief Первоначальная настройка в конструкторе
 */
void AirportStatistic::InitialSetup() {
    ui->tabWidget->setCurrentIndex(0);
    ui->cb_Months->setCurrentIndex(0);
    for (auto it: categories) {
        ui->cb_Months->addItem(it);
     }

    ui->pb_Close->setText("Закрыть статистику");
    ui->tabWidget->setTabText(0, "За год");
    ui->tabWidget->setTabText(1, "За месяц");
}
/*!
 * @brief Первоначальная настройка графиков
 */
void AirportStatistic::ChartsSetup() {
    year_chart = new QChart();
    year_chart_view = new QChartView(year_chart);
    year_series = new QBarSeries(this); // заполняется из базы данных

    month_chart = new QChart();
    month_chart_view = new QChartView(month_chart);
    month_series = new QLineSeries(this); // заполняется из базы данных

    year_chart->addSeries(year_series);// добавление данных (годы)
    year_chart->legend()->hide();
    year_chart->setTitle("Статистика за год:");
    year_chart->setAnimationOptions(QChart::SeriesAnimations);

    month_chart->addSeries(month_series);// добавление данных (месяцы)
    month_chart->legend()->hide();
    month_chart->setTitle("Статистика за месяц:");
    month_chart->setAnimationOptions(QChart::SeriesAnimations);


    auto year_AxisX = new QBarCategoryAxis(this); // пример  https://doc.qt.io/qt-6/qtcharts-barchart-example.html
    year_AxisX->setTitleText("Месяцы");
    year_AxisX->append(categories);
    year_chart->addAxis(year_AxisX, Qt::AlignBottom);
    year_series->attachAxis(year_AxisX);// добавление данных (годы)

    auto year_AxisY = new QValueAxis(this);
    year_AxisY->setTitleText("Кол-во рейсов");
    year_AxisY->setLabelFormat("%i");
    year_chart->addAxis(year_AxisY, Qt::AlignLeft);
    year_series->attachAxis(year_AxisY);// добавление данных (годы)

    auto month_AxisX = new QValueAxis(this);
    month_AxisX->setTitleText("дни");
    month_AxisX->setLabelFormat("%i");
    month_chart->addAxis(month_AxisX, Qt::AlignBottom);
    month_series->attachAxis(month_AxisX);// добавление данных (месяцы)

    auto month_AxisY = new QValueAxis(this);
    month_AxisY->setTitleText("Кол-во рейсов");
    month_AxisY->setLabelFormat("%i");
    month_chart->addAxis(month_AxisY, Qt::AlignLeft);
    month_series->attachAxis(month_AxisY);// добавление данных (месяцы)

    year_chart_view->setRenderHint(QPainter::Antialiasing);
    month_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->wid_YearChart->setLayout(new QVBoxLayout());
    ui->wid_YearChart->layout()->addWidget(year_chart_view);

    ui->wid_MonthChart->setLayout(new QVBoxLayout());
    ui->wid_MonthChart->layout()->addWidget(month_chart_view);
}
/*!
 * @brief Получение данных за год из БД
 */
void AirportStatistic::rcv_DataPerYear(QSqlQueryModel* model) {
year_series->clear();                                // добавление данных (годы)
QBarSet *set = new QBarSet("Flights");

int month{12};
int monthCarrell{8};
for (int i = (month-monthCarrell); i < month; i++) {
 int flights_count = model->record(i).value(0).toInt();// добавляем с января по август т.к. запрос с августа по август
 //qDebug()<<flights_count; // проверка значений
 //qDebug()<<model->record(i).value(1).toDate();// проверка значений
 *set << flights_count; // заполняем ось
}
for (int i = 0; i < (month-monthCarrell); i++) {
 int flights_count = model->record(i).value(0).toInt();// добавляем с сентября по декабрь т.к. запрос с августа по август
 //qDebug()<<flights_count;// проверка значений
 //qDebug()<<model->record(i).value(1).toDate();// проверка значений
 *set << flights_count;
}
year_series->append(set);  // продолжаем заполнять ось                            // добавление данных (годы)

QList<QAbstractAxis*> axes = year_chart->axes(Qt::Vertical);
if (!axes.isEmpty()) {
    QValueAxis* axisY = static_cast<QValueAxis*>(axes.first());
    float minY = 0;
    float maxY = 0;

    for (int i = 0; i < 12; i++) {
        int point = model->record(i).value(0).toInt();
         if (point < minY) {
             minY = point;
            }
          if (point > maxY) {
              maxY = point;
            }
    }

    axisY->setRange(minY, maxY + 10); // значение по оси

}
}
/*!
 * @brief Получение данных по месяцам из БД
 */
void AirportStatistic::rcv_DataPerMonth(QSqlQueryModel* model) {
    months_data->clear();


        for (int i = 0; i < model->rowCount(); i++) { // количество строк
            int flights_count = model->record(i).value(0).toInt(); // значение записи о столбца
            QDate day_date = model->record(i).value(1).toDate(); // значение записи 1 столбца

            //qDebug()<<flights_count;// проверка значений
            //qDebug()<<model->record(i).value(1).toDate();


            int day_number = day_date.day();
            int month_number = day_date.month();

            if (!months_data->contains(month_number)) {
                months_data->insert(month_number, QVector<QPointF>());
            }

            QVector<QPointF>& data = months_data->operator[](month_number);
            data.append(QPointF(day_number, flights_count));

        }

        int current_month = ui->cb_Months->currentIndex() + 1;
        UpdateMonthGraph(current_month);//Обновление графика за (месяцы)
}

/*!
 * @brief Обновление графика за выбранный месяц
 * @param month_index выбранный месяц
 */
void AirportStatistic::UpdateMonthGraph(int month_index) {
    month_series->clear();                                                                       // добавление данных (месяцы)

    if (!months_data->contains(month_index)) {

        return;
    }

    const QVector<QPointF>& data = months_data->operator[](month_index);
    month_series->append(data);                                                                       // добавление данных (месяцы)

    QList<QAbstractAxis*> axesX = month_chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> axesY = month_chart->axes(Qt::Vertical);

    if (!axesX.isEmpty() && !axesY.isEmpty()) {
        QValueAxis* axisX = static_cast<QValueAxis*>(axesX.first());
        QValueAxis* axisY = static_cast<QValueAxis*>(axesY.first());

        qreal minX = data.first().x();
        qreal maxX = data.first().x();
        qreal minY = data.first().y();
        qreal maxY = data.first().y();

        for (const auto& point : data) {
            if (point.x() < minX) minX = point.x();
            if (point.x() > maxX) maxX = point.x();
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }

        axisX->setRange(minX, maxX);
        axisY->setRange(minY - 1, maxY + 1);

        axisY->setTickCount((maxY - minY) + 3);
        //axisY->setMinorTickCount((maxY - minY) + 1);
        //qDebug() << "Диапазон X-оси графика за месяц установлен на:" << minX << "до" << maxX;
        //qDebug() << "Диапазон Y-оси графика за месяц установлен на:" << minY << "до" << maxY;

        month_chart->update();
    }
}

/*!
 * @brief Закрытие окна
 */
void AirportStatistic::on_pb_Close_clicked() {
    ui->cb_Months->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    emit sig_Closed();
    close();
}
/*!
 * @brief Смена месяца
 * @param index индекс месяца
 */
void AirportStatistic::on_cb_Months_highlighted(int index) {
    int month = index + 1;
    UpdateMonthGraph(month);
}

void AirportStatistic::closeEvent(QCloseEvent *event) {
    ui->cb_Months->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    emit sig_Closed();
    QWidget::closeEvent(event);
}
