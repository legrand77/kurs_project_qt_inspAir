#include "database.h"
/*!
 * @brief Конструктор
 */
Database::Database(QObject *parent) : QObject{parent} {
    data_base = new QSqlDatabase();
}
/*!
 * @brief Деструктор
 */
Database::~Database() {
    delete data_base;
}

//!< Main methods
/*!
 * @brief Инициализация БД
 */
void Database::AddDataBase(QString driver, QString nameDB) {
    *data_base = QSqlDatabase::addDatabase(driver, nameDB);
}
/*!
 * @brief Инициализация списка данных и подключение к БД
 */
void Database::ConnectToDB() {
    data_base->setHostName("981757-ca08998.tmweb.ru");
    data_base->setDatabaseName(DB_NAME);
    data_base->setUserName("netology_usr_cpp");
    data_base->setPassword("CppNeto3");
    data_base->setPort(5432);

    status_connection = data_base->open();
    emit sig_SendStatusConnection(status_connection);
}
/*!
 * @brief Отсоединение от БД
 * @param nameDb имя БД
 */
void Database::DisconnectFromDataBase(QString nameDb) {
    *data_base = QSqlDatabase::database(nameDb);
    data_base->close();
}
/*!
 * @brief Получаем последнюю ошибку из БД
 */
QSqlError Database::GetLastError() {
    return data_base->lastError();
}

//!< Requests
/*!
 * @brief Получаем список аэропортов
 */
void Database::GetAirports() {
    if (status_connection) {
        QString request = "SELECT airport_name->>'ru' AS name, airport_code FROM bookings.airports_data ORDER BY name";
        QSqlError err;
        auto query_model = new QSqlQueryModel(this);

        query_model->setQuery(request, *data_base);
        query_model->setHeaderData(0, Qt::Horizontal, tr("Аэропорт"));
        query_model->setHeaderData(1, Qt::Horizontal, tr("Код аэропорта"));

        err = query_model->lastError();

        if (err.type() != QSqlError::NoError) {
           emit sig_SendStatusRequest(err);
        } else {
           emit sig_SendAirports(query_model);
        }
    }
}
/*!
 * @brief Получаем список прибытий
 * @param airport_code код аэропорта
 * @param date дата, за какой период
 */
void Database::GetArrivals(const QString& airport_code, const QString& date) {
    if (status_connection) {
        QString parsed_date = ConvertDate(date); // дату нужно конвертировать
        QString request = "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' AS name "
                          "FROM bookings.flights f "
                          "JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport "
                          "WHERE (f.arrival_airport  = '" + airport_code + "' AND f.scheduled_arrival::date = date('" + parsed_date + "')) "
                          "ORDER BY name";

        auto query_model = new QSqlQueryModel(this);
        QSqlError err;
        query_model->setQuery(request, *data_base);
        query_model->setHeaderData(0, Qt::Horizontal, tr("Рейс"));
        query_model->setHeaderData(1, Qt::Horizontal, tr("Прибытие"));
        query_model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт"));

        err = query_model->lastError();

        if (err.type() != QSqlError::NoError) {
            emit sig_SendStatusRequest(err);
        } else {
            emit sig_SendArrivals(query_model);// отсылаем для вывода на графике  аэропорты
        }
    }
}
/*!
 * @brief Получаем список вылетов
 * @param airport_code код аэропорта
 * @param date дата, за какой период
 */
void Database::GetDepartures(const QString &airport_code, const QString& date) {
    if (status_connection) {
        QString parsed_date = ConvertDate(date);
        QString request = "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' AS name "
                          "FROM bookings.flights f "
                          "JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport "
                          "WHERE (f.departure_airport  = '" + airport_code + "' AND f.scheduled_departure::date = date('" + parsed_date + "')) "
                          "ORDER BY name";

        auto query_model = new QSqlQueryModel(this);
        QSqlError err;
        query_model->setQuery(request, *data_base);
        query_model->setHeaderData(0, Qt::Horizontal, tr("Рейс"));
        query_model->setHeaderData(1, Qt::Horizontal, tr("Вылет"));
        query_model->setHeaderData(2, Qt::Horizontal, tr("Аэропорт"));

        err = query_model->lastError();

        if (err.type() != QSqlError::NoError) {
            emit sig_SendStatusRequest(err);
        } else {
            emit sig_SendDepartures(query_model);// отсылаем для вывода на графике  список вылетов
        }
    }
}
/*!
 * @brief Получаем данные аэропорта за год
 * @param airport_code код аэропорта
 */
void Database::GetDataPerYear(const QString &airport_code) {
    if (status_connection) {
        QString request = "SELECT count(flight_no), date_trunc('month', scheduled_departure) AS Month "
                          "FROM bookings.flights f "
                          "WHERE (scheduled_departure::date > date('2016-08-31') "
                          "AND scheduled_departure::date <= date('2017-08-31')) AND "
                          "(departure_airport = '" + airport_code + "' or arrival_airport = '" + airport_code + "') "
                          "GROUP BY Month";

        auto query_model = new QSqlQueryModel(this);
        QSqlError err;
        query_model->setQuery(request, *data_base);
        query_model->setHeaderData(0, Qt::Horizontal, tr("Количество рейсов"));
        query_model->setHeaderData(1, Qt::Horizontal, tr("Месяц"));

        err = query_model->lastError();

        if (err.type() != QSqlError::NoError) {
            emit sig_SendStatusRequest(err);
        } else {
            emit sig_SendDataPerYear(query_model);// отсылаем для вывода на графике  годы
        }
    }
}

/*!
 * @brief Получаем данные аэропорта за год по месяцам
 * @param airport_code код аэропорта
 */
void Database::GetDataPerMonth(const QString &airport_code) {
    if (status_connection) {
        QString request = "SELECT count(flight_no), date_trunc('day', scheduled_departure) AS Day "
                          "FROM bookings.flights f "
                          "WHERE (scheduled_departure::date > date('2016-08-31') "
                          "AND scheduled_departure::date <= date('2017-08-31')) AND "
                          "(departure_airport = '" + airport_code + "' or arrival_airport = '" + airport_code + "') "
                          "GROUP BY Day";

        auto query_model = new QSqlQueryModel(this);
        QSqlError err;
        query_model->setQuery(request, *data_base);
        query_model->setHeaderData(0, Qt::Horizontal, tr("Количество рейсов"));
        query_model->setHeaderData(1, Qt::Horizontal, tr("День"));

        err = query_model->lastError();

        if (err.type() != QSqlError::NoError) {
            emit sig_SendStatusRequest(err);
        } else {
            emit sig_SendDataPerMonth(query_model);// отсылаем для вывода на графике  месяцы
        }
    }
}

//!< UTILS
/*!
 * @brief Функция для конвертации даты для запроса(request) к ДБ
 * @param date дата
 */
QString Database::ConvertDate(const QString &date) {
    QString day, month, year;
    for(int i = 0; i < date.size(); i++) {
        if(i < 2) {
            day += date[i];
        }

        if((i > 2) && (i < 5)) {
            month += date[i];
        }

        if(i > 5) {
            year += date[i];
        }
    }

    QString result = year + "-" + month + "-" + day;
    return result;
}
