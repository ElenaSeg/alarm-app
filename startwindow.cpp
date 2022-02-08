#include "startwindow.h"
#include "ui_startwindow.h"
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QLabel>
#include <QDir>
#include "myqtimer.h"

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    move(800,400);
    handleTrayIcon();

    m_alarm.readSettings();
    m_alarm.initTimer();

    connect(ui->listOfReminders, &QAction::triggered, &m_alarm,
         &Alarm::ShowListOfReminders);
    connect(ui->addAlarm, &QAction::triggered, &m_alarm,
         &Alarm::show);

    fillTableWithTime();
    keepUpToDateTime();
    qDebug() << QDir::currentPath();

}

StartWindow::~StartWindow(){
    delete ui;
    delete m_trayIcon;
    delete m_trayIconMenu;
    delete m_timer;
}

void StartWindow::handleTrayIcon(){
    m_trayIcon=new QSystemTrayIcon(this);
    m_trayIconMenu=new QMenu(this);
    QAction *QAshow=new QAction("&Свернуть/развернуть окно",this);
    QAction *QAquit=new QAction("&Выйти",this);

    m_trayIconMenu->addAction(QAshow);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(QAquit);
    m_trayIcon->setContextMenu(m_trayIconMenu);

    this->m_trayIcon->setIcon(QIcon(":/Icons/Clock.png"));

    m_trayIcon->setToolTip("Alarm");
    m_trayIcon->show();
    connect(QAquit,&QAction::triggered,this, &StartWindow::saveInfoAndQuit);
    connect(QAshow,SIGNAL(triggered()),this,SLOT(ToggleWindow()));
    connect(m_trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(ToggleWindow(QSystemTrayIcon::ActivationReason)));
}

void StartWindow::ToggleWindow(QSystemTrayIcon::ActivationReason Reason){
    if(Reason==QSystemTrayIcon::DoubleClick || Reason==QSystemTrayIcon::Trigger)
        ToggleWindow();
}

void StartWindow::ToggleWindow(){
    if(this->isHidden())
    {
        this->show();
    }else{
        this->hide();
    }
}

bool StartWindow::readXml(){
    QFile file(":/cities.xml");
    QString cityName;
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QXmlStreamReader xml(file.readAll());
    file.close();
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "cities"){
                continue;
            }
            if (xml.name() == "city"){
                QXmlStreamAttributes attr = xml.attributes();
                if (attr.hasAttribute("city_name"))
                    cityName = attr.value("city_name").toString();
                xml.readNextStartElement();
                QString UTCarg = xml.readElementText();
                m_cityToUTCtimeArg.insert(cityName, UTCarg.toInt());
            }
        }
    }
    if (m_cityToUTCtimeArg.isEmpty())
        return false;
    return true;
}

void StartWindow::fillTableWithTime(){
    if (!readXml()){
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        fillRow(0, "Москва", 3);
    }
    else{
        foreach (QString cityName, m_cityToUTCtimeArg.keys()) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            fillRow(ui->tableWidget->rowCount() - 1,
                    cityName, m_cityToUTCtimeArg.value(cityName));
        }
    }
}

void StartWindow::fillRow(int rowNumber, QString cityName, int argUTC){
    ui->tableWidget->setItem(rowNumber, 0, new QTableWidgetItem(cityName));
//    ui->tableWidget->itemAt(rowNumber, 0)->setTextAlignment(Qt::AlignCenter);
    QLabel *lbl= new QLabel(this);
    lbl->setText(QString("%1 \nUTC %2")
                 .arg(QDateTime::currentDateTimeUtc()
                      .addSecs(argUTC * 3600)
                      .toString("hh:mm"))
                 .arg(argUTC));
    lbl->setAlignment(Qt::AlignCenter);
    ui->tableWidget->setCellWidget(rowNumber, 1, lbl);
    m_cityNameToTimeLabel.insert(cityName, lbl);
}

void StartWindow::keepUpToDateTime() {    // подает сигнал на изменение времени в таблице
    m_timer = new MyQTimer(this);
    QObject::connect(m_timer, &MyQTimer::timeout, this, &StartWindow::updateTime);
    m_timer->start(1000);
}

void StartWindow::updateTime(){
    m_timer->stop();
    int UTCarg;
    foreach(QString city, m_cityToUTCtimeArg.keys()){
        UTCarg = m_cityToUTCtimeArg.value(city);
        m_cityNameToTimeLabel.value(city)->setText(QString("%1 \nUTC%2%3")
                 .arg(QDateTime::currentDateTimeUtc()
                                .addSecs(UTCarg * 3600)
                                .toString("hh:mm"))
                 .arg(UTCarg >= 0 ? " +" : " ")
                 .arg(UTCarg));
    }
    m_timer->start();
}

void StartWindow::closeEvent(QCloseEvent * event){
    if(QSystemTrayIcon::isSystemTrayAvailable()){
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
                                                    QSystemTrayIcon::Information);
        m_trayIcon->showMessage("Tray Program",
                              trUtf8("Приложение свернуто в трей. Чтобы "
                                     "развернуть окно, щелкните по иконке приложения в трее"),
                              icon, 2000);
    }else{
        event->accept();
    }
}

void StartWindow::saveInfoAndQuit(){
    QCoreApplication::quit();
}
