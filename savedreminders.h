#ifndef SAVEDREMINDERS_H
#define SAVEDREMINDERS_H

#include <QDialog>
#include "schedule_item.h"

namespace Ui {
class SavedRemindersDialog;
}

class SavedRemindersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SavedRemindersDialog(QWidget *parent,
                            QList<ScheduleItem*> *remaindersKeeper);
    ~SavedRemindersDialog();

private slots:
    void deleteReminders();
private:
    Ui::SavedRemindersDialog *ui;
    QList<ScheduleItem*> *m_remindersKeeper;
    void placeRemindersInTable();
    void fillRow(ScheduleItem *reminder, int rowNumber);
    void stopAllTimersOfReminder(ScheduleItem* reminder);
};

#endif // SAVEDREMINDERS_H
