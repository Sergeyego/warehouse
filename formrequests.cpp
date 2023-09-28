#include "formrequests.h"
#include "ui_formrequests.h"

FormRequests::FormRequests(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRequests)
{
    ui->setupUi(this);
    loadSettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    for (int i=1; i<=12; i++){
        ui->comboBoxMonth->addItem(QLocale().standaloneMonthName(i));
    }
    ui->comboBoxMonth->setCurrentIndex(QDate::currentDate().month()-1);
    ui->spinBoxYear->setValue(QDate::currentDate().year());

    modelStatEl = new ModelStat(this);
    modelStatEl->setQuery("select z.nam, z.plan, z.cor, s.itogo, "
                          "coalesce(z.id_el, s.id_el), coalesce(z.diam,s.diam), coalesce(z.id_pack, s.id_pack) from ( "
                          "select e.marka ||' ф '||d.sdim || ' ('||ep.pack_ed||')' as nam, sum(re.itogo) as plan, sum(re.cor) as cor, "
                          "re.id_el as id_el, d.diam as diam, re.id_pack as id_pack, e.marka as marka "
                          "from requests_el re "
                          "inner join elrtr e on e.id = re.id_el "
                          "inner join diam d on d.id = re.id_diam "
                          "inner join elrtr_vars ev on ev.id = re.id_var "
                          "inner join el_pack ep on ep.id = re.id_pack "
                          "where re.id_req = :id_req "
                          "group by e.marka, d.sdim, ep.pack_ed, re.id_el, d.diam, re.id_pack "
                          ") as z "
                          "full join "
                          "( "
                          "select p.id_el as id_el, p.diam as diam, p.id_pack as id_pack, sum(rse.kvo) as itogo "
                          "from requests_ship_el rse "
                          "inner join otpusk o on o.id = rse.id_ship_data "
                          "inner join parti p on p.id = o.id_part "
                          "where rse.id_req = :id_req "
                          "group by p.id_el, p.diam, p.id_pack "
                          ") as s on s.id_el=z.id_el and s.diam = z.diam and s.id_pack = z.id_pack "
                          "order by z.marka, z.diam");
    ui->tableViewStatEl->setModel(modelStatEl);

    modelStatWire = new ModelStat(this);
    modelStatWire->setQuery("select z.nam, z.plan, z.cor, s.itogo, "
                            "coalesce(z.id_provol, s.id_provol), coalesce(z.id_diam,s.id_diam), coalesce(z.id_spool, s.id_spool) from ( "
                            "select p.nam || ' ф ' || d.sdim ||' '|| wpk.short as nam, sum(rw.itogo) as plan, sum(rw.cor) as cor, "
                            "rw.id_provol as id_provol, rw.id_diam as id_diam, rw.id_spool as id_spool "
                            "from requests_wire rw "
                            "inner join provol p on p.id = rw.id_provol "
                            "inner join diam d on d.id = rw.id_diam "
                            "inner join wire_pack_kind wpk on wpk.id = rw.id_spool "
                            "where rw.id_req = :id_req "
                            "group by p.nam, d.sdim, wpk.short, rw.id_provol, rw.id_diam, rw.id_spool "
                            ") as z "
                            "full join "
                            "( "
                            "select wpm.id_provol as id_provol, wpm.id_diam as id_diam, wp.id_pack as id_spool, sum(rsw.kvo) as itogo "
                            "from requests_ship_wire rsw "
                            "inner join wire_shipment_consist wsc on wsc.id = rsw.id_ship_data "
                            "inner join wire_parti wp on wp.id = wsc.id_wparti "
                            "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                            "where rsw.id_req = :id_req "
                            "group by wpm.id_provol, wpm.id_diam, wp.id_pack "
                            ") as s on s.id_provol=z.id_provol and s.id_diam = z.id_diam and s.id_spool = z.id_spool "
                            "order by z.nam");
    ui->tableViewStatWire->setModel(modelStatWire);

    modelStatElData = new ModelRo(this);
    modelStatElData->setDecimal(1);
    ui->tableViewStatElData->setModel(modelStatElData);

    modelStatWireData = new ModelRo(this);
    modelStatWireData->setDecimal(1);
    ui->tableViewStatWireData->setModel(modelStatWireData);

    modelChanges = new DbTableModel("requests_changes",this);
    modelChanges->addColumn("id",tr("id"));
    modelChanges->addColumn("id_req",tr("id_req"));
    modelChanges->addColumn("nam",tr("Изменения"));
    modelChanges->addColumn("dat",tr("Дата"));
    modelChanges->setSort("requests_changes.dat, requests_changes.nam");
    ui->tableViewChanges->setModel(modelChanges);
    ui->tableViewChanges->setColumnHidden(0,true);
    ui->tableViewChanges->setColumnHidden(1,true);
    ui->tableViewChanges->setColumnWidth(2,80);
    ui->tableViewChanges->setColumnWidth(3,80);

    modelReqEl = new ModelReqEl(this);
    ui->tableViewEl->setModel(modelReqEl);
    ui->tableViewEl->setColumnHidden(0,true);
    ui->tableViewEl->setColumnWidth(1,130);
    ui->tableViewEl->setColumnWidth(2,60);
    ui->tableViewEl->setColumnWidth(3,150);
    ui->tableViewEl->setColumnWidth(4,110);
    ui->tableViewEl->setColumnWidth(5,80);
    ui->tableViewEl->setColumnWidth(6,70);
    ui->tableViewEl->setColumnWidth(7,70);
    ui->tableViewEl->setColumnWidth(8,70);
    ui->tableViewEl->setColumnWidth(9,130);
    ui->tableViewEl->setColumnWidth(10,110);

    modelReqWire = new ModelReqWire(this);
    ui->tableViewWire->setModel(modelReqWire);
    ui->tableViewWire->setColumnHidden(0,true);
    ui->tableViewWire->setColumnWidth(1,130);
    ui->tableViewWire->setColumnWidth(2,60);
    ui->tableViewWire->setColumnWidth(3,110);
    ui->tableViewWire->setColumnWidth(4,150);
    ui->tableViewWire->setColumnWidth(5,80);
    ui->tableViewWire->setColumnWidth(6,70);
    ui->tableViewWire->setColumnWidth(7,70);
    ui->tableViewWire->setColumnWidth(8,70);
    ui->tableViewWire->setColumnWidth(9,130);
    ui->tableViewWire->setColumnWidth(10,110);

    modelReq = new ModelReq(this);
    ui->tableViewReq->setModel(modelReq);
    ui->tableViewReq->setColumnHidden(0,true);
    ui->tableViewReq->setColumnWidth(1,80);
    ui->tableViewReq->setColumnWidth(2,80);
    ui->tableViewReq->setColumnWidth(3,300);
    for (int i=4; i<ui->tableViewReq->model()->columnCount(); i++){
        ui->tableViewReq->setColumnHidden(i,true);
    }

    mapper = new DbMapper(ui->tableViewReq,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEditReq,2);
    mapper->addMapping(ui->comboBoxPol,3);
    mapper->addMapping(ui->comboBoxCat,4);
    mapper->addMapping(ui->lineEditComment,5);
    mapper->addMapping(ui->lineEditTdNum,6);
    mapper->addEmptyLock(ui->tableViewEl);
    mapper->addEmptyLock(ui->tableViewWire);
    mapper->addEmptyLock(ui->tableViewChanges);
    mapper->addEmptyLock(ui->pushButtonLoad);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->comboBoxMonth,SIGNAL(currentIndexChanged(int)),this,SLOT(updReq()));
    connect(ui->spinBoxYear,SIGNAL(valueChanged(int)),this,SLOT(updReq()));

    connect(ui->pushButtonUpd,&QPushButton::clicked,this,&FormRequests::updReq);
    connect(mapper,&DbMapper::currentIndexChanged,this,&FormRequests::updData);
    connect(modelReqEl,&ModelReqEl::sigSum,ui->labelElSum,&QLabel::setText);
    connect(modelReqWire,&ModelReqWire::sigSum,ui->labelWireSum,&QLabel::setText);
    connect(ui->checkBoxMonth,&QCheckBox::clicked,this,&FormRequests::switchFlt);

    connect(ui->tableViewStatEl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updStatDataEl(QModelIndex)));
    connect(modelReqEl,SIGNAL(sigUpd()),modelStatEl,SLOT(select()));
    connect(modelStatEl,SIGNAL(sigUpd()),ui->tableViewStatEl,SLOT(resizeToContents()));

    connect(ui->tableViewStatWire->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updStatDataWire(QModelIndex)));
    connect(modelReqWire,SIGNAL(sigUpd()),modelStatWire,SLOT(select()));
    connect(modelStatWire,SIGNAL(sigUpd()),ui->tableViewStatWire,SLOT(resizeToContents()));

    connect(ui->pushButtonLoad,SIGNAL(clicked(bool)),this,SLOT(loadReq()));

    updReq();
}

