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

    connect(ui->pushButton80_1016,SIGNAL(clicked(bool)),labelE801016,SLOT(printLabel()));
    connect(ui->pushButton95_110,SIGNAL(clicked(bool)),labelG95110,SLOT(printLabel()));
    connect(ui->pushButton45_70,SIGNAL(clicked(bool)),labelE4570,SLOT(printLabel()));
    connect(ui->pushButton100_100,SIGNAL(clicked(bool)),labelG100100,SLOT(printLabel()));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),labelG100100Pal,SLOT(printLabel()));

    connect(ui->pushButtonPackListA5,SIGNAL(clicked(bool)),this,SLOT(printPackListA5()));
}

FormLabelsWire::~FormLabelsWire()
{
    delete ui;
}

void FormLabelsWire::printPackListA5()
{
    /*PackWireDoc doc(data);
    DialogPrintPackList d(&doc);
    d.exec();*/
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
