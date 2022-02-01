#include "dialogolapflt.h"
#include "ui_dialogolapflt.h"

DialogOlapFlt::DialogOlapFlt(ProxyDataModel *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOlapFlt)
{
    ui->setupUi(this);
    proxyModel = m;
    for (int j=0; j<proxyModel->columnCount(); j++){
        QListWidget *w = new QListWidget(this);
        QStringList itemList = proxyModel->getSourceVal(j);
        QStringList selItem = proxyModel->getSelectVal(j);
        for (QString it : itemList){
            QListWidgetItem *item = new QListWidgetItem(it,w);
            item->setData(Qt::CheckStateRole,selItem.contains(it) ? Qt::Checked : Qt::Unchecked);
        }
        ui->tabWidget->addTab(w,proxyModel->headerData(j,Qt::Horizontal).toString());
    }
}

DialogOlapFlt::~DialogOlapFlt()
{
    delete ui;
}
