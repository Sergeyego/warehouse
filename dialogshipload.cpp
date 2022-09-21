#include "dialogshipload.h"
#include "ui_dialogshipload.h"

DialogShipLoad::DialogShipLoad(int id_ship, QWidget *parent) :
    QDialog(parent), id_s(id_ship),
    ui(new Ui::DialogShipLoad)
{
    ui->setupUi(this);

    modelShip = new ModelRo(this);
    modelEl = new ModelRo(this);
    modelWire = new ModelRo(this);

    QSqlQuery query;
    query.prepare("select sp.id, sp.nom_s, sp.dat_vid, p.short "
                  "from ship_plan sp "
                  "inner join poluch p on p.id = sp.id_pol "
                  "where sp.id_pol = (select s.id_pol  from sertifikat s where s.id = :id_sert ) and sp.dat_vid>= :d "
                  "order by sp.dat_vid desc, sp.nom_s desc");
    query.bindValue(":id_sert",id_s);
    query.bindValue(":d",QDate::currentDate().addYears(-1));
    if (modelShip->execQuery(query)){
        modelShip->setHeaderData(1,Qt::Horizontal,tr("Номер"));
        modelShip->setHeaderData(2,Qt::Horizontal,tr("Дата"));
        modelShip->setHeaderData(3,Qt::Horizontal,tr("Получатель"));
    }

    ui->tableViewShip->setModel(modelShip);
    if (modelShip->columnCount()){
        ui->tableViewShip->setColumnHidden(0,true);
        ui->tableViewShip->resizeToContents();
    }

    ui->tableViewEl->setModel(modelEl);
    ui->tableViewWire->setModel(modelWire);

    connect(ui->tableViewShip->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updShipData(QModelIndex)));

    if (ui->tableViewShip->model()->rowCount()){
        ui->tableViewShip->selectRow(0);
    }
}

DialogShipLoad::~DialogShipLoad()
{
    delete ui;
}

void DialogShipLoad::accept()
{
    for (int i=0; i<modelEl->rowCount(); i++){
        int id_part = modelEl->data(modelEl->index(i,0),Qt::EditRole).toInt();
        double mas=modelEl->data(modelEl->index(i,6),Qt::EditRole).toDouble();
        double masShip=getStockEl(id_part);
        if (mas>=0 && mas<=masShip){
            if (!insertEl(id_part,mas)){
                break;
            }
        } else {
            QString part = modelEl->data(modelEl->index(i,1),Qt::EditRole).toString();
            part+=tr(" (")+modelEl->data(modelEl->index(i,2),Qt::EditRole).toString();
            part+=tr(" ф")+modelEl->data(modelEl->index(i,3),Qt::EditRole).toString();
            part+=tr(")");
            QString mess=QString("На складе на день отгрузки числится %1 "
                                 "кг электродов партии %2 из требуемых %3 кг. "
                                 "Масса передачи должна быть положительной и не больше, "
                                 "чем числится на складе.").arg(masShip).arg(part).arg(mas);
            QMessageBox::critical(this,tr("Ошибка"),mess,QMessageBox::Cancel);
            break;
        }
    }

    for (int i=0; i<modelWire->rowCount(); i++){
        int id_part = modelWire->data(modelWire->index(i,0),Qt::EditRole).toInt();
        double mas=modelWire->data(modelWire->index(i,6),Qt::EditRole).toDouble();
        double masShip=getStockWire(id_part);
        if (mas>=0 && mas<=masShip){
            if (!insertWire(id_part,mas)){
                break;
            }
        } else {
            QString part = modelWire->data(modelWire->index(i,1),Qt::EditRole).toString();
            part+=tr(" (")+modelWire->data(modelWire->index(i,2),Qt::EditRole).toString();
            part+=tr(" ф")+modelWire->data(modelWire->index(i,3),Qt::EditRole).toString();
            part+=tr(" ")+modelWire->data(modelWire->index(i,4),Qt::EditRole).toString();
            part+=tr(")");
            QString mess=QString("На складе на день отгрузки числится %1 "
                                 "кг проволоки партии %2 из требуемых %3 кг. "
                                 "Масса передачи должна быть положительной и не больше, "
                                 "чем числится на складе.").arg(masShip).arg(part).arg(mas);
            QMessageBox::critical(this,tr("Ошибка"),mess,QMessageBox::Cancel);
            break;
        }
    }

    return QDialog::accept();
}