FormRequests::~FormRequests()
{
    saveSettings();
    delete ui;
}

void FormRequests::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("requests_splitter_width").toByteArray());
    ui->tabWidget->setCurrentIndex(settings.value("requests_tab_index").toInt());
}

void FormRequests::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("requests_splitter_width",ui->splitter->saveState());
    settings.setValue("requests_tab_index",ui->tabWidget->currentIndex());
}

void FormRequests::updReq()
{
    if (ui->checkBoxMonth->isChecked()){
        modelReq->refresh(ui->comboBoxMonth->currentIndex()+1,ui->spinBoxYear->value());
    } else {
        modelReq->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    }
}

void FormRequests::updData(int index)
{
    int id_req=mapper->modelData(index,0).toInt();
    modelReqEl->refresh(id_req);
    modelReqWire->refresh(id_req);

    modelChanges->setFilter("requests_changes.id_req="+QString::number(id_req));
    modelChanges->setDefaultValue(1,id_req);
    modelChanges->select();

    modelStatEl->refresh(id_req);
    for (int i=4; i<ui->tableViewStatEl->model()->columnCount(); i++){
        ui->tableViewStatEl->setColumnHidden(i,true);
    }
    if (ui->tableViewStatEl->model()->rowCount()){
        ui->tableViewStatEl->selectRow(0);
    } else {
        modelStatElData->clear();
    }

    modelStatWire->refresh(id_req);
    for (int i=4; i<ui->tableViewStatWire->model()->columnCount(); i++){
        ui->tableViewStatWire->setColumnHidden(i,true);
    }
    if (ui->tableViewStatWire->model()->rowCount()){
        ui->tableViewStatWire->selectRow(0);
    } else {
        modelStatWireData->clear();
    }
}

