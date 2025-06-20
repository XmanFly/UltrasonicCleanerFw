#ifndef TIMERWORKER_H
#define TIMERWORKER_H

#include <QTimer>

class TimerWorker : public QObject {
    Q_OBJECT
public:
    TimerWorker(int interval) :
        QObject(),
        m_interval(interval)
    {
    }
public slots:
    void init() {
        t = new QTimer();
        t->setTimerType(Qt::PreciseTimer);
        t->setInterval(m_interval);
        connect(t, &QTimer::timeout, this, &TimerWorker::tick);

        start();
    }
    void start() { t->start(); }
signals:
    void tick();            // 1ms 打点信号

private:
    int m_interval;
    QTimer *t;
};


#endif // TIMERWORKER_H
