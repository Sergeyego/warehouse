#include "formcells.h"
#include "ui_formcells.h"

FormCells::FormCells(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCells)
{
    ui->setupUi(this);
}

FormCells::~FormCells()
{
    delete ui;
}