void FormRequests::updStatDataEl(QModelIndex index)
{
    int id_el=ui->tableViewStatEl->model()->data(ui->tableViewStatEl->model()->index(index.row(),4),Qt::EditRole).toInt();
    double diam=ui->tableViewStatEl->model()->data(ui->tableViewStatEl->model()->index(index.row(),5),Qt::EditRole).toDouble();
    int id_pack=ui->tableViewStatEl->model()->data(ui->tableViewStatEl->model()->index(index.row(),6),Qt::EditRole).toInt();
    int id_req=mapper->modelData(mapper->currentIndex(),0).toInt();

    QSqlQuery query;
    query.prepare("select s.dat_vid, s.nom_s||' '||p2.short as srt, p.str, rse.kvo "
                  "from requests_ship_el rse "
                  "inner join otpusk o on o.id = rse.id_ship_data "
                  "inner join sertifikat s on s.id = o.id_sert "
                  "inner join poluch p2 on p2.id = s.id_pol "
                  "inner join parti p on p.id = o.id_part "
                  "where rse.id_req = :id_req and p.id_el = :id_el and p.diam = :diam and p.id_pack = :id_pack "
                  "order by s.dat_vid");
    query.bindValue(":id_req",id_req);
    query.bindValue(":id_el",id_el);
    query.bindValue(":diam",diam);
    query.bindValue(":id_pack",id_pack);
    if (modelStatElData->execQuery(query)){
        modelStatElData->setHeaderData(0,Qt::Horizontal,tr("Дата"));
        modelStatElData->setHeaderData(1,Qt::Horizontal,tr("Отгрузка"));
        modelStatElData->setHeaderData(2,Qt::Horizontal,tr("Партия"));
        modelStatElData->setHeaderData(3,Qt::Horizontal,tr("Масса, кг"));
        ui->tableViewStatElData->resizeToContents();
    }
}

