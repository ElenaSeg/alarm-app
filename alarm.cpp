#include "alarm.h"
#include "ui_alarm.h"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include "savedreminders.h"
#include "reminderwindow.h"

Alarm::Alarm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Alarm)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->saveBtn->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(ui->saveBtn, &QPushButton::clicked, this, &Alarm::save_slot);

    m_dayChBoxes.insert(ui->chkMon, 1);
    m_dayChBoxes.insert(ui->chkTues, 2);
    m_dayChBoxes.insert(ui->chkWed, 3);
    m_dayChBoxes.insert(ui->chkThurs, 4);
    m_dayChBoxes.insert(ui->chkFri, 5);
    m_dayChBoxes.insert(ui->chkSat, 6);
    m_dayChBoxes.insert(ui->chkSun, 7);

    connect(ui->chkDate, &QCheckBox::clicked, this, &Alarm::handleDateChBoxes);
    foreach (QCheckBox* checkBox, m_dayChBoxes.keys()) {
         connect(checkBox, &QCheckBox::clicked, this, &Alarm::handleDateChBoxes);
    }

    move(500,400);
}

Alarm::~Alarm()
{
    delete ui;
}

void Alarm::handleDateChBoxes(){
    if (ui->chkDate->isChecked()){
        m_flagWeekDays = false;
        ui->dateEdit->setEnabled(true);

        foreach (QCheckBox* checkBox, m_dayChBoxes.keys()) {
            checkBox->setEnabled(false);
        }
    }
    else if (!ui->chkDate->isChecked()){
        ui->dateEdit->setEnabled(false);

        foreach (QCheckBox* checkBox, m_dayChBoxes.keys()) {
            checkBox->setEnabled(true);
        }
        m_flagWeekDays = false;
        foreach (QCheckBox* checkBox, m_dayChBoxes.keys()) {
            if (checkBox->isChecked()){
                m_flagWeekDays = true;
                break;
            }
        }
    }
}

void Alarm::initTimer(){
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()), this, SLOT(timeCheck()));
    m_timer->start(1000);
}

void Alarm::save_slot(){
        if (m_flagWeekDays == true){
            saveWithWeekDays(/*ui->repeatEveryWeek->isChecked()*/);
            writeDataToBin(m_remindersKeeper);
            return ;
        }
        else if(!ui->chkDate->isChecked() && (!m_flagWeekDays)){
            QMessageBox::warning(this,"Не выбран день напоминания",
                             "Выберите день недели или дату напоминания", "Oк");
            return;
        }
        QDateTime currAlarmDate;
        currAlarmDate.setDate(ui->dateEdit->date());
        currAlarmDate.setTime(ui->timeEdit->time());
        if (checkDate(currAlarmDate)) { // вернет false если дата напоминания уже прошла
            saveWithDate(currAlarmDate);
            writeDataToBin(m_remindersKeeper);
        }
        else
            QMessageBox::warning(this,"Время напоминания  уже прошло",
                             "Измените время или дату напоминания \n"
                             "на более позднее", "Oк");
}

void Alarm::saveWithWeekDays(/*bool repeatEveryWeek*/){
//    if (repeatEveryWeek){
    QMap<int, bool> currAlarmWeekDays; // key - номер дня недели (с 1 по 7), value - флаг(отработало напоминание или нет. по умолчанию - false);
    foreach (QCheckBox* checkBox, m_dayChBoxes.keys()) {
        if (checkBox->isChecked())
            currAlarmWeekDays.insert(m_dayChBoxes.value(checkBox), false);
    }

    ScheduleItem *scheduleItem = new ScheduleItem(ui->timeEdit->time(),
                                                  ui->msgEdit->toPlainText());
    scheduleItem->setWeekDaysFlag(true);
    scheduleItem->setWeekDaysMap(currAlarmWeekDays);
    m_remindersKeeper.append(scheduleItem);

    ui->statusbar->setStyleSheet("color : green");
    ui->statusbar->showMessage("Сохранено", 2000);
}

void Alarm::timeCheck(){
    m_timer->stop();
    QDateTime now = QDateTime::currentDateTime();

    for (auto curReminder : m_remindersKeeper){
        if (curReminder->checkTimeToRemind(now)) {
           curReminder->openReminderWindow();
           if (curReminder->checkIfReminderWorked())
               deletePassedReminder();
        }
    }
    m_timer->start();
}

bool Alarm::checkDate(QDateTime currAlarmDate){
    QDateTime now = QDateTime::currentDateTime();
    qint64 timeUntilRmndr = now.secsTo(currAlarmDate);
    if (timeUntilRmndr <= 0)
        return false;
    else
        return true;
}

void Alarm::saveWithDate(QDateTime currAlarmDate){
    ScheduleItem *item = new ScheduleItem(currAlarmDate, ui->msgEdit->toPlainText());
    m_remindersKeeper.append(item);

    ui->statusbar->setStyleSheet("color : green");
    ui->statusbar->showMessage("Сохранено", 2000);
}

void Alarm::ShowListOfReminders(){
    if (m_remindersKeeper.isEmpty())
        QMessageBox::warning(this,"Список напоминаний пуст",
                         "Нет созданных напоминаний", "Oк");
    else {
        SavedRemindersDialog *savedRemsDlg = new SavedRemindersDialog(this, &m_remindersKeeper);
        savedRemsDlg->show();
    }
}

void Alarm::deletePassedReminder(){ // удалить уже прошедшие напоминания из списка
    QListIterator<ScheduleItem*> it(m_remindersKeeper);
    while(it.hasNext()){
        ScheduleItem* reminder = it.next();
        if(reminder->getFlagToRemove()){
            delete reminder;
            m_remindersKeeper.removeOne(reminder);
        }
    }
    writeDataToBin(m_remindersKeeper);
}

void Alarm::readSettings(){
    readDataFromBin(m_remindersKeeper);
}

void Alarm::closeEvent(QCloseEvent * event){
    event->ignore();
    this->hide();
}
