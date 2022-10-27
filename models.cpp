#include "models.h"

Models* Models::models_instance=0;

Models::Models(QObject *parent) :
    QObject(parent)
{
    sync1C = new Sync1C(this);
    invoiceManager = new InvoiceManager(this);

    relPol = new DbSqlRelation("pol_view","id","nam",this);
    relDrv = new DbSqlRelation("drv_view","id","drv",this);

    relAccTypeEl = new DbSqlRelation("prod_nakl_tip","id","nam",this);
    relAccTypeEl->setFilter("prod_nakl_tip.en=true");

    relAccTypeWire = new DbSqlRelation("wire_way_bill_type","id","nam",this);
    relAccTypeWire->setFilter("wire_way_bill_type.en=true");

    relKis = new DbSqlRelation("kis_view","kis","mark",this);
    relKis->setFilterColumn("typ");
    relKis->model()->setAsync(false);

    relWirePart = new DbSqlRelation("wire_parti","id","str",this);
    relWirePart->setSort("str desc");
    relWirePart->setFilter("wire_parti.id<>0");
    relWirePart->setFilterColumn("ids");
    relWirePart->model()->setLimit(4000);

    relElPart = new DbSqlRelation("parti","id","str",this);
    relElPart->setSort("parti.dat_part desc, parti.n_s desc");
    relElPart->setFilter("parti.id<>0");
    relElPart->setFilterColumn("ids");
    relElPart->model()->setLimit(4000);

}

Models *Models::instance()
{
    if (models_instance == 0)
        models_instance = new Models;
    return models_instance;
}

QString Models::createPalBarcode(QString prefix)
{
    QString palBarcode="";
    QSqlQuery query;
    query.prepare("insert into pallets (datetime, prefix) values (:datetime, :prefix) returning id");
    query.bindValue(":datetime",QDateTime::currentDateTime());
    query.bindValue(":prefix",prefix);
    if (query.exec()){
        if (query.next()){
            palBarcode=prefix+QString("%1").arg((query.value(0).toInt()),10-prefix.length(),'d',0,QChar('0'));
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return palBarcode;
}