void FormRequests::updStatDataWire(QModelIndex index)
{
    int id_provol=ui->tableViewStatWire->model()->data(ui->tableViewStatWire->model()->index(index.row(),4),Qt::EditRole).toInt();
    int id_diam=ui->tableViewStatWire->model()->data(ui->tableViewStatWire->model()->index(index.row(),5),Qt::EditRole).toInt();
    int id_pack=ui->tableViewStatWire->model()->data(ui->tableViewStatWire->model()->index(index.row(),6),Qt::EditRole).toInt();
    int id_req=mapper->modelData(mapper->currentIndex(),0).toInt();

    QSqlQuery query;
    query.prepare("select s.dat_vid, s.nom_s||' '||p.short as srt, wp.str, rsw.kvo "
                  "from requests_ship_wire rsw "
                  "inner join wire_shipment_consist wsc on wsc.id = rsw.id_ship_data "
                  "inner join sertifikat s on s.id = wsc.id_ship "
                  "inner join poluch p on p.id = s.id_pol "
                  "inner join wire_parti wp on wp.id = wsc.id_wparti "
                  "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                  "where rsw.id_req  = :id_req and wpm.id_provol = :id_provol and wpm.id_diam  = :id_diam and wp.id_pack = :id_pack "
                  "order by s.dat_vid");
    query.bindValue(":id_req",id_req);
    query.bindValue(":id_provol",id_provol);
    query.bindValue(":id_diam",id_diam);
    query.bindValue(":id_pack",id_pack);
    if (modelStatWireData->execQuery(query)){
        modelStatWireData->setHeaderData(0,Qt::Horizontal,tr("Дата"));
        modelStatWireData->setHeaderData(1,Qt::Horizontal,tr("Отгрузка"));
        modelStatWireData->setHeaderData(2,Qt::Horizontal,tr("Партия"));
        modelStatWireData->setHeaderData(3,Qt::Horizontal,tr("Масса, кг"));
        ui->tableViewStatWireData->resizeToContents();
    }
}

void FormRequests::switchFlt(bool b)
{
    ui->dateEditBeg->setEnabled(!b);
    ui->dateEditEnd->setEnabled(!b);
    ui->comboBoxMonth->setEnabled(b);
    ui->spinBoxYear->setEnabled(b);
    updReq();
}

void FormRequests::loadReq()
{
    DialogReqLoad d;
    d.exec();
}

ModelReq::ModelReq(QWidget *parent) : DbTableModel("requests",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_rec",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_cat",tr("Представительство"),Models::instance()->relKat);
    addColumn("comment",tr("Комментарий"));
    addColumn("tdnum",tr("Номер 1С"));
    setSort(QString("%1.dat, %2.num").arg(name()).arg(name()));
}

QVariant ModelReq::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        int id=this->data(this->index(index.row(),0),Qt::EditRole).toInt();
        return status.value(id,QColor(255,255,255));
    }
    return DbTableModel::data(index,role);
}

void ModelReq::refresh(QDate beg, QDate end)
{
    flt=".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    QString filter=name()+flt;
    setFilter(filter);
    refreshState();
    select();
}

void ModelReq::refresh(int month, int year)
{
    flt=QString(".id in (select distinct re.id_req from requests_el re "
                "where date_part('month',re.dat_term) = %1 and date_part('year',re.dat_term) = %2 "
                "union "
                "select distinct rw.id_req  from requests_wire rw "
                "where date_part('month',rw.dat_term) = %1 and date_part('year',rw.dat_term) = %2 )").arg(month).arg(year);
    QString filter=name()+flt;
    setFilter(filter);
    refreshState();
    select();
}

