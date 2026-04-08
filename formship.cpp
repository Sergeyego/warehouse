#include "formship.h"
#include "ui_formship.h"

FormShip::FormShip(bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShip)
{
    ui->setupUi(this);
    loadsettings();

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
    ui->tableViewBal->setModel(modelBalance);
    ui->tableViewBal->setColumnHidden(13,true);
    ui->tableViewBal->setColumnHidden(14,true);

    modelShip = new ModelShip(this);
    ui->tableViewShip->setModel(modelShip);
    ui->tableViewShip->setColumnHidden(0,true);
    ui->tableViewShip->setColumnWidth(1,55);
    ui->tableViewShip->setColumnWidth(2,70);
    ui->tableViewShip->setColumnWidth(3,330);
    for (int i=4; i<modelShip->columnCount(); i++){
        ui->tableViewShip->setColumnHidden(i,true);
    }

    push = new DbMapper(ui->tableViewShip,this);
    ui->horizontalLayoutPush->insertWidget(0,push);
    push->addMapping(ui->lineEditNum,1);
    push->addMapping(ui->dateEdit,2);
    push->addMapping(ui->comboBoxPol,3);
    push->addMapping(ui->comboBoxType,4);
    push->addMapping(ui->lineEditPrim,5);
    push->setDefaultFocus(3);
    push->addEmptyLock(ui->tableViewEl);
    push->addEmptyLock(ui->tableViewWire);
    push->addEmptyLock(ui->pushButton1C);
    push->addEmptyLock(ui->pushButtonEdt);
    push->addEmptyLock(ui->pushButtonNakl);
    push->addLock(ui->cmdUpdShip);
    push->addLock(ui->checkBoxOnly);
    push->addLock(ui->comboBoxOnly);

    ui->comboBoxOnly->setModel(Models::instance()->relPol->model());

    DbSqlRelation *relElPart = new DbSqlRelation("parti","id","str",this);
    relElPart->setSort("parti.dat_part desc, parti.n_s desc");
    relElPart->setFilter("parti.id<>0");
    relElPart->setFilterColumn("ids");
    relElPart->model()->setLimit(4000);

    shipContInfo ei;
    ei.tablename="ship_plan_el";
    ei.namId="id";
    ei.namIdDoc="id_sert";
    ei.namKis="kis";
    ei.namIdPart="id_part";
    ei.namKvo="massa";
    ei.modelBalence=modelBalance;
    ei.prefix="e";
    ei.relPart = relElPart;
    modelShipEl = new ModelShipData(ei,this);
    ui->tableViewEl->setModel(modelShipEl);
    ui->tableViewEl->setColumnHidden(0,true);
    ui->tableViewEl->setColumnHidden(1,true);
    ui->tableViewEl->setColumnWidth(2,200);
    ui->tableViewEl->setColumnWidth(3,350);
    ui->tableViewEl->setColumnWidth(4,100);

    DbSqlRelation *relWirePart = new DbSqlRelation("wire_parti","id","str",this);
    relWirePart->setSort("str desc");
    relWirePart->setFilter("wire_parti.id<>0");
    relWirePart->setFilterColumn("ids");
    relWirePart->model()->setLimit(4000);

    shipContInfo wi;
    wi.tablename="ship_plan_wire";
    wi.namId="id";
    wi.namIdDoc="id_ship";
    wi.namKis="kis";
    wi.namIdPart="id_wparti";
    wi.namKvo="m_netto";
    wi.modelBalence=modelBalance;
    wi.prefix="w";
    wi.relPart = relWirePart;
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

    connect(modelShipEl, SIGNAL(sigStock(QString)),ui->labelEl,SLOT(setText(QString)));
    connect(modelShipWire, SIGNAL(sigStock(QString)),ui->labelWire,SLOT(setText(QString)));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
    connect(ui->tableViewEl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(ui->tableViewWire->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(ui->pushButtonEdt,SIGNAL(clicked(bool)),this,SLOT(updBalance()));
    connect(push,SIGNAL(sigWrite()),this,SLOT(updBalance()));
    connect(modelShipEl, SIGNAL(sigSum(QString)),ui->labelSumEl,SLOT(setText(QString)));
    connect(modelShipWire, SIGNAL(sigSum(QString)),ui->labelSumWire,SLOT(setText(QString)));

    connect(modelShipEl,SIGNAL(sigUpd()),this,SLOT(updShipStatisticEl()));
    connect(modelShipEl,SIGNAL(sigRefresh()),this,SLOT(updShipStatisticEl()));

    connect(modelShipWire,SIGNAL(sigUpd()),this,SLOT(updShipStatisticWire()));
    connect(modelShipWire,SIGNAL(sigRefresh()),this,SLOT(updShipStatisticWire()));

    connect(modelBalance,SIGNAL(sigUpd()),ui->tableViewBal,SLOT(resizeToContents()));

    updPol();
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
    if ((this->sender()==ui->comboBoxOnly && ui->checkBoxOnly->isChecked() && ui->comboBoxOnly->currentIndex()>=0) || (this->sender()!=ui->comboBoxOnly)){
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
    int id_pol=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,3),Qt::EditRole).toInt();

    ui->lineEditPol->setText(Models::instance()->relPol->getDisplayValue(id_pol,"naim"));

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

void FormShip::printNakl()
{
    const int id_ship = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),0),Qt::EditRole).toInt();

    DialogWebView d;
    if (d.sendGetReq("packnakl/shipnakl/"+QString::number(id_ship))){
        d.exec();
    }
}

