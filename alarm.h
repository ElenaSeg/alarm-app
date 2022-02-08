#ifndef ALARM_H
#define ALARM_H

#include <QMainWindow>
#include <QShortcut>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QList>
#include "schedule_item.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Alarm; }
QT_END_NAMESPACE

class ScheduleItem;

class Alarm : public QMainWindow
{
    Q_OBJECT

public:
    Alarm(QWidget *parent = nullptr);
    ~Alarm();
    void readSettings();
    void initTimer();

public slots:
    void deletePassedReminder();
    void timeCheck();
    void ShowListOfReminders();

signals:
    void closeApp();

private slots:
    void save_slot();
    void handleDateChBoxes();

private:
    Ui::Alarm *ui;
    QTimer *m_timer;

    bool m_flagWeekDays = false; // true - если напоминание по дням недели, false - если по дате
    QList<ScheduleItem*> m_remindersKeeper; // хранит список всех созданных напоминаний
    QMap<QCheckBox*, int> m_dayChBoxes;

    void saveWithWeekDays(/*bool repeatEveryWeek*/);
    void saveWithDate(QDateTime currAlarmDate);
    bool checkDate(QDateTime currAlarmDate);
    void checkIfAllTimersStopped(ScheduleItem* curReminder);
    void closeEvent(QCloseEvent * event);
};

#endif // ALARM_H
