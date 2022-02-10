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

void DialogCmd::goCmd()
{
    QByteArray text = ui->plainTextEdit->toPlainText().toUtf8();
    TPrinter::printData(printerUrl,text);
}
