#ifndef REMINDERWINDOW_H
#define REMINDERWINDOW_H

#include <QLabel>
#include <QDialog>
#include "schedule_item.h"
#include "myqtimer.h"

namespace Ui {
class ReminderWindow;
}

class ReminderWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderWindow(QWidget *parent = 0);
    explicit ReminderWindow(ScheduleItem *scheduleItem);
    ~ReminderWindow();

private:
    Ui::ReminderWindow *ui;
    MyQTimer *m_timer = nullptr;
    int m_timePassedVar = 0;     // показывает кол-во минут, прошедших с открытия окна напоминания
    ScheduleItem *m_schItem;

    void showEvent(QShowEvent * event);
    void handleTimePassedLabel(); // метод раз в минуту обновляет лейбл, показывающий, сколько времени прошло с момента напоминания
    void showTimePassed();
};

#endif // REMINDERWINDOW_H
