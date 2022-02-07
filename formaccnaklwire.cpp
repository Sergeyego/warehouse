#include "formaccnaklwire.h"
#include "ui_formaccnaklwire.h"

FormAccNaklWire::FormAccNaklWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAccNaklWire)
{
    ui->setupUi(this);
}

FormAccNaklWire::~FormAccNaklWire()
{
    delete ui;
}
