#include "formlabelsel.h"
#include "ui_formlabelsel.h"

FormLabelsEl::FormLabelsEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLabelsEl)
{
    ui->setupUi(this);

    data=new FormDataEl(this);
    ui->groupBox->layout()->addWidget(data);

    labelE5040 = new LabelE5040("e5040",50,40,2.5,data,this);
    labelG5040 = new LabelG5040("g5040",50,40,2.5,data,this);

    connect(ui->pushButtonEd5040,SIGNAL(clicked(bool)),labelE5040,SLOT(printLabel()));
    connect(ui->pushButtonGr5040,SIGNAL(clicked(bool)),labelG5040,SLOT(printLabel()));
}

FormLabelsEl::~FormLabelsEl()
{
    delete ui;
}