#include "formlabelsel.h"
#include "ui_formlabelsel.h"

FormLabelsEl::FormLabelsEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLabelsEl)
{
    ui->setupUi(this);

    data=new FormDataEl(this);
    ui->groupBox->layout()->addWidget(data);

    labelE5040 = new LabelE5040("Этикетка_50*40_пачка",50,40,2.5,data,this);
    labelG5040 = new LabelG5040("Этикетка_50*40_гофрокороб",50,40,2.5,data,this);

    labelE60150 = new LabelE60150("Этикетка_60*150_пачка",60,150,2.5,data,this);
    labelG60150 = new LabelG60150("Этикетка_60*150_гофрокороб",60,150,2.5,data,this);

    connect(ui->pushButtonEd5040,SIGNAL(clicked(bool)),labelE5040,SLOT(printLabel()));
    connect(ui->pushButtonGr5040,SIGNAL(clicked(bool)),labelG5040,SLOT(printLabel()));

    connect(ui->pushButtonEd60150,SIGNAL(clicked(bool)),labelE60150,SLOT(printLabel()));
    connect(ui->pushButtonGr60150,SIGNAL(clicked(bool)),labelG60150,SLOT(printLabel()));

    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(printPackList()));
}

FormLabelsEl::~FormLabelsEl()
{
    delete ui;
}

void FormLabelsEl::printPackList()
{
    PackElDoc doc(data);
    DialogPrintPackList d(&doc);
    d.exec();
}
