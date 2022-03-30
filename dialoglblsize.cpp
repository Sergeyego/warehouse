#include "dialoglblsize.h"
#include "ui_dialoglblsize.h"

DialogLblSize::DialogLblSize(double w, double h, double g, bool rot, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLblSize)
{
    ui->setupUi(this);
    ui->doubleSpinBoxWidth->setValue(w);
    ui->doubleSpinBoxHeight->setValue(h);
    ui->doubleSpinBoxGap->setValue(g);
    ui->checkBoxRotate->setChecked(rot);
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

bool DialogLblSize::getRotate()
{
    return ui->checkBoxRotate->isChecked();
}
