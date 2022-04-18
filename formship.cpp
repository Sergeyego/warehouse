#include "formship.h"
#include "ui_formship.h"

FormShip::FormShip(bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShip)
{
    ui->setupUi(this);
    loadsettings();

    ui->cmdUpdPart->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdUpdShip->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));
    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    QStringList listStatHeader;
    listStatHeader<<"Номенклатура"<<"Масса, кг";

    modelElStat = new TableModel(this);
    modelElStat->setHeader(listStatHeader);
    modelElStat->setDecimal(2);
    ui->tableViewElStat->setModel(modelElStat);

    modelWireStat = new TableModel(this);
    modelWireStat->setHeader(listStatHeader);
    modelWireStat->setDecimal(2);
    ui->tableViewWireStat->setModel(modelWireStat);

    modelBalance = new ModelBalance(this);
    proxyModelBalance = new QSortFilterProxyModel(this);
    proxyModelBalance->setSourceModel(modelBalance);
    proxyModelBalance->sort(2);
    ui->tableViewBal->setModel(proxyModelBalance);

    modelShip = new ModelShip(this);
    modelShip->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    ui->tableViewShip->setModel(modelShip);
    ui->tableViewShip->setColumnHidden(0,true);
    ui->tableViewShip->setColumnWidth(1,55);
    ui->tableViewShip->setColumnWidth(2,70);
    ui->tableViewShip->setColumnWidth(3,350);
    ui->tableViewShip->setColumnHidden(4,true);

    push = new DbMapper(ui->tableViewShip,this);
    ui->horizontalLayoutPush->insertWidget(0,push);
    push->addMapping(ui->lineEditNum,1);
    push->addMapping(ui->dateEdit,2);
    push->addMapping(ui->comboBoxPol,3);
    push->addMapping(ui->comboBoxType,4);
    push->setDefaultFocus(3);
    push->addEmptyLock(ui->tableViewEl);
    push->addEmptyLock(ui->tableViewWire);
    push->addEmptyLock(ui->pushButton1C);
    push->addEmptyLock(ui->pushButtonEdt);
    push->addLock(ui->cmdUpdShip);
    push->addLock(ui->checkBoxOnly);
    push->addLock(ui->comboBoxOnly);

    ui->comboBoxOnly->setModel(Models::instance()->relPol->model());
    ui->comboBoxOnly->setModelColumn(Models::instance()->relPol->columnDisplay());
    ui->comboBoxOnly->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->comboBoxOnly->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    shipContInfo ei;
    ei.tablename="ship_plan_el";
    ei.namId="id";
    ei.namIdDoc="id_sert";
    ei.namKis="kis";
    ei.namIdPart="id_part";
    ei.namKvo="massa";
    ei.modelBalence=modelBalance;
    ei.prefix="e";
    ei.relPart = new DbRelation(Models::instance()->modelElPart,0,1,this);
    modelShipEl = new ModelShipData(ei,this);
    ui->tableViewEl->setModel(modelShipEl);
    ui->tableViewEl->setColumnHidden(0,true);
    ui->tableViewEl->setColumnHidden(1,true);
    ui->tableViewEl->setColumnWidth(2,200);
    ui->tableViewEl->setColumnWidth(3,350);
    ui->tableViewEl->setColumnWidth(4,100);

    shipContInfo wi;
    wi.tablename="ship_plan_wire";
    wi.namId="id";
    wi.namIdDoc="id_ship";
    wi.namKis="kis";
    wi.namIdPart="id_wparti";
    wi.namKvo="m_netto";
    wi.modelBalence=modelBalance;
    wi.prefix="w";
    wi.relPart = new DbRelation(Models::instance()->modelWirePart,0,1,this);
    modelShipWire = new ModelShipData(wi, this);
    ui->tableViewWire->setModel(modelShipWire);
    ui->tableViewWire->setColumnHidden(0,true);
    ui->tableViewWire->setColumnHidden(1,true);
    ui->tableViewWire->setColumnWidth(2,200);
    ui->tableViewWire->setColumnWidth(3,350);
    ui->tableViewWire->setColumnWidth(4,100);

    if (readonly){
        ui->tableViewEl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewWire->setEditTriggers(QAbstractItemView::NoEditTriggers);
        push->setEnabled(false);
        ui->tableViewShip->setMenuEnabled(false);
        ui->tableViewEl->setMenuEnabled(false);
        ui->tableViewWire->setMenuEnabled(false);
    }

    connect(ui->checkBoxOst,SIGNAL(clicked(bool)),modelShipEl,SLOT(setOstControl(bool)));
    connect(ui->checkBoxOst,SIGNAL(clicked(bool)),modelShipWire,SLOT(setOstControl(bool)));

    connect(ui->cmdUpdShip,SIGNAL(clicked(bool)),this,SLOT(updPol()));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),this,SLOT(updShip()));
    connect(ui->comboBoxOnly,SIGNAL(currentIndexChanged(int)),this,SLOT(updShip()));
    connect(push,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentShip(int)));
    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));

    connect(ui->cmdUpdPart,SIGNAL(clicked(bool)),Models::instance()->relElPart->model(),SLOT(refresh()));
    connect(ui->cmdUpdPart,SIGNAL(clicked(bool)),Models::instance()->relWirePart->model(),SLOT(refresh()));

    connect(modelShipEl, SIGNAL(sigStock(QString)),ui->labelEl,SLOT(setText(QString)));
    connect(modelShipWire, SIGNAL(sigStock(QString)),ui->labelWire,SLOT(setText(QString)));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->tableViewEl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(ui->tableViewWire->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(modelBalance,SIGNAL(sigUpd()),ui->tableViewBal,SLOT(resizeToContents()));
    connect(ui->pushButtonEdt,SIGNAL(clicked(bool)),this,SLOT(updBalance()));
    connect(push,SIGNAL(sigWrite()),this,SLOT(updBalance()));
    connect(modelShipEl, SIGNAL(sigSum(QString)),ui->labelSumEl,SLOT(setText(QString)));
    connect(modelShipWire, SIGNAL(sigSum(QString)),ui->labelSumWire,SLOT(setText(QString)));

    connect(modelShipEl,SIGNAL(sigUpd()),this,SLOT(updShipStatisticEl()));
    connect(modelShipEl,SIGNAL(sigRefresh()),this,SLOT(updShipStatisticEl()));

    connect(modelShipWire,SIGNAL(sigUpd()),this,SLOT(updShipStatisticWire()));
    connect(modelShipWire,SIGNAL(sigRefresh()),this,SLOT(updShipStatisticWire()));

    push->last();
}

