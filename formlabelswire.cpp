#include "formlabelswire.h"
#include "ui_formlabelswire.h"

FormLabelsWire::FormLabelsWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLabelsWire)
{
    ui->setupUi(this);
    data=new FormDataWire(this);
    ui->groupBox->layout()->addWidget(data);

    labelE801016 = new LabelE801016(QString::fromUtf8("Этикетка_80*101.6"),80,101.6,4,data,this);
    labelG95110 = new LabelG95110(QString::fromUtf8("Этикетка_95*110"),95,110,4,data,this);
    labelE4570 = new LabelE4570(QString::fromUtf8("Этикетка_45*70_упак_пров"),45,70,2,data,this);
    labelG100100 = new LabelG100100(QString::fromUtf8("Этикетка_100*100"),100,100,2.8,data,this);
    labelG100100Pal = new LabelG100100Pal(QString::fromUtf8("Упаковочный_лист_100*100"),100,100,2.8,data,this);
    labelE7035 = new LabelE7035(QString::fromUtf8("Этикетка_70*35"),70,35,3,data,this);
    labelWG60150 = new LabelWG60150("Этикетка_60*150",60,150,2,data,this);

    connect(ui->pushButton80_1016,SIGNAL(clicked(bool)),this,SLOT(printE801016()));
    connect(ui->pushButton95_110,SIGNAL(clicked(bool)),this,SLOT(printG95110()));
    connect(ui->pushButton45_70,SIGNAL(clicked(bool)),this,SLOT(printE4570()));
    connect(ui->pushButton70_35,SIGNAL(clicked(bool)),this,SLOT(printE7035()));
    connect(ui->pushButton100_100,SIGNAL(clicked(bool)),this,SLOT(printG100100()));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(printG100100Pal()));
    connect(ui->pushButton60_150,SIGNAL(clicked(bool)),this,SLOT(printW60150()));

    connect(ui->pushButtonPackListA5,SIGNAL(clicked(bool)),this,SLOT(printPackListA5()));
}

FormLabelsWire::~FormLabelsWire()
{
    delete ui;
}

void FormLabelsWire::printPackListA5()
{
    if (!data->masPal().isEmpty()){
        QByteArray packer=data->master().toUtf8();
        QString p64url = packer.toBase64().replace("+","-").replace("/","_");
        QString pallet=Models::instance()->createPalBarcode("W");
        DialogWebView d;
        if (d.sendGetReq("packlists/old/w/"+pallet+"/"+QString::number(data->getIdPart())+"/"+data->masPal()+"?packer="+p64url+"&dat_pack="+data->datePack()+"&pack_kvo="+data->kvoSpool())){
            d.setSingle(true);
            d.exec();
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),tr("Нет массы поддона"),QMessageBox::Cancel);
    }
}

void FormLabelsWire::printE801016()
{
    if (data->checkGroup()){
        labelE801016->printLabel();
    }
}

void FormLabelsWire::printG95110()
{
    if (data->checkGroup()){
        labelG95110->printLabel();
    }
}

void FormLabelsWire::printE4570()
{
    if (data->checkEd()){
        labelE4570->printLabel();
    }
}

void FormLabelsWire::printG100100()
{
    if (data->checkGroup()){
        labelG100100->printLabel();
    }
}

void FormLabelsWire::printG100100Pal()
{
    if (data->checkEd()){
        labelG100100Pal->printLabel();
    }
}

void FormLabelsWire::printE7035()
{
    if (data->checkEd(true)){
        labelE7035->printLabel();
    }
}

void FormLabelsWire::printW60150()
{
    if (data->checkGroup()){
        labelWG60150->printLabel();
    }
}
