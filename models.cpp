#include "models.h"

Models* Models::models_instance=0;

Models::Models(QObject *parent) :
    QObject(parent)
{
    sync1C = new Sync1C(this);

    relEl = newDbRelation(new DbRelationalModel("select id, marka from elrtr order by marka",this),0,1);
    relPol = newDbRelation(new DbRelationalModel("select id, short ||' "+tr("ИНН")+ " '|| COALESCE(substring(innkpp from '\\m\\d*'),'-'), naim from poluch order by short",this),0,1);
    relDrv = newDbRelation(new DbRelationalModel("select r.id, r.surname||' '||coalesce (substr(r.name,1,1)||'. ','')||coalesce (substr(r.middle_name,1,1)||'. ','')||coalesce('('||r.car_num||')','') as drv from drvs as r order by drv",this),0,1);
    relShipType = newDbRelation(new DbRelationalModel("select id, nam from sert_type order by nam",this),0,1);
    relAccTypeEl = newDbRelation(new DbRelationalModel("select id, nam, prefix from prod_nakl_tip where en=true order by nam",this),0,1);
    relAccTypeWire = newDbRelation(new DbRelationalModel("select id, nam, prefix from wire_way_bill_type where en=true order by nam",this),0,1);
    relDocType = newDbRelation(new DbRelationalModel("select id, nam from zvd_doc_type order by nam",this),0,1);

    relKis = newDbRelation(new DbRelationalModel("(select distinct ee.id_el||':'||ee.id_diam as kis, e.marka ||' ф '|| d.sdim as mark, 'e' as typ, e.id_u as id_u "
                                                 "from ean_el ee "
                                                 "inner join elrtr e on e.id = ee.id_el "
                                                 "inner  join diam d on d.id = ee.id_diam) "
                                                 "union "
                                                 "(select distinct we.id_prov ||':'||we.id_diam ||':'||we.id_spool, p.nam ||' ф '|| d.sdim||' '||wpk.short, 'w', 1 "
                                                 "from wire_ean we "
                                                 "inner join provol p on p.id=we.id_prov "
                                                 "inner  join diam d on d.id = we.id_diam "
                                                 "inner join wire_pack_kind wpk on wpk.id = we.id_spool) "
                                                 "order by typ, id_u, mark"),0,1);

    QString queryWire=QString("select p.id, m.n_s ||'-'||date_part('year',m.dat) ||' '||pr.nam ||' '|| d.sdim || ' '|| k.short || "
                              "CASE WHEN (p.id_pack_type <> 0) THEN (' (' || COALESCE(t.mas_ed,0) || ' кг)') ELSE '' END as part, "
                              "m.id_provol ||':'|| m.id_diam ||':'|| p.id_pack ||'-'||date_part('year',m.dat), "
                              "t.mas_ed "
                              "from wire_parti as p "
                              "inner join wire_parti_m as m on p.id_m=m.id "
                              "inner join provol as pr on pr.id=m.id_provol "
                              "inner join diam as d on d.id=m.id_diam "
                              "inner join wire_pack_kind as k on k.id=p.id_pack "
                              "inner join wire_pack as t on t.id=p.id_pack_type "
                              "where m.dat >= :d "
                              "order by part desc");


    modelWirePart = new ModelPart(queryWire,this);
    relWirePart = new RelPart(modelWirePart,this);
    rels.push_back(relWirePart);

    QString queryEl=QString("select p.id, p.n_s||'-'||date_part('year',p.dat_part)||' '||e.marka||' ф '||cast(p.diam as varchar(3)) "
                            "|| ' ('||ep.pack_ed||')' "
                            "|| CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' END "
                            "as str, "
                            "p.id_el ||':'||(select d.id from diam d where d.diam=p.diam)||'-'|| date_part('year',p.dat_part), ep.mass_ed "
                            "from parti p "
                            "inner join el_pack ep on ep.id=p.id_pack "
                            "inner join elrtr e on e.id=p.id_el "
                            "inner join elrtr_vars ev on ev.id = p.id_var "
                            "where p.dat_part >= :d "
                            "order by str desc");
    modelElPart = new ModelPart(queryEl,this);
    relElPart = new RelPart(modelElPart,this);
    rels.push_back(relElPart);

    relKis->proxyModel()->sort(1);
    relKis->proxyModel()->sort(2);
}

DbRelation *Models::newDbRelation(QAbstractItemModel *queryModel, int key, int disp)
{
    DbRelation *r = new DbRelation(queryModel,key,disp,this);
    rels.push_back(r);
    return r;
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

void Models::refresh()
{
    for (int i=0; i < rels.size(); ++i) {
        rels.at(i)->refreshModel();
    }
    emit sigRefresh();
}

ModelPart::ModelPart(QString partQuery, QObject *parent) : DbRelationalModel(parent), baseQuery(partQuery)
{
    minDate = QDate::currentDate().addDays(1);
}

void ModelPart::setMinDate(QDate d, bool upd)
{
    if (d<minDate){
        minDate=d;
        refresh();
    } else if (rowCount()<1 || upd) {
        refresh();
    }
}

void ModelPart::refresh()
{
    QString query=baseQuery;
    query.replace(":d","'"+minDate.toString("yyyy-MM-dd")+"'");
    setQuery(query);
    //qDebug()<<minDate;
}

RelPart::RelPart(DbRelationalModel *model, QObject *parent) : DbRelation(model,0,1,parent)
{
    this->proxyModel()->setFilterKeyColumn(2);
    fltIndex=1;
    setFilter(fltIndex);
}

int RelPart::currentFilter()
{
    return fltIndex;
}

void RelPart::setFilter(int index)
{
    int year=QDate::currentDate().year();
    QString pattern=QString();
    if (index==0){
        pattern=QString::number(year);
    } else if (index==1){
        pattern=QString::number(year-1)+"|"+QString::number(year);
    }
    this->proxyModel()->setFilterRegExp(pattern);
    fltIndex=index;
    emit filterChanged(index);
}