FormShip::~FormShip()
{
    savesettings();
    delete ui;
}

void FormShip::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("ship_splitter_width").toByteArray());
    ui->splitterEl->restoreState(settings.value("ship_splitter_el_width").toByteArray());
    ui->splitterWire->restoreState(settings.value("ship_splitter_wire_width").toByteArray());
}

void FormShip::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ship_splitter_width",ui->splitter->saveState());
    settings.setValue("ship_splitter_el_width",ui->splitterEl->saveState());
    settings.setValue("ship_splitter_wire_width",ui->splitterWire->saveState());
}

void FormShip::updShip()
{  
    if ((this->sender()==ui->comboBoxOnly && ui->checkBoxOnly->isChecked()) || (this->sender()!=ui->comboBoxOnly)){
        int id_pol=-1;
        if (ui->checkBoxOnly->isChecked()){
            id_pol=ui->comboBoxOnly->model()->data(ui->comboBoxOnly->model()->index(ui->comboBoxOnly->currentIndex(),0),Qt::EditRole).toInt();
        }
        modelShip->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_pol);
    }
}

void FormShip::setCurrentShip(int index)
{
    ui->pushButtonEdt->setEnabled(true);
    modelBalance->clear();

    int id_ship=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,0),Qt::EditRole).toInt();
    QString id_pol=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,3),Qt::EditRole).toString();

    ui->lineEditPol->setText(Models::instance()->relPol->data(id_pol,2).toString());

    modelShipEl->refresh(id_ship);
    modelShipWire->refresh(id_ship);
    ui->tableViewEl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewWire->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void FormShip::sync()
{
    const int id_ship = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),0),Qt::EditRole).toInt();
    Models::instance()->sync1C->syncShip(id_ship);
}

void FormShip::setPartFilter()
{
    int index=ui->comboBoxPart->currentIndex();
    Models::instance()->setFilter(index);
    modelShipEl->setPartFlt(index);
    modelShipWire->setPartFlt(index);
}

void FormShip::updPol()
{
    ui->comboBoxOnly->blockSignals(true);
    Models::instance()->relPol->refreshModel();
    ui->comboBoxOnly->blockSignals(false);
    updShip();
}

void FormShip::updBalance()
{
    const QDate date = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),2),Qt::EditRole).toDate();
    QAbstractItemView::EditTriggers editTrig=QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::AnyKeyPressed;
    ui->tableViewEl->setEditTriggers(editTrig);
    ui->tableViewWire->setEditTriggers(editTrig);
    modelBalance->updData(date);
    ui->pushButtonEdt->setEnabled(false);
    modelShipEl->setFlt("");
    modelShipWire->setFlt("");
}

void FormShip::updKisBalance(QModelIndex ind)
{
    const QAbstractItemModel *m = ind.model();
    QString kis;
    if (ind.isValid()){
        kis = m->data(m->index(ind.row(),2),Qt::EditRole).toString();
    }
    modelBalance->refresh(kis);
}