bool ModelReq::insertRow(int row, const QModelIndex &parent)
{
    select();
    int old_num=0;
    if (rowCount()>0) {
        old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
    }
    setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

void ModelReq::refreshState()
{
    QString qu("select r.id, (select coalesce(sum(re.itogo),0) from requests_el re where re.id_req = r.id)+ "
                  "(select coalesce(sum(rw.itogo),0) from requests_wire rw where rw.id_req = r.id) as plan, "
                  "(select coalesce(sum(rse.kvo),0) from requests_ship_el rse where rse.id_req = r.id)+ "
                  "(select coalesce(sum(rsw.kvo),0) from requests_ship_wire rsw where rsw.id_req = r.id) as ship "
                  "from requests r");
    if (!flt.isEmpty()){
        qu+=" where r"+flt;
    }
    QSqlQuery query;
    query.prepare(qu);
    if (query.exec()){
        status.clear();
        while (query.next()){
            double plan=query.value(1).toDouble();
            double ship=query.value(2).toDouble();
            QColor color = QColor(255,255,255);
            if (plan==ship){
                color = QColor(170,255,170);
            } else if (ship>0){
                color = QColor(255,200,100);
            }
            status.insert(query.value(0).toInt(),color);
        }
        if (this->rowCount()){
            emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

ModelReqEl::ModelReqEl(QWidget *parent) : DbTableModel("requests_el",parent)
{
    addColumn("id_req",tr("id_req"));
    addColumn("id_el",tr("Марка"),Models::instance()->relElrtr);
    addColumn("id_diam",tr("Диам."),Models::instance()->relDiam);
    addColumn("id_pack",tr("Упаковка"),Models::instance()->relElPack);
    addColumn("id_var",tr("Вариант"),Models::instance()->relVars);
    addColumn("dat_term",tr("Срок"));
    addColumn("plan",tr("План"));
    addColumn("cor",tr("Коррект."));
    addColumn("itogo",tr("Итого"));
    addColumn("id_cause",tr("Причина коррект."),Models::instance()->relReqCause);
    addColumn("comment",tr("Примечание"));
    setDefaultValue(4,1);
    setSort(QString("elrtr.marka, diam.diam, requests_el.dat_term"));

    setColumnFlags(8,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    connect(this,&ModelReqEl::sigUpd,this,&ModelReqEl::calcSum);
    connect(this,&ModelReqEl::sigRefresh,this,&ModelReqEl::calcSum);
}

void ModelReqEl::refresh(int id_req)
{
    QString filter=name()+".id_req="+QString::number(id_req);
    setFilter(filter);
    setDefaultValue(0,id_req);
    select();
}

bool ModelReqEl::insertRow(int row, const QModelIndex &parent)
{
    if (rowCount()>0){
        int oldMark=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        QDate oldDate=this->data(this->index(rowCount()-1,5),Qt::EditRole).toDate();
        setDefaultValue(1,oldMark);
        setDefaultValue(5,oldDate);
    }
    return DbTableModel::insertRow(row,parent);
}

bool ModelReqEl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()==2 && role==Qt::EditRole){
        int id_el = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        int id_diam = value.toInt();
        QVariant id_pack = getIdPack(id_el,id_diam);
        if (!id_pack.isNull()){
            setData(this->index(index.row(),3),id_pack,Qt::EditRole);
            if (this->sqlRelation(3)){
                QVariant display= this->sqlRelation(3)->getDisplayValue(id_pack);
                setData(this->index(index.row(),3),display,Qt::DisplayRole);
            }
        }
    }
    return DbTableModel::setData(index,value,role);
}

QVariant ModelReqEl::getIdPack(int id_el, int id_diam)
{
    QVariant id_pack;
    QSqlQuery query;
    query.prepare("select p.id_pack, count(p.id_pack) as stat "
                  "from parti p "
                  "inner join diam d on d.diam = p.diam "
                  "where p.dat_part >= :dat and p.id_el = :id_el and d.id = :id_diam "
                  "group by p.id_pack order by stat desc");
    query.bindValue(":dat",QDate::currentDate().addDays(-365));
    query.bindValue(":id_el",id_el);
    query.bindValue(":id_diam", id_diam);
    if (query.exec()){
        if (query.next()){
            id_pack=query.value(0);
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return id_pack;
}

void ModelReqEl::calcSum()
{
    double sum=0, sumCorr=0;
    QString title = tr("Электроды");
    for (int i=0; i<rowCount(); i++){
        sumCorr+=data(index(i,7),Qt::EditRole).toDouble();
        sum+=data(index(i,8),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0 || sumCorr>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг; Корректировка итого: ")+QLocale().toString(sumCorr,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelReqWire::ModelReqWire(QWidget *parent) : DbTableModel("requests_wire",parent)
{
    addColumn("id_req",tr("id_req"));
    addColumn("id_provol",tr("Марка"),Models::instance()->relProvol);
    addColumn("id_diam",tr("Диам."),Models::instance()->relDiam);
    addColumn("id_spool",tr("Носитель"),Models::instance()->relSpool);
    addColumn("id_pack",tr("Упаковка"),Models::instance()->relWirePack);
    addColumn("dat_term",tr("Срок"));
    addColumn("plan",tr("План"));
    addColumn("cor",tr("Коррект."));
    addColumn("itogo",tr("Итого"));
    addColumn("id_cause",tr("Причина коррект."),Models::instance()->relReqCause);
    addColumn("comment",tr("Примечание"));
    setSort(QString("provol.nam, diam.diam, wire_pack_kind.short, requests_wire.dat_term"));

    setColumnFlags(8,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    connect(this,&ModelReqWire::sigUpd,this,&ModelReqWire::calcSum);
    connect(this,&ModelReqWire::sigRefresh,this,&ModelReqWire::calcSum);
}

void ModelReqWire::refresh(int id_req)
{
    QString filter=name()+".id_req="+QString::number(id_req);
    setFilter(filter);
    setDefaultValue(0,id_req);
    select();
}

bool ModelReqWire::insertRow(int row, const QModelIndex &parent)
{
    if (rowCount()>0){
        int oldMark=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        QDate oldDate=this->data(this->index(rowCount()-1,5),Qt::EditRole).toDate();
        setDefaultValue(1,oldMark);
        setDefaultValue(5,oldDate);
    }
    return DbTableModel::insertRow(row,parent);
}

bool ModelReqWire::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()==3 && role==Qt::EditRole){
        if (this->sqlRelation(3)){
            int id_spool=value.toInt();
            int id_pack=this->sqlRelation(3)->getDisplayValue(id_spool,"id_pack_def").toInt();
            setData(this->index(index.row(),4),id_pack,Qt::EditRole);
            if (this->sqlRelation(4)){
                QVariant display= this->sqlRelation(4)->getDisplayValue(id_pack);
                setData(this->index(index.row(),4),display,Qt::DisplayRole);
            }
        }
    }
    return DbTableModel::setData(index,value,role);
}

void ModelReqWire::calcSum()
{
    double sum=0, sumCorr=0;
    QString title = tr("Проволока");
    for (int i=0; i<rowCount(); i++){
        sumCorr+=data(index(i,7),Qt::EditRole).toDouble();
        sum+=data(index(i,8),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0 || sumCorr>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг; Корректировка итого: ")+QLocale().toString(sumCorr,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelStat::ModelStat(QWidget *parent) : TableModel(parent)
{
    setDecimal(1);
    QStringList header;
    header<<tr("Номенклатура")<<tr("План, кг")<<tr("Корр., кг")<<tr("Отгруж., кг")<<tr("id_prod")<<tr("id_diam")<<tr("id_pack");
    setHeader(header);
}

void ModelStat::refresh(int id_req)
{
    currentIdReq=id_req;
    QVector<QVector<QVariant>> dt;
    QSqlQuery query;
    QString strQu=strQuery;
    strQu=strQu.replace(":id_req",QString::number(id_req));

    query.prepare(strQu);
    if (query.exec()){
        int colCount=query.record().count();
        QVector<QVariant> sums;
        sums.resize(colCount);
        while (query.next()){
            QVector<QVariant> st;
            for (int i=0; i<colCount; i++){
                st.push_back(query.value(i));
                if (i>=1 && i<=3){
                    sums[i]=sums[i].toDouble()+query.value(i).toDouble();
                }
            }
            dt.push_back(st);
        }
        sums[0]=tr("ИТОГО");
        if (!dt.isEmpty()){
            dt.push_back(sums);
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    setModelData(dt);
}

QVariant ModelStat::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundRole){
        double plan=this->data(this->index(item.row(),1),Qt::EditRole).toDouble();
        double ship=this->data(this->index(item.row(),3),Qt::EditRole).toDouble();
        if (plan==ship){
            return QVariant(QColor(170,255,170));
        } else if (ship>0){
            return QVariant(QColor(255,200,100));
        }
    }
    return TableModel::data(item,role);
}

void ModelStat::select()
{
    refresh(currentIdReq);
}
void ModelStat::setQuery(QString q)
{
    strQuery=q;
}
