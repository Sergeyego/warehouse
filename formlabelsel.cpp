#include "formlabelsel.h"
#include "ui_formlabelsel.h"

FormLabelsEl::FormLabelsEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLabelsEl)
{
    ui->setupUi(this);

    data=new FormDataEl(this);
    ui->groupBox->layout()->addWidget(data);

    labelE60150 = new LabelE60150("Этикетка_60*150_пачка",60,150,2,data,this);
    labelG60150 = new LabelG60150("Этикетка_60*150_гофрокороб",60,150,2,data,this);

    connect(ui->pushButtonEd60150,SIGNAL(clicked(bool)),this,SLOT(printE60150()));
    connect(ui->pushButtonGr60150,SIGNAL(clicked(bool)),this,SLOT(printG60150()));

    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(printPackList()));
}

FormLabelsEl::~FormLabelsEl()
{
    delete ui;
}

void FormLabelsEl::printPackList()
{
    if (!data->masPal().isEmpty()){
        QByteArray packer=data->packer().toUtf8();
        QString p64url = packer.toBase64().replace("+","-").replace("/","_");
        QString pallet=Models::instance()->createPalBarcode("E");
        DialogWebView d;
        if (d.sendGetReq("packlists/old/e/"+pallet+"/"+QString::number(data->getIdPart())+"/"+data->masPal()+"?packer="+p64url+"&dat_pack="+data->datePack()+"&pack_kvo="+data->kvoPackPal())){
            d.setSingle(true);
            d.exec();
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),tr("Нет массы поддона"),QMessageBox::Cancel);
    }
}

void FormLabelsEl::printE60150()
{
    if (data->check()){
        labelE60150->printLabel();
    }
}

void FormLabelsEl::printG60150()
{
    if (data->check()){
        labelG60150->printLabel();
    }
}