double DialogShipLoad::getStockWire(int id_part)
{
    double kvo=0;
    QSqlQuery query;
    query.prepare("select st from wire_calc_stock((select dat_vid from sertifikat where id = :id_ship)) where id_wparti= :id_part");
    query.bindValue(":id_part",id_part);
    query.bindValue(":id_ship",id_s);
    if (query.exec()){
        while (query.next()){
            kvo = query.value(0).toDouble();
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return kvo;
}

bool DialogShipLoad::insertEl(int id_part, double kvo)
{
    bool ok=false;
    QSqlQuery query;
    query.prepare("insert into otpusk (id_sert, id_part, massa) values (:id_sert, :id_part, :massa)");
    query.bindValue(":id_sert",id_s);
    query.bindValue(":id_part",id_part);
    query.bindValue(":massa",kvo);
    ok=query.exec();
    if (!ok){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

bool DialogShipLoad::insertWire(int id_part, double kvo)
{
    bool ok=false;
    QSqlQuery query;
    query.prepare("insert into wire_shipment_consist (id_ship, id_wparti, m_netto) values (:id_sert, :id_part, :massa)");
    query.bindValue(":id_sert",id_s);
    query.bindValue(":id_part",id_part);
    query.bindValue(":massa",kvo);
    ok=query.exec();
    if (!ok){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

double DialogShipLoad::getStockEl(int id_part)
{
    double kvo=0;
    QSqlQuery query;
    query.prepare("select kvoRs from calc_parti_one(:id_part, (select dat_vid from sertifikat where id = :id_ship ))");
    query.bindValue(":id_part",id_part);
    query.bindValue(":id_ship",id_s);
    if (query.exec()){
        while (query.next()){
            kvo = query.value(0).toDouble();
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return kvo;
}

void DialogShipLoad::updShipData(QModelIndex index)
{
    int id_sert=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index.row(),0),Qt::EditRole).toInt();

    double sum_el=0;
    QSqlQuery queryEl;
    queryEl.prepare("select spe.id_part, p.n_s||'-'||date_part('year',p.dat_part), e.marka, p.diam, ev.nam, ep.pack_ed, spe.massa "
                    "from ship_plan_el spe "
                    "inner join parti p on p.id = spe.id_part "
                    "inner join elrtr e on e.id = p.id_el "
                    "inner join elrtr_vars ev on ev.id = p.id_var "
                    "inner join el_pack ep on ep.id = p.id_pack "
                    "where spe.id_sert = :id_sert "
                    "order by spe.id");
    queryEl.bindValue(":id_sert",id_sert);
    if (modelEl->execQuery(queryEl)){
        for (int i=0; i<modelEl->rowCount(); i++){
            sum_el+=modelEl->data(modelEl->index(i,6),Qt::EditRole).toDouble();
        }
        modelEl->setHeaderData(1,Qt::Horizontal,tr("Партия"));
        modelEl->setHeaderData(2,Qt::Horizontal,tr("Марка"));
        modelEl->setHeaderData(3,Qt::Horizontal,tr("Диаметр"));
        modelEl->setHeaderData(4,Qt::Horizontal,tr("Вариант"));
        modelEl->setHeaderData(5,Qt::Horizontal,tr("Упаковка"));
        modelEl->setHeaderData(6,Qt::Horizontal,tr("Масса, кг"));
        ui->tableViewEl->setColumnHidden(0,true);
        ui->tableViewEl->resizeToContents();
    }
    QString lblEl=tr("Электроды");
    if (sum_el>0){
        lblEl+=tr(" итого: ")+QLocale().toString(sum_el,'f',2)+tr(" кг");
    }
    ui->labelEl->setText(lblEl);

    double sum_wire=0;
    QSqlQuery queryWire;
    queryWire.prepare("select spw.id_wparti, wpm.n_s||'-'||date_part('year',wpm.dat), p.nam, d.sdim, wpk.short, wp2.pack_ed, spw.m_netto "
                      "from ship_plan_wire spw "
                      "inner join wire_parti wp on wp.id = spw.id_wparti "
                      "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                      "inner join provol p on p.id = wpm.id_provol "
                      "inner join diam d on d.id = wpm.id_diam "
                      "inner join wire_pack_kind wpk on wpk.id = wp.id_pack "
                      "inner join wire_pack wp2 on wp2.id = wp.id_pack_type "
                      "where spw.id_ship = :id_sert "
                      "order by spw.id");
    queryWire.bindValue(":id_sert",id_sert);
    if (modelWire->execQuery(queryWire)){
        for (int i=0; i<modelWire->rowCount(); i++){
            sum_wire+=modelWire->data(modelWire->index(i,6),Qt::EditRole).toDouble();
        }
        modelWire->setHeaderData(1,Qt::Horizontal,tr("Партия"));
        modelWire->setHeaderData(2,Qt::Horizontal,tr("Марка"));
        modelWire->setHeaderData(3,Qt::Horizontal,tr("Диаметр"));
        modelWire->setHeaderData(4,Qt::Horizontal,tr("Носитель"));
        modelWire->setHeaderData(5,Qt::Horizontal,tr("Упаковка"));
        modelWire->setHeaderData(6,Qt::Horizontal,tr("Масса, кг"));
        ui->tableViewWire->setColumnHidden(0,true);
        ui->tableViewWire->resizeToContents();
    }
    QString lblWire=tr("Проволока");
    if (sum_wire>0){
        lblWire+=tr(" итого: ")+QLocale().toString(sum_wire,'f',2)+tr(" кг");
    }
    ui->labelWire->setText(lblWire);
}
