#include "formpoluch.h"
#include "ui_formpoluch.h"
#include <QDebug>

FormPoluch::FormPoluch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormPoluch)
{
    ui->setupUi(this);

    modelPoluch = new DbTableModel("poluch",this);
    modelPoluch->addColumn(QString("id"),QString("id"));
    modelPoluch->addColumn(QString("short"),QString::fromUtf8("Крат. наименование"));
    modelPoluch->addColumn(QString("naim"),QString::fromUtf8("naim"));
    modelPoluch->addColumn(QString("adres"),QString::fromUtf8("adres"));
    modelPoluch->addColumn(QString("telef"),QString::fromUtf8("telef"));
    modelPoluch->addColumn(QString("innkpp"),QString::fromUtf8("innkpp"));
    modelPoluch->addColumn(QString("id_kat"),QString::fromUtf8("id_kat"),Models::instance()->relKat);
    modelPoluch->addColumn(QString("bank"),QString::fromUtf8("bank"));
    modelPoluch->addColumn(QString("city"),QString::fromUtf8("city"));
    modelPoluch->addColumn(QString("rs"),QString::fromUtf8("rs"));
    modelPoluch->addColumn(QString("ks"),QString::fromUtf8("ks"));
    modelPoluch->addColumn(QString("bik"),QString::fromUtf8("bik"));
    modelPoluch->addColumn(QString("okpo"),QString::fromUtf8("okpo"));
    modelPoluch->addColumn(QString("adres_egrul"),QString::fromUtf8("adres_egrul"));
    modelPoluch->addColumn(QString("naim_en"),QString::fromUtf8("naim"));

    modelPoluch->setSort("poluch.short");
    modelPoluch->select();

    ui->tableView->setModel(modelPoluch);
    for (int i=0; i<ui->tableView->model()->columnCount(); i++){
        if (i!=1){
            ui->tableView->setColumnHidden(i,true);
        }
    }
    ui->tableView->setColumnWidth(1,200);

    mapper = new DbMapper(ui->tableView,this);
    mapper->addMapping(ui->lineEditShort,1);
    mapper->addMapping(ui->lineEditNaim,2);
    mapper->addMapping(ui->lineEditAdres,3);
    mapper->addMapping(ui->lineEditTelef,4);
    mapper->addMapping(ui->lineEditInnKpp,5);
    mapper->addMapping(ui->comboBoxKat,6);
    mapper->addMapping(ui->lineEditBank,7);
    mapper->addMapping(ui->lineEditCity,8);
    mapper->addMapping(ui->lineEditRs,9);
    mapper->addMapping(ui->lineEditKs,10);
    mapper->addMapping(ui->lineEditBIK,11);
    mapper->addMapping(ui->lineEditOkpo,12);
    mapper->addMapping(ui->lineEditAdresEgrul,13);
    mapper->addMapping(ui->lineEditNaimEn,14);

    ui->horizontalLayoutMapper->insertWidget(0,mapper);

    connect(modelPoluch,SIGNAL(sigUpd()),Models::instance()->relPol,SLOT(refreshModel()));
}

FormPoluch::~FormPoluch()
{
    delete ui;
}

void FormPoluch::keyPressEvent(QKeyEvent *pe)
{
    if (pe->key()==Qt::Key_Insert) mapper->slotNew();
    //QDialog::keyPressEvent(pe);
}

