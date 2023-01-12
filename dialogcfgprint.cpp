#include "dialogcfgprint.h"
#include "ui_dialogcfgprint.h"

DialogCfgPrint::DialogCfgPrint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCfgPrint)
{
    ui->setupUi(this);
    modelPrint = new ModelPrint(this);
    ui->tableView->setModel(modelPrint);
    ui->tableView->setColumnWidth(0,120);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setColumnWidth(2,70);
    ui->tableView->setColumnWidth(3,70);

    connect(this,SIGNAL(accepted()),modelPrint,SLOT(save()));
    connect(ui->pushButtonNewPrint,SIGNAL(clicked(bool)),modelPrint,SLOT(newPrint()));
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
