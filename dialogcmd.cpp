#include "dialogcmd.h"
#include "ui_dialogcmd.h"

DialogCmd::DialogCmd(QString &cmd, QUrl printer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCmd),
    printerUrl(printer)
{
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(cmd);
    connect(ui->pushButtonGo,SIGNAL(clicked(bool)),this,SLOT(goCmd()));
}

DialogCmd::~DialogCmd()
{
    delete ui;
}

void DialogCmd::setPrintCmd(QString cmd)
{
    ui->plainTextEditPrint->setPlainText(cmd);
}

void DialogCmd::setImageCmd(const QByteArray &cmd)
{
    imageCmd=cmd;
    ui->checkBoxImage->setEnabled(!cmd.isEmpty());
}

void DialogCmd::goCmd()
{
    QByteArray text = ui->plainTextEdit->toPlainText().toUtf8();
    if (ui->checkBoxImage->isChecked()){
        text.append(imageCmd);
    }
    text.append(ui->plainTextEditPrint->toPlainText().toUtf8());
    TPrinter::printData(printerUrl,text);
}
