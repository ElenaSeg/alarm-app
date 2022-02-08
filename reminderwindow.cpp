#include "reminderwindow.h"
#include "ui_reminderwindow.h"
#include <QDebug>
#include <QtMultimedia/QMediaPlayer>
#include <QBoxLayout>
#include "myqtimer.h"

ReminderWindow::ReminderWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReminderWindow)
{
    ui->setupUi(this);
}

ReminderWindow::ReminderWindow(ScheduleItem *scheduleItem) :
    ui(new Ui::ReminderWindow)
{
    ui->setupUi(this);

    m_schItem = scheduleItem;
    setWindowTitle("Напоминание");
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->timePassedLbl->setText("С момента напоминания прошло: 0 мин.");
    ui->timePassedLbl->setStyleSheet("QLabel {background: rgb(209, 214, 222);"
                                     " border: 1px solid grey; font-size: 12px }");

    if (scheduleItem->getWDaysFlag())
        ui->labelRemindAt->setText(QString("Напомнить в %1")
                                   .arg(scheduleItem->getWDaysTime().toString("hh:mm")));
    else
        ui->labelRemindAt->setText(QString("Напомниe: %1 в %2")
                                   .arg(scheduleItem->getDate().date().toString("dd.MM.yyyy"))
                                   .arg(scheduleItem->getDate().time().toString("hh:mm")));
//    ui->labelRemindAt->setStyleSheet("QLabel {background: rgb(237, 239, 242)}");

    ui->textBrowser->setText(scheduleItem->getMsg());

    ui->okBtn->setShortcut(Qt::Key_Enter);
    connect(ui->okBtn, &QPushButton::clicked, this, &ReminderWindow::close);
}

ReminderWindow::~ReminderWindow()
{
    delete ui;
    delete m_timer;
}

void ReminderWindow::showEvent(QShowEvent * event){
    event->accept();
    if (m_timer == nullptr){
        handleTimePassedLabel();
        m_schItem->setFlagToRemove();
    }
    QMediaPlayer *player = new QMediaPlayer(this);

    player->setMedia(QUrl("qrc:/Sounds/alarm.ogg"));
    player->setVolume(20);
    player->play();
    connect(ui->offSoundBtn, &QPushButton::clicked, player, &QMediaPlayer::stop);
}

void ReminderWindow::handleTimePassedLabel() {
    m_timer = new MyQTimer(this);

    QObject::connect(m_timer, &MyQTimer::timeout, this, &ReminderWindow::showTimePassed);
    m_timer->start(60000);
}

void ReminderWindow::showTimePassed(){
    m_timer->stop();
    ++m_timePassedVar;
    if (m_timePassedVar < 60)
        ui->timePassedLbl->setText(QString("С момента напоминания прошлo: %1 мин.").arg(m_timePassedVar));
    else{
        ui->timePassedLbl->setText(QString("С момента напоминания прошло: %1 ч. %2 мин.")
                                 .arg(m_timePassedVar / 60).arg(m_timePassedVar % 60));
    }
    m_timer->start();
}