void FormShip::updShipStatisticEl()
{
    calcStat(modelShipEl,modelElStat);
    ui->tableViewElStat->resizeToContents();
}

void FormShip::updShipStatisticWire()
{
    calcStat(modelShipWire,modelWireStat);
    ui->tableViewWireStat->resizeToContents();
}

QDomElement FormShip::newElement(QString nam, QString val, QDomDocument *doc)
{
    QDomElement l = doc->createElement(nam);
    l.appendChild(doc->createTextNode(val));
    return l;
}

void FormShip::calcStat(ModelShipData *modelShipData, TableModel *modelStat)
{
    QVector <QVector<QVariant>> data;
    QMultiMap<QString, double> hash;
    for (int i=0; i<modelShipData->rowCount(); i++){
        QString nom=modelShipData->data(modelShipData->index(i,2),Qt::DisplayRole).toString();
        double kvo=modelShipData->data(modelShipData->index(i,4),Qt::EditRole).toDouble();
        hash.insert(nom,kvo);
    }
    for (QString key : hash.uniqueKeys()){
        double sum=0;
        QList<double> vals = hash.values(key);
        for (double s : vals){
            sum+=s;
        }
        QVector<QVariant> v;
        v.push_back(key);
        v.push_back(sum);
        data.push_back(v);
    }
    modelStat->setModelData(data);
}

ModelShip::ModelShip(QObject *parent) : DbTableModel("ship_plan",parent)
{
    addColumn("id",tr("id"));
    addColumn("nom_s",tr("Номер"));
    addColumn("dat_vid",tr("Дата"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),Models::instance()->relShipType);
    setSort("ship_plan.dat_vid, ship_plan.nom_s");
    setDefaultValue(4,1);
}

void ModelShip::refresh(QDate beg, QDate end, int id_pol)
{
    QString filter="ship_plan.dat_vid between '"+beg.toString("yyyy-MM-dd")+"' and '"
            +end.toString("yyyy-MM-dd")+"'";
    if (id_pol!=-1){
        filter+=" and ship_plan.id_pol = "+QString::number(id_pol);
    }
    this->setFilter(filter);
    this->select();
}

bool ModelShip::insertRow(int row, const QModelIndex &parent)
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

ModelBalance::ModelBalance(QObject *parent) : TableModel(parent)
{
    QStringList head;
    head<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рец./плавка"<<"Коммент."<<"Кол-во, кг"<<"План прих., кг"<<"План расх., кг"<<"Зона"<<"Ячейка"<<"Поддон";
    setHeader(head);
}

void ModelBalance::updData(QDate dat)
{
    Models::instance()->sync1C->getBalance(dat,part);
    Models::instance()->sync1C->getContBalance(dat,cont);
}

double ModelBalance::getStock(QString ide)
{
    double kvo=0;
    QStringList zoneOt=Models::instance()->sync1C->getZoneOt();
    QMultiHash<QString, partInfo>::const_iterator i = part.constBegin();
    while (i != part.constEnd()) {
        partInfo pinfo=i.value();
        contInfo cnt = cont.value(pinfo.contKey);
        if (zoneOt.contains(cnt.zone) && pinfo.id_part_kis==ide){
            kvo+=(pinfo.kvo-pinfo.rasch);
        }
        ++i;
    }
    return kvo;
}

void ModelBalance::clear()
{
    part.clear();
    cont.clear();
    TableModel::clear();
}

QString ModelBalance::getPackName(QString id_part_kis)
{
    QStringList idpl = id_part_kis.split(":");
    QString pack;
    if (idpl.size()>1){
        QString id_part=idpl.at(1);
        if (idpl.at(0)=="w"){
            pack=Models::instance()->relWirePart->data(id_part,3).toString();
        } else if (idpl.at(0)=="e"){
            pack=Models::instance()->relElPart->data(id_part,3).toString();
        }
    }
    return pack;
}

QString ModelBalance::getDesc(QString id_part_kis, QString defval)
{
    QStringList idpl = id_part_kis.split(":");
    QString desc=defval;
    if (idpl.size()>1){
        QString id_part=idpl.at(1);
        if (idpl.at(0)=="e"){
            QString prim=Models::instance()->relElPart->data(id_part,4).toString();
            if (!prim.isEmpty()){
                desc=prim;
            }
        }
    }
    return desc;
}

