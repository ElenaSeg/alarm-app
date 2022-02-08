#ifndef MYQTIMER_H
#define MYQTIMER_H
#include <QTimer>
#include <QDebug>

class MyQTimer : public QTimer
{
public:
    explicit MyQTimer(QObject *parent = Q_NULLPTR){ qDebug()<< "Таймер создан"; }
    ~MyQTimer() { qDebug() << "Таймер удален"; }
};

#endif // MYQTIMER_H
