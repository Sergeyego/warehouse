#include "formmovwire.h"
#include "ui_formmovwire.h"

FormMovWire::FormMovWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMovWire)
{
    ui->setupUi(this);
    loadSettings();
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    modelNamWire = new ModelNamWire(this);
    ui->tableViewNam->setModel(modelNamWire);

    modelPackWire = new ModelPackWire(this);
    ui->tableViewPack->setModel(modelPackWire);

    modelPerePackWire = new ModelPerePackWire(this);
    ui->tableViewPerePack->setModel(modelPerePackWire);

    modelBreakWire = new ModelBreakWire(this);
    ui->tableViewBreak->setModel(modelBreakWire);

    modelStockWire = new ModelStockWire(this);
    ui->tableViewStock->setModel(modelStockWire);

    modelShipWire = new ModelShipWire(this);
    ui->tableViewShip->setModel(modelShipWire);

    modelPart = new TableModel(this);

    QStringList header;
    header<<tr("id")<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Носитель")<<tr("Упаковка")<<tr("Источник")<<tr("Остаток");
    modelPart->setHeader(header);
    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->resizeToContents();

    executorPart = new ProgressExecutor(this);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(executorPart,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->plainTextEditPrim,SIGNAL(textChanged()),this,SLOT(enSavePrim()));
    connect(ui->pushButtonSavePrim,SIGNAL(clicked(bool)),this,SLOT(savePrim()));

    connect(modelNamWire,SIGNAL(sigSum(QString)),ui->labelNam,SLOT(setText(QString)));
    connect(modelPackWire,SIGNAL(sigSum(QString)),ui->labelPack,SLOT(setText(QString)));
    connect(modelPerePackWire,SIGNAL(sigSum(QString)),ui->labelPerePack,SLOT(setText(QString)));
    connect(modelBreakWire,SIGNAL(sigSum(QString)),ui->labelBreak,SLOT(setText(QString)));
    connect(modelStockWire,SIGNAL(sigSum(QString)),ui->labelStock,SLOT(setText(QString)));
    connect(modelShipWire,SIGNAL(sigSum(QString)),ui->labelShip,SLOT(setText(QString)));

    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updInfo(QModelIndex)));
}

FormMovWire::~FormMovWire()
{
    saveSettings();
    delete ui;
}

void FormMovWire::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("mov_wire_splitter_width").toByteArray());
}

void FormMovWire::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("mov_wire_splitter_width",ui->splitter->saveState());
}

void FormMovWire::startUpd()
{
    QString query = QString("select s.id_wparti, m.n_s ||'-'|| date_part('year',m.dat), pr.nam, d.sdim, pk.short, wp.pack_ed, ws.nam, s.st as sklad "
                            "from wire_calc_stock('3000-01-01') as s "
                            "inner join wire_parti as p on s.id_wparti=p.id "
                            "inner join wire_parti_m as m on p.id_m=m.id "
                            "inner join provol as pr on m.id_provol=pr.id "
                            "inner join diam as d on m.id_diam=d.id "
                            "inner join wire_pack_kind as pk on p.id_pack=pk.id "
                            "inner join wire_pack wp on wp.id = p.id_pack_type "
                            "inner join wire_source ws on ws.id = m.id_source "
                            "where m.dat between '%1' and '%2' "
                            "order by m.n_s, m.dat, pr.nam, d.sdim, pk.short").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    executorPart->setQuery(query);
    executorPart->start();
}

void FormMovWire::upd()
{
    QVector<QVector<QVariant>> data = executorPart->getData();
    modelPart->setModelData(data);
    ui->tableViewPart->resizeToContents();
    if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->selectRow(0);
    }
}

void FormMovWire::updInfo(QModelIndex index)
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toInt();

    QSqlQuery query;
    query.prepare("select prim_prod from wire_parti where id = :id");
    query.bindValue(":id",id_part);
    if (query.exec()){
        query.next();
        ui->plainTextEditPrim->setPlainText(query.value(0).toString());
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
    ui->pushButtonSavePrim->setEnabled(false);

    modelNamWire->refresh(id_part);
    ui->tableViewNam->resizeToContents();

    modelPackWire->refresh(id_part);
    ui->tableViewPack->resizeToContents();

    modelPerePackWire->refresh(id_part);
    ui->tableViewPerePack->resizeToContents();

    modelBreakWire->refresh(id_part);
    ui->tableViewBreak->resizeToContents();

    modelStockWire->refresh(id_part);
    ui->tableViewStock->resizeToContents();

    modelShipWire->refresh(id_part);
    ui->tableViewShip->resizeToContents();
}

void FormMovWire::enSavePrim()
{
    if (ui->tableViewPart->currentIndex().isValid()){
        ui->pushButtonSavePrim->setEnabled(true);
    }
}