void ModelBalance::refresh(QString kis)
{
    QVector<QVector<QVariant>> tmpd;
    QList<partInfo> list = part.values(kis);
    QStringList zoneOt=Models::instance()->sync1C->getZoneOt();
    for (partInfo i : list){
        QVector<QVariant> row;
        contInfo cnt = cont.value(i.contKey);        
        if (zoneOt.contains(cnt.zone)){
            row.push_back(i.name);
            row.push_back(getPackName(i.id_part_kis));
            row.push_back(i.number);
            row.push_back(i.ist);
            row.push_back(i.rcp);
            row.push_back(getDesc(i.id_part_kis,i.desc));
            row.push_back(i.kvo);
            row.push_back(i.prich);
            row.push_back(i.rasch);
            row.push_back(cnt.zone);
            row.push_back(cnt.cell);
            row.push_back(cnt.name);
            tmpd.push_back(row);
        }
    }
    setModelData(tmpd);
}

ModelShipData::ModelShipData(shipContInfo c, QObject *parent) : DbTableModel(c.tablename,parent)
{
    info=c;
    fltind=1;
    ostControl=true;
    info.relPart->proxyModel()->setFilterKeyColumn(2);
    setFlt("");
    addColumn(info.namId,tr("id"));
    addColumn(info.namIdDoc, tr("id_sert"));
    addColumn(info.namKis,tr("Номенклатура"),Models::instance()->relKis);
    addColumn(info.namIdPart,tr("Партия"),info.relPart);
    addColumn(info.namKvo,tr("Масса, кг"));
    setSort(info.tablename+"."+info.namId);
    currentIdShip=-1;
    setDecimals(4,2);
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}


void ModelShipData::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter(info.tablename+"."+info.namIdDoc+" = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    setDefaultValue(2,QString());
    select();
}

QVariant ModelShipData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        QString id_part=this->data(this->index(index.row(),3),Qt::EditRole).toString();
        double mas_ed=info.relPart->data(id_part,5).toDouble();
        if (mas_ed>0){
            double kvo=this->data(this->index(index.row(),4),Qt::EditRole).toDouble();
            double b;
            double ost=modf(kvo/mas_ed, &b);
            return ost>0 ? QColor(255,170,170): QColor(255,255,255);
        }
    }
    return DbTableModel::data(index,role);
}

bool ModelShipData::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole) {
        if (index.column()==2){
            QString kis=value.toString();
            info.modelBalence->refresh(kis);
            setFlt(kis);
        } else if (index.column()==3){
            emit sigStock(tr("Остаток на день отгрузки: ")+QString::number(getStock(index))+tr(" кг"));
        }
    }
    return ok;
}

bool ModelShipData::submit()
{
    bool ok=false;
    if (ostControl){
        if (this->isEdt()){
            double kvo=this->data(this->index(currentEdtRow(),4),Qt::EditRole).toDouble();
            double m=getStock(this->index(currentEdtRow(),3));
            if (kvo>=0 && m>=kvo){
                ok=DbTableModel::submit();
            } else {
                QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день отгрузки числится ")+
                                      QLocale().toString(m,'f',2)+tr(" кг номенклатуры этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
            }
        } else {
            ok=DbTableModel::submit();
        }
    } else {
        ok=DbTableModel::submit();
    }
    if (ok) {
        emit sigStock("");
    }
    return ok;
}

void ModelShipData::revert()
{
    emit sigStock("");
    return DbTableModel::revert();
}

void ModelShipData::setFlt(QString kis)
{
    int year=QDate::currentDate().year();
    QString pattern;
    QString fkis = !kis.isEmpty() ? kis+"-" : "";
    if (fltind==0){
        pattern=fkis+QString::number(year);
    } else if (fltind==1){
        pattern=fkis+QString::number(year-1)+"|"+fkis+QString::number(year);
    } else {
        pattern=kis;
    }
    info.relPart->proxyModel()->setFilterRegExp(pattern);
}

bool ModelShipData::insertRow(int row, const QModelIndex &parent)
{
    QString oldkis;
    if (rowCount()>0){
        oldkis=this->data(this->index(rowCount()-1,2),Qt::EditRole).toString();
    }
    setDefaultValue(2,oldkis);
    return DbTableModel::insertRow(row,parent);
}

double ModelShipData::getStock(QModelIndex index)
{
    int id_part = this->data(this->index(index.row(),3),Qt::EditRole).toInt();
    double plan=0;
    for (int i=0; i<rowCount(); i++){
        if (this->data(this->index(i,3),Qt::EditRole).toInt()==id_part && i!=index.row()){
            plan+=this->data(this->index(i,4),Qt::EditRole).toDouble();
        }
    }
    return info.modelBalence->getStock(info.prefix+":"+QString::number(id_part))-plan;
}


void ModelShipData::setPartFlt(int ind)
{
    fltind=ind;
    setFlt("");
}

void ModelShipData::setOstControl(bool b)
{
    ostControl=b;
}

void ModelShipData::calcSum()
{
    double sum=0;
    QString title = info.prefix==("e")? "Электроды" : "Проволока";
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,4),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
