#include "dialogprintlabel.h"
#include "ui_dialogprintlabel.h"

DialogPrintLabel::DialogPrintLabel(LabelBase *l, QWidget *parent) :
    QDialog(parent), label(l),
    ui(new Ui::DialogPrintLabel)
{
    ui->setupUi(this);

    printCmdMode=false;

    modelPrint = new ModelPrint(this);
    ui->comboBoxPrint->setModel(modelPrint);
    ui->comboBoxPrint->setModelColumn(0);

    ui->doubleSpinBoxWidth->setValue(label->getWidth());
    ui->doubleSpinBoxHeidth->setValue(label->getHeight());
    ui->doubleSpinBoxGap->setValue(label->getGap());

    for (int i=0; i<modelPrint->rowCount(); i++){
        QString pnam=modelPrint->data(modelPrint->index(i,0),Qt::EditRole).toString();
        if (pnam==label->getPrinterName()){
            ui->comboBoxPrint->setCurrentIndex(i);
            break;
        }
    }
    changedPrinter();

    connect(this,SIGNAL(accepted()),this,SLOT(print()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(savePrinter()));
    connect(ui->pushButtonCal,SIGNAL(clicked(bool)),this,SLOT(calibrate()));
    connect(ui->comboBoxPrint,SIGNAL(currentIndexChanged(int)),this,SLOT(changedPrinter()));
    connect(ui->pushButtonCode,SIGNAL(clicked(bool)),this,SLOT(showCmd()));

}

DialogPrintLabel::~DialogPrintLabel()
{
    delete ui;
}

void DialogPrintLabel::setPrintCmdMode(bool b)
{
    printCmdMode = b;
    ui->spinBox->setDisabled(b);
}

void DialogPrintLabel::setCut(bool b)
{
    ui->checkBoxCut->setChecked(b);
}

QUrl DialogPrintLabel::currentUrl()
{
    return QUrl(modelPrint->data(modelPrint->index(ui->comboBoxPrint->currentIndex(),1),Qt::EditRole).toString());
}

int DialogPrintLabel::currentDpi()
{
    return modelPrint->data(modelPrint->index(ui->comboBoxPrint->currentIndex(),2),Qt::EditRole).toInt();
}

int DialogPrintLabel::currentDensity()
{
    return modelPrint->data(modelPrint->index(ui->comboBoxPrint->currentIndex(),3),Qt::EditRole).toInt();
}

QString DialogPrintLabel::currentCmd()
{
    QString sprint = printCmdMode ? "" : label->print(ui->spinBox->value());
    return label->cut(ui->checkBoxCut->isChecked())+label->getCod()+sprint;
}

void DialogPrintLabel::print()
{
    QByteArray data=currentCmd().toUtf8();
    TPrinter::printData(currentUrl(),data);
}

void DialogPrintLabel::savePrinter()
{
    label->setPrinterName(ui->comboBoxPrint->currentText());
    ui->pushButtonSave->setEnabled(false);
}

void DialogPrintLabel::changedPrinter()
{
    ui->pushButtonSave->setEnabled(label->getPrinterName()!=ui->comboBoxPrint->currentText());
    label->setDpi(currentDpi());
    label->setDensity(currentDensity());
}

void DialogPrintLabel::calibrate()
{
    QString d = label->cut(false)+label->calibrate();
    QByteArray data=d.toUtf8();
    TPrinter::printData(currentUrl(),data);
}

void DialogPrintLabel::showCmd()
{
    QString cmd=currentCmd();
    DialogCmd d(cmd,currentUrl());
    d.exec();
}
