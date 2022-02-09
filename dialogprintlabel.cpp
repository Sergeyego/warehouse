#include "dialogprintlabel.h"
#include "ui_dialogprintlabel.h"

DialogPrintLabel::DialogPrintLabel(LabelBase *l, QWidget *parent) :
    QDialog(parent), label(l),
    ui(new Ui::DialogPrintLabel)
{
    ui->setupUi(this);

    modelPrint = new ModelPrint(this);
    ui->comboBoxPrint->setModel(modelPrint);
    ui->comboBoxPrint->setModelColumn(0);

    ui->doubleSpinBoxWidth->setValue(label->getWidth());
    ui->doubleSpinBoxHeidth->setValue(label->getHeight());
    ui->doubleSpinBoxGap->setValue(label->getGap());

    connect(this,SIGNAL(accepted()),this,SLOT(print()));

}

DialogPrintLabel::~DialogPrintLabel()
{
    delete ui;
}

QUrl DialogPrintLabel::currentUrl()
{
    return QUrl(modelPrint->data(modelPrint->index(ui->comboBoxPrint->currentIndex(),1),Qt::EditRole).toString());
}

int DialogPrintLabel::currentDpi()
{
    return modelPrint->data(modelPrint->index(ui->comboBoxPrint->currentIndex(),2),Qt::EditRole).toInt();
}

void DialogPrintLabel::print()
{
    label->setDpi(currentDpi());
    QByteArray data;
    data.append(label->getCod());
    data.append(label->print(ui->spinBox->value()));
    TPrinter::printData(currentUrl(),data);
}
