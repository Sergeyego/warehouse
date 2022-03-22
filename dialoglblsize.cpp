#include "dialoglblsize.h"
#include "ui_dialoglblsize.h"

DialogLblSize::DialogLblSize(double w, double h, double g, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLblSize)
{
    ui->setupUi(this);
    ui->doubleSpinBoxWidth->setValue(w);
    ui->doubleSpinBoxHeight->setValue(h);
    ui->doubleSpinBoxGap->setValue(g);
}

DialogLblSize::~DialogLblSize()
{
    delete ui;
}

double DialogLblSize::getWidth()
{
    return ui->doubleSpinBoxWidth->value();
}

double DialogLblSize::getHeight()
{
    return ui->doubleSpinBoxHeight->value();
}

double DialogLblSize::getGap()
{
    return ui->doubleSpinBoxGap->value();
}