void FormMovWire::savePrim()
{
    if (ui->tableViewPart->currentIndex().isValid()){
        int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(ui->tableViewPart->currentIndex().row(),0),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("update wire_parti set prim_prod = :text where id = :id");
        query.bindValue(":text",ui->plainTextEditPrim->toPlainText());
        query.bindValue(":id",id_part);
        if (query.exec()){
            ui->pushButtonSavePrim->setEnabled(false);
        } else {
            QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        }
    }
}

ModelNamWire::ModelNamWire(QObject *parent): ModelRo(parent)
{
    setDecimal(1);
}

void ModelNamWire::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Намотка");
    QSqlQuery query;
    query.prepare("select wicd.dat, wict.short, wicd.m_netto "
                  "from wire_in_cex_data wicd "
                  "inner join wire_in_cex_type wict on wict.id = wicd.id_type "
                  "where wict.koef = 1 and wicd.id_wparti = :id_part "
                  "order by wicd.dat, wict.nam ");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("Операция"));
        setHeaderData(2,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,2),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelPackWire::ModelPackWire(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelPackWire::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Упаковка");
    QSqlQuery query;
    query.prepare("select wpp.dat, wpp.kvo "
                  "from wire_parti_pack wpp "
                  "where wpp.id_part = :id_part order by wpp.dat");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,1),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}


ModelPerePackWire::ModelPerePackWire(QObject *parent): ModelRo(parent)
{
    setDecimal(1);
}

void ModelPerePackWire::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Переупаковка");
    QSqlQuery query;
    query.prepare(QString::fromUtf8("(select n.dat as dat, n.num, 'Переуп. в партию '||m.n_s ||'-'||date_part('year',m.dat) as nam, -1*p.m_netto as mas from wire_perepack as p "
                                    "inner join wire_perepack_nakl as n on p.id_nakl=n.id "
                                    "inner join wire_parti as wp on p.id_wpartires=wp.id "
                                    "inner join wire_parti_m as m on wp.id_m=m.id "
                                    "where p.id_wpartisrc= :id_part1 ) "
                                    "union "
                                    "(select n.dat as dat, n.num, 'Переуп. из партии '||m.n_s ||'-'||date_part('year',m.dat) as nam, p.m_netto from wire_perepack as p "
                                    "inner join wire_perepack_nakl as n on p.id_nakl=n.id "
                                    "inner join wire_parti as wp on p.id_wpartisrc=wp.id "
                                    "inner join wire_parti_m as m on wp.id_m=m.id "
                                    "where p.id_wpartires= :id_part2 ) "
                                    "order by dat, nam"));
    query.bindValue(":id_part1",id_part);
    query.bindValue(":id_part2",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Операция"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelBreakWire::ModelBreakWire(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelBreakWire::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Списание");
    QSqlQuery query;
    query.prepare("select wicd.dat, wict.short, wicd.m_netto "
                  "from wire_in_cex_data wicd "
                  "inner join wire_in_cex_type wict on wict.id = wicd.id_type "
                  "where wict.koef = -1 and wicd.id_wparti = :id_part "
                  "order by wicd.dat, wict.nam ");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("Операция"));
        setHeaderData(2,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,2),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelStockWire::ModelStockWire(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelStockWire::refresh(int id_part)
{
    QString title=tr("Передача на склад");
    QMap <QString,double> map;
    QSqlQuery query;
    query.prepare("select i.dat, i.num, t.nam, d.m_netto*t.koef "
                  "from wire_warehouse as d "
                  "inner join wire_whs_waybill as i on i.id=d.id_waybill "
                  "inner join wire_way_bill_type as t on t.id=i.id_type "
                  "where d.id_wparti = :id_part and t.koef <>0 "
                  "order by i.dat, i.num, t.nam");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Операция"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            QString key=data(index(i,2),Qt::EditRole).toString();
            double val=data(index(i,3),Qt::EditRole).toDouble();
            if (map.contains(key)){
                double oldval=map.value(key);
                map[key]=oldval+val;
            } else {
                map[key]=val;
            }
        }
    }
    QString s;
    QStringList list = map.keys();
    for (QString l : list) {
        if (!s.isEmpty()){
            s+=tr("; ");
        }
        s+=l+tr(": ")+QLocale().toString(map.value(l),'f',1)+tr(" кг");
    }
    if (!s.isEmpty()){
        s=tr(": ")+s;
    }
    emit sigSum(title+s);
}

ModelShipWire::ModelShipWire(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelShipWire::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Отгрузки");
    QSqlQuery query;
    query.prepare("select s.dat_vid, s.nom_s, p.short, d.m_netto "
                  "from wire_shipment_consist as d "
                  "inner join sertifikat as s on s.id=d.id_ship "
                  "inner join poluch as p on s.id_pol=p.id "
                  "where d.id_wparti = :id_part and s.id_type=1 "
                  "order by s.dat_vid, s.nom_s");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Получатель"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}
