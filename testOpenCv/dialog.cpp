#include "dialog.h"
#include "ui_dialog.h"
#include <QDateTime>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    close();
}

void Dialog::on_pushButton_2_clicked()
{
    QString str = QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");
    emit dateChange(str);
}
