#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QLabel>
#include "alarm.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

signals:
    void closeApp();

private slots:
    void ToggleWindow();
    void ToggleWindow(QSystemTrayIcon::ActivationReason);

private:
    Ui::StartWindow *ui;
    Alarm m_alarm;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;
    MyQTimer *m_timer = nullptr;
    QMap<QString, int> m_cityToUTCtimeArg;
    QMap<QString, QLabel*> m_cityNameToTimeLabel;

    void closeEvent(QCloseEvent *);
    void handleTrayIcon();
    void saveInfoAndQuit();
    bool readXml();
    void fillTableWithTime();
    void fillRow(int rowNumber, QString cityName, int argUTC);
    void keepUpToDateTime();
    void updateTime();
};

#endif // STARTWINDOW_H
