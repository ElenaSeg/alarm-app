QT += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    alarm.cpp \
    reminderwindow.cpp \
    savedreminders.cpp \
    startwindow.cpp \
    schedule_item.cpp

HEADERS += \
    alarm.h \
    reminderwindow.h \
    savedreminders.h \
    schedule_item.h \
    myqtimer.h \
    startwindow.h

FORMS += \
    alarm.ui \
    reminderwindow.ui \
    savedreminders.ui \
    startwindow.ui

RESOURCES += \
    Resourses.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
