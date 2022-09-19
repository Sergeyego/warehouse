#include "dialogrest.h"
#include "ui_dialogrest.h"

DialogRest::DialogRest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRest)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()));
    QSqlQuery query;
    query.prepare("select max(dat) from ostat");
    if (query.exec()){
        while (query.next()){
            ui->labelDate->setText(query.value(0).toDate().addDays(-1).toString("dd.MM.yy"));
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

DialogRest::~DialogRest()
{
    delete ui;
}

QDate DialogRest::getDate()
{
    return ui->dateEdit->date();
}

void DialogRest::accept()
{
    if (ui->dateEdit->date().month()!=ui->dateEdit->date().addDays(1).month()){
        return QDialog::accept();
    } else {
        QMessageBox::information(this,tr("Предупреждение"),tr("Дата должна быть последним числом месяца."),QMessageBox::Ok);
    }
}
