#ifndef SCHEDULE_ITEM_H
#define SCHEDULE_ITEM_H
#include <QObject>
#include <QDateTime>
#include <QCheckBox>
#include "alarm.h"
#include <QTimer>
#include "myqtimer.h"

class ScheduleItem : public QObject
{
    Q_OBJECT
public:
    ScheduleItem(){}
    ScheduleItem(QDateTime d, QString msg){ m_date = d; m_message = msg; } // для напоминания с фиксированной датой
    ScheduleItem(QTime alarmTime, QString msg) // для напоминания по дням недели
            { m_wDaysTime = alarmTime, m_message = msg; }

    ~ScheduleItem() { if (m_weekDays != nullptr) delete m_weekDays; }
    QCheckBox *m_chBox = nullptr;
    QString getMsg() const { return m_message;}
    QDateTime getDate() const { return m_date;}
    bool chBoxisChecked() const { return m_chBox->isChecked(); }

    void setWeekDaysFlag(bool hasWeekDays);
    bool getWDaysFlag() const { return m_flagWeekDays;}
    void setWeekDaysMap(QMap<int, bool> &map) { *m_weekDays = map; }

    void setFlagToRemove(); // устанавливаем true, если напоминание сработало и надо его удалять
    bool getFlagToRemove() { return m_toRemove; }

    QTime getWDaysTime() const { return m_wDaysTime; }
    QMap <int, bool>* getWDaysMap() const { return m_weekDays; }

    bool checkIfCurrentReminderWorked(int dayIdx) {return m_weekDays->value(dayIdx);}
    bool checkIfReminderWorked();
    bool checkTimeToRemind(QDateTime now); // проверка на совпадение текущего времени и времени напоминания
    void openReminderWindow();
    void changeReminderState(QDateTime now); // устанавливаем true - признак того, что напом-е отработало

    friend void writeDataToBin(QList<ScheduleItem*> list);
    friend void readDataFromBin(QList<ScheduleItem*>& remindersKeeper);

private:
    QString m_message;
    bool m_flagWeekDays = false;
    QDateTime m_date;
        // задействовано что-то одно: или m_date, или m_weekDays
    QMap <int, bool> *m_weekDays = nullptr; // map хранит индекс дня недели и флаг (true - если напоминание отработало)
    QTime m_wDaysTime;
    bool m_toRemove = false;

    void serialize(QDataStream &stream);
    void deserialize(QDataStream &stream);
};

#endif // SCHEDULE_ITEM_H