void FormShip::setPartFilter()
{
    int index=ui->comboBoxPart->currentIndex();
    modelShipEl->setPartFlt(index);
    modelShipWire->setPartFlt(index);
}

void FormShip::updPol()
{
    if (sender()==ui->cmdUpdShip){
        modelShip->refreshRelsModel();
        modelShipEl->refreshRelsModel();
        modelShipWire->refreshRelsModel();
    }
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
    modelShipEl->refreshRelsModel();
    modelShipWire->refreshRelsModel();
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

void FormShip::calcStat(ModelShipData *modelShipData, TableModel *modelStat)
{
    QVector <QVector<QVariant>> data;
    QMultiMap<QString, double> hash;
    for (int i=0; i<modelShipData->rowCount(); i++){
        QVariant id_part=modelShipData->data(modelShipData->index(i,3),Qt::EditRole);
        if (id_part.toInt()>0){
            QString nom=modelShipData->data(modelShipData->index(i,2),Qt::DisplayRole).toString();
            QString part=modelShipData->data(modelShipData->index(i,3),Qt::DisplayRole).toString();
            QRegularExpression reg(QString::fromUtf8("^.*\\((.*)\\).*$"));
            reg.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
            QRegularExpressionMatch match = reg.match(part);
            if (match.hasMatch()){
                nom+=" ("+match.captured(1)+")";
            }
            double kvo=modelShipData->data(modelShipData->index(i,4),Qt::EditRole).toDouble();
            hash.insert(nom,kvo);
        }
    }
    for (QString &key : hash.uniqueKeys()){
        double sum=0;
        QList<double> vals = hash.values(key);
        for (double &s : vals){
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
    DbSqlRelation *relType = new DbSqlRelation("sert_type","id","nam",this);
    relType->setFilter("sert_type.id in (1,3)");
    addColumn("id",tr("id"));
    addColumn("nom_s",tr("Номер"));
    addColumn("dat_vid",tr("Дата"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),relType);
    addColumn("prim",tr("Примечание"));
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
    QString num = QString::number(old_num+1);
    num=num.rightJustified(4,'0',true);
    setDefaultValue(1,num);
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

ModelBalance::ModelBalance(QObject *parent) : QSortFilterProxyModel (parent)
{
    srcModel = new TableModel(this);
    QStringList head;
    head<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рец./плавка"<<"Коммент."<<"Кол-во, кг"<<"План прих., кг"<<"План расх., кг"<<"Зона"<<"Ячейка"<<"Поддон"<<"Год"<<"id_part"<<"id_kis";
    srcModel->setHeader(head);
    this->setSourceModel(srcModel);
    this->setSortRole(Qt::EditRole);
    this->setFilterCaseSensitivity(Qt::CaseSensitive);
    this->setFilterKeyColumn(14);
    this->sort(12);
}

void ModelBalance::updData(QDate dat)
{
    QByteArray data;
    bool ok = HttpSyncManager::sendGet("/wms/balance/"+dat.toString("yyyy-MM-dd")+"?getall=false",data);
    if (ok){
        QJsonDocument respDoc = QJsonDocument::fromJson(data);
        if (respDoc.isArray()){
            QVector<QVector<QVariant>> tmpd;
            QJsonArray arr = respDoc.array();
            for (const QJsonValue &v : qAsConst(arr)){
                QVector<QVariant> row;
                QJsonObject o = v.toObject();
                QString part=o.value("part").toString();
                row.push_back(o.value("name").toString());
                row.push_back(o.value("pack").toString());
                row.push_back(part);
                row.push_back(o.value("ist").toString());
                row.push_back(o.value("rcpplav").toString());
                row.push_back(o.value("prim").toString());
                row.push_back(o.value("kvo").toDouble());
                row.push_back(o.value("prich").toDouble());
                row.push_back(o.value("rasch").toDouble());
                row.push_back(o.value("zone").toString());
                row.push_back(o.value("cell").toString());
                row.push_back(o.value("cont").toString());
                row.push_back(part.right(4)+'-'+part.left(4));
                row.push_back(o.value("prefix").toString()+":"+QString::number(o.value("id_part").toInt()));
                row.push_back(o.value("id_kis").toString());
                tmpd.push_back(row);
            }
            srcModel->setModelData(tmpd);
        }
        this->refresh("");
    }
}

double ModelBalance::getStock(QString ide)
{
    double kvo=0;
    QVector<QVector<QVariant>> data = srcModel->getData();
    for (QVector<QVariant> &row : data){
        if (row.at(13).toString()==ide){
            kvo+=(row.at(6).toDouble()-row.at(8).toDouble());
        }
    }
    return kvo;
}

void ModelBalance::clear()
{
    srcModel->clear();
    emit sigUpd();
}

void ModelBalance::refresh(QString kis)
{
    this->setFilterRegularExpression("^"+QRegularExpression::escape(kis)+"$");
    emit sigUpd();
}

ModelShipData::ModelShipData(shipContInfo c, QObject *parent) : DbTableModel(c.tablename,parent)
{
    info=c;
    fltind=1;
    ostControl=true;
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
        QString s=this->data(this->index(index.row(),3),Qt::DisplayRole).toString();
        QRegularExpression reg(QString::fromUtf8("^.*\\s\\(\\S*?\\s*(\\d*[//.//,]*\\d)\\sкг.*\\).*$"));
        reg.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpressionMatch match = reg.match(s);
        double mas_ed=0;
        if (match.hasMatch()){
            QString str_mas_ed = match.captured(1);
            str_mas_ed=str_mas_ed.replace(",",".");
            mas_ed=str_mas_ed.toDouble();
        }
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
    info.relPart->setFilterRegExp(pattern);
}

bool ModelShipData::insertRow(int row, const QModelIndex &parent)
{
    QString oldkis;
    if (rowCount()>0){
        oldkis=this->data(this->index(rowCount()-1,2),Qt::EditRole).toString();
    }
    setDefaultValue(2,oldkis);
    setFlt(oldkis);
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
    QString title = info.prefix==("e")? tr("Электроды") : tr("Проволока");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,4),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
