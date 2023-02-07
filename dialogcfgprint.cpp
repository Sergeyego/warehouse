#include "dialogcfgprint.h"
#include "ui_dialogcfgprint.h"

DialogCfgPrint::DialogCfgPrint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCfgPrint)
{
    ui->setupUi(this);

    actionAddSocket = new QAction("socket://",this);
    actionAddIpp = new QAction("ipp://",this);

    modelPrint = new ModelPrint(this);
    ui->tableView->setModel(modelPrint);
    ui->tableView->setColumnWidth(0,120);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setColumnWidth(2,70);
    ui->tableView->setColumnWidth(3,70);

    ui->toolButtonNewPrint->addAction(actionAddSocket);
    ui->toolButtonNewPrint->addAction(actionAddIpp);

    connect(actionAddIpp,SIGNAL(triggered(bool)),this,SLOT(addPrint()));
    connect(actionAddSocket,SIGNAL(triggered(bool)),this,SLOT(addPrint()));

    connect(this,SIGNAL(accepted()),modelPrint,SLOT(save()));
    connect(ui->tableView,SIGNAL(sigRemove(int)),this,SLOT(removePrint(int)));
}

DialogCfgPrint::~DialogCfgPrint()
{
    delete ui;
}

void DialogCfgPrint::removePrint(int row)
{
    modelPrint->removeRow(row,QModelIndex());
}

void DialogCfgPrint::addPrint()
{
    QString adr="socket://192.168.1.2:9100";
    if (sender()==actionAddIpp){
        adr="http://localhost:631/printers/new_Printer";
    }
    modelPrint->newPrint(adr);
}
