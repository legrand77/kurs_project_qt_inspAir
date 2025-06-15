#include "timer.h"

/*!
 * @brief Конструктор
 */
Timer::Timer(QObject *parent) : QObject{parent} {
    timer = new QTimer(this);
    timer->setInterval(INTERVAL);
    connect(timer, &QTimer::timeout, this, &Timer::TimeToReconnect);
}
/*!
 * @brief Деструктор
 */
Timer::~Timer() {
    delete timer;
}
/*!
 * @brief Реконнект
 */
void Timer::TimeToReconnect() {
    timer->stop();
    emit sig_Reconnect();
}
/*!
 * @brief Старт таймера
 */
void Timer::StartTimer() {
    timer->start();
}
