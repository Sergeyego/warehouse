#include "formdrv.h"
#include "ui_formdrv.h"

FormDrv::FormDrv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDrv)
{
    ui->setupUi(this);

    modelDrv = new DbTableModel("drvs",this);
    modelDrv->addColumn("id",tr("id"));
    modelDrv->addColumn("surname",tr("Фамилия"));
    modelDrv->addColumn("name",tr("Имя"));
    modelDrv->addColumn("middle_name",tr("Отчество"));
    modelDrv->addColumn("drv_license",tr("ВУ"));
    modelDrv->addColumn("car",tr("Машина"));
    modelDrv->addColumn("car_num",tr("Номер машины"));
    modelDrv->setSort("surname, name, middle_name");
    modelDrv->select();

    ui->tableView->setModel(modelDrv);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,120);
    ui->tableView->setColumnWidth(2,120);
    ui->tableView->setColumnWidth(3,120);
    ui->tableView->setColumnHidden(4,true);
    ui->tableView->setColumnHidden(5,true);
    ui->tableView->setColumnWidth(6,110);

    mapper = new DbMapper(ui->tableView,this);

    mapper->addMapping(ui->lineEditSur,1);
    mapper->addMapping(ui->lineEditNam,2);
    mapper->addMapping(ui->lineEditMid,3);
    mapper->addMapping(ui->lineEditLic,4);
    mapper->addMapping(ui->lineEditCar,5);
    mapper->addMapping(ui->lineEditCarNum,6);

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    mapper->last();

    connect(modelDrv,SIGNAL(sigUpd()),Models::instance()->relDrv,SLOT(refreshModel()));
}

FormDrv::~FormDrv()
{
    delete ui;
}
