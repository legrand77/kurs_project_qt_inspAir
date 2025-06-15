#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <QTime>

static const int INTERVAL = 5000;

class Timer : public QObject
{
    Q_OBJECT

public:
    Timer(QObject *parent = nullptr);
    ~Timer();

    void StartTimer();

signals:
    void sig_Reconnect();

private:
    QTimer *timer;

    void TimeToReconnect();
};

#endif // TIMER_H
