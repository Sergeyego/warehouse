#include "models.h"

Models* Models::models_instance=0;

Models::Models(QObject *parent) :
    QObject(parent)
{
    sync1C = new Sync1C(this);
    invoiceManager = new InvoiceManager(this);

    relPol = new DbSqlRelation("poluch","id","str",this);
    relDrv = new DbSqlRelation("drv_view","id","drv",this);
    relKat = new DbSqlRelation("pol_kat","id","nam",this);

    relElrtr = new DbSqlRelation("elrtr","id","marka",this);
    relElrtr->setSort("marka");

    relDiam = new DbSqlRelation("diam","id","sdim",this);
    relDiam->setSort("diam");

    relProvol = new DbSqlRelation("provol","id","nam",this);
    relProvol->setSort("nam");

    relSpool = new DbSqlRelation("wire_pack_kind","id","short",this);
    relSpool->setSort("short");

    relWirePack = new DbSqlRelation("wire_pack","id","pack_ed",this);
    relWirePack->setSort("pack_ed");

    relReqCause = new DbSqlRelation("requests_adj_cause","id","nam",this);
    relReqCause->setSort("nam");

    relElPack = new DbSqlRelation("el_pack_view","id","nam",this);
    relElPack->setSort("nam");

    relVars = new DbSqlRelation("elrtr_vars","id","nam",this);
    relVars->setSort("id");

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

    relReq = new DbSqlRelation("requests","id","str",this);
    relReq->setSort("requests.dat desc, requests.num desc");
    relReq->model()->setLimit(4000);

    relCex = new DbSqlRelation("cex","id","nam",this);
    relCex->setSort("cex.nam");
    relCex->setFilter("cex.is_el=true");

    relPackOp = new DbSqlRelation("pallets_op","id","nam",this);
    relPackOp->setSort("nam");

    relPallet = new DbSqlRelation("pallets","id","nam",this);
    relPallet->setSort("pallets.id desc");
    relPallet->model()->setLimit(2000);

    relMaster = new DbSqlRelation("kamin_empl","id","snam",this);
    relMaster->setAlias("rab_master");
    relMaster->setSort("rab_master.snam");
    relMaster->setFilter("rab_master.id in (select id from el_master)");
    relRabPack = new DbSqlRelation("kamin_empl","id","snam",this);
    relRabPack->setSort("kamin_empl.snam");
    relRabPack->setFilter("kamin_empl.id in (select id from el_packer)");

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
    query.prepare("insert into pallets (datetime, prefix) values (:datetime, :prefix) returning nam");
    query.bindValue(":datetime",QDateTime::currentDateTime());
    query.bindValue(":prefix",prefix);
    if (query.exec()){
        if (query.next()){
            palBarcode=query.value(0).toString();
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return palBarcode;
}

