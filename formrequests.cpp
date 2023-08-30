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
    mapper->addEmptyLock(ui->tableViewEl);
    mapper->addEmptyLock(ui->tableViewWire);
    mapper->addEmptyLock(ui->tableViewChanges);
    mapper->addEmptyLock(ui->pushButtonLoad);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->pushButtonUpd,&QPushButton::clicked,this,&FormRequests::updReq);
    connect(mapper,&DbMapper::currentIndexChanged,this,&FormRequests::updData);
    connect(modelReqEl,&ModelReqEl::sigSum,ui->labelElSum,&QLabel::setText);
    connect(modelReqWire,&ModelReqWire::sigSum,ui->labelWireSum,&QLabel::setText);

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
    modelReq->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormRequests::updData(int index)
{
    int id_req=mapper->modelData(index,0).toInt();
    modelReqEl->refresh(id_req);
    modelReqWire->refresh(id_req);

    modelChanges->setFilter("requests_changes.id_req="+QString::number(id_req));
    modelChanges->setDefaultValue(1,id_req);
    modelChanges->select();
}

ModelReq::ModelReq(QWidget *parent) : DbTableModel("requests",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_rec",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_cat",tr("Представительство"),Models::instance()->relKat);
    addColumn("comment",tr("Комментарий"));
    setSort(QString("%1.dat, %2.num").arg(name()).arg(name()));
}

void ModelReq::refresh(QDate beg, QDate end)
{
    QString filter=name()+".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    setFilter(filter);
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

ModelReqEl::ModelReqEl(QWidget *parent) : DbTableModel("requests_el",parent)
{
    addColumn("id_req",tr("id_req"));
    addColumn("id_el",tr("Марка"),Models::instance()->relElrtr);
    addColumn("id_diam",tr("Диам."),Models::instance()->relDiam);
    addColumn("id_pack",tr("Упаковка"),Models::instance()->relElPack);
    addColumn("id_var",tr("Вариант"),Models::instance()->relVars);
    addColumn("dat_term",tr("Срок"));
    addColumn("plan",tr("План"));
    addColumn("corr",tr("Коррект."));
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
    addColumn("corr",tr("Коррект."));
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
