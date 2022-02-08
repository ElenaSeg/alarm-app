#include "schedule_item.h"
#include "reminderwindow.h"
#include <QCoreApplication>
#include <QDir>

void ScheduleItem::setWeekDaysFlag(bool hasWeekDays){
    m_flagWeekDays = hasWeekDays;
    if (m_flagWeekDays == true) // если заданы дни недели, то выделяем место для map (хранит индекс дня недели и флаг)
        m_weekDays = new (QMap<int, bool>);
}

void ScheduleItem::setFlagToRemove() {
    if (m_flagWeekDays){
        if(checkIfReminderWorked())
            m_toRemove = true;
    }
    else
        m_toRemove = true;
}
bool ScheduleItem::checkTimeToRemind(QDateTime now){ // проверка на совпадение текущего времени и времени напоминания
      if (m_flagWeekDays){
        if (m_weekDays->keys().contains(now.date().dayOfWeek())
            && m_wDaysTime.hour() == now.time().hour() && m_wDaysTime.minute() == now.time().minute())
        {
            if (checkIfCurrentReminderWorked(now.date().dayOfWeek())) // если напоминание уже отработало
                return false;
            else{
                changeReminderState(now);
                return true;
            }
        }
      }
      else{
          if(m_date.date() == now.date() && m_date.time().hour() == now.time().hour()
                  && m_date.time().minute() == now.time().minute())
              return true;
      }
      return false;
}

bool ScheduleItem::checkIfReminderWorked(){
    if(!getWDaysFlag()){  // если напоминание по дате уже сработало, то установлен FlagToRemove, метод вернет true
        return this->getFlagToRemove();
    }
    else
        foreach(bool reminderWorked, m_weekDays->values()){ // если напоминание по дням недели,
            if (reminderWorked == false)                   // то нужно перебрать все флаги
                return false;
        }
    return true;
}

void ScheduleItem::openReminderWindow(){
        ReminderWindow *rw = new ReminderWindow(this);
        rw->show();
}

void ScheduleItem::changeReminderState(QDateTime now){
    m_weekDays->remove(now.date().dayOfWeek());
    m_weekDays->insert(now.date().dayOfWeek(), true);
}

void ScheduleItem::serialize(QDataStream &stream){
    if (getWDaysFlag()){
        stream << getWDaysFlag() << getMsg() << *getWDaysMap() << getWDaysTime().toString();
    }
    else{
        stream << getWDaysFlag() << getMsg()  << getDate().toString();
    }
}

void ScheduleItem::deserialize(QDataStream &stream){
    QString dateTimeStr;

    stream >> m_flagWeekDays;
    if (getWDaysFlag())
    {
        stream >> m_message;
        m_weekDays = new(QMap<int, bool>);
        stream >> *m_weekDays;
        stream >> dateTimeStr;
        m_wDaysTime = QTime::fromString(dateTimeStr, "hh:mm:ss");
    }
    else{
        stream >> m_message;
        stream >> dateTimeStr;
        m_date= QDateTime::fromString(dateTimeStr, "ddd MMM dd hh:mm:ss yyyy");
    }
}

const QString assignFileName(){
    return (QCoreApplication::arguments().size() < 2)
                                     ? QString("%1/settings.bin").arg(QCoreApplication::applicationDirPath())
                                     : QCoreApplication::arguments().at(1);
}

void writeDataToBin(QList<ScheduleItem*> listOfReminders){
    const QString FILE_NAME = assignFileName();
    QFile out( FILE_NAME );

    if( out.open(QIODevice::WriteOnly) ) {
        QDataStream stream( &out );
        for(auto item: listOfReminders){
            item->serialize(stream);
        }
        out.close();
    }
    else
           qDebug() << "error = " << out.error ();
}

void readDataFromBin(QList<ScheduleItem*>& remindersKeeper){
    const QString FILE_NAME = assignFileName();
    QFile in( FILE_NAME );

    if( in.open(QIODevice::ReadOnly) ) {
        QDataStream stream( &in );
        while(in.bytesAvailable() > 0)
        {
            ScheduleItem* item = new ScheduleItem();
            item->deserialize(stream);
            remindersKeeper.append(item);
        }
        in.close();
    }
}
