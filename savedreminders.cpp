#include "savedreminders.h"
#include "ui_savedreminders.h"
#include <QTextBrowser>
#include <QDebug>
#include <QMessageBox>


QString handleDaysIdxs(QList<int> dayIdxs){
    QStringList str;
    for(auto idx: dayIdxs){
        switch (idx) {
            case 1: str.append("Пн"); break;
            case 2: str.append("Вт"); break;
            case 3: str.append("Ср"); break;
            case 4: str.append("Чт"); break;
            case 5: str.append("Пт"); break;
            case 6: str.append("Сб"); break;
            case 7: str.append("Вс"); break;
        }
    }
    return str.join(" ");
}

SavedRemindersDialog::SavedRemindersDialog(QWidget *parent, QList<ScheduleItem*> *remindersKeeper) :
    QDialog(parent),
    ui(new Ui::SavedRemindersDialog)
{
    m_remindersKeeper = remindersKeeper;
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    move(500,400);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    ui->closeBtn->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(ui->closeBtn, &QPushButton::clicked, this, &SavedRemindersDialog::close);

    ui->rmBtn->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(ui->rmBtn, &QPushButton::clicked, this, &SavedRemindersDialog::deleteReminders);

    placeRemindersInTable();
}

SavedRemindersDialog::~SavedRemindersDialog()
{
    delete ui;
}

void SavedRemindersDialog::placeRemindersInTable(){
    ui->tableWidget->clear();

    if (ui->tableWidget->rowCount() > 0){
        int currentRow = 0;
        for (auto reminder: *m_remindersKeeper) {
            fillRow(reminder, currentRow);
            currentRow++;
        }
    } else {
        for (auto reminder: *m_remindersKeeper) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            fillRow(reminder, ui->tableWidget->rowCount() - 1);
        }
    }
}

void SavedRemindersDialog::fillRow(ScheduleItem *reminder, int rowNumber){
    if (reminder->getWDaysFlag() == true){
        ui->tableWidget->setItem(rowNumber, 0,
                          new QTableWidgetItem(QString("%1\nВремя: %2")
                      .arg(handleDaysIdxs(reminder->getWDaysMap()->keys()),
                           reminder->getWDaysTime().toString())));
    }
    else
        ui->tableWidget->setItem(rowNumber, 0,
                                 new QTableWidgetItem(QString("%1\nВремя: %2")
                           .arg(reminder->getDate().date().toString("dd.MM.yyyy"),
                                reminder->getDate().time().toString())));

   // в среднюю колонку вставляем QTextBrowser и размещаем в нем сообщение
    if (reminder->getMsg().length() > 40){
        QTextBrowser *browser = new QTextBrowser;
        browser->setText(reminder->getMsg());
        ui->tableWidget->setCellWidget(rowNumber, 1, browser);
    }
    else
        ui->tableWidget->setItem(rowNumber, 1,
                                       new QTableWidgetItem(reminder->getMsg()));

// добавим QCheckBox для каждого reminder (отмечать для удаления)
    reminder->m_chBox = new QCheckBox();

    QWidget *checkBoxWidget = new QWidget(this);   // виджет и слой для размещения по центру
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget);
    layoutCheckBox->addWidget(reminder->m_chBox);
    layoutCheckBox->setAlignment(Qt::AlignCenter);
    layoutCheckBox->setContentsMargins(0,0,0,0);
    ui->tableWidget->setCellWidget(rowNumber, 2, checkBoxWidget);
}

void SavedRemindersDialog::deleteReminders(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Удалить", "Точно хотите удалить выбранные напоминания?",
                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        for(auto reminder: *m_remindersKeeper) {
            if (reminder->chBoxisChecked()){
                delete reminder->m_chBox;
                delete reminder;
                m_remindersKeeper->removeOne(reminder);
            }
        }
        writeDataToBin(*m_remindersKeeper); // обновить список сохраненных напоминаний
        placeRemindersInTable();
    }
}
