#include "formshipcons.h"
#include "ui_formshipcons.h"

FormShipCons::FormShipCons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShipCons)
{
    ui->setupUi(this);
    loadsettings();
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxPolFlt->setModel(Models::instance()->relPol->model());

    modelShip = new ModelShipCons(this);
    ui->tableViewShip->setModel(modelShip);
    ui->tableViewShip->setColumnHidden(0,true);
    ui->tableViewShip->setColumnWidth(1,55);
    ui->tableViewShip->setColumnWidth(2,70);
    ui->tableViewShip->setColumnWidth(3,300);
    for (int i=4; i<modelShip->columnCount(); i++){
        ui->tableViewShip->setColumnHidden(i,true);
    }

    modelEl = new ModelShipConsEl(this);
    ui->tableViewEl->setModel(modelEl);
    ui->tableViewEl->setColumnHidden(0,true);
    ui->tableViewEl->setColumnHidden(1,true);
    ui->tableViewEl->setColumnWidth(2,400);
    ui->tableViewEl->setColumnWidth(3,100);

    modelWire = new ModelShipConsWire(this);
    ui->tableViewWire->setModel(modelWire);
    ui->tableViewWire->setColumnHidden(0,true);
    ui->tableViewWire->setColumnHidden(1,true);
    ui->tableViewWire->setColumnWidth(2,350);
    ui->tableViewWire->setColumnWidth(3,100);

    mapper = new DbMapper(ui->tableViewShip,this);
    ui->horizontalLayoutMapper->insertWidget(1,mapper);

    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxPol,3);
    mapper->addMapping(ui->comboBoxType,4);
    mapper->setDefaultFocus(3);
    mapper->addEmptyLock(ui->tableViewEl);
    mapper->addEmptyLock(ui->tableViewWire);
    mapper->addEmptyLock(ui->pushButtonCods);
    mapper->addEmptyLock(ui->pushButtonXML);
    mapper->addEmptyLock(ui->pushButtonLoad);
    mapper->addLock(ui->pushButtonUpd);
    mapper->addLock(ui->checkBoxOnly);
    mapper->addLock(ui->comboBoxPolFlt);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updPol()));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),this,SLOT(updShip()));
    connect(ui->comboBoxPolFlt,SIGNAL(currentIndexChanged(int)),this,SLOT(updShip()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentShip(int)));
    connect(modelEl,SIGNAL(sigStock(QString)),ui->labelStatEl,SLOT(setText(QString)));
    connect(modelEl,SIGNAL(sigSum(QString)),ui->labelEl,SLOT(setText(QString)));
    connect(modelWire,SIGNAL(sigStock(QString)),ui->labelStatWire,SLOT(setText(QString)));
    connect(modelWire,SIGNAL(sigSum(QString)),ui->labelWire,SLOT(setText(QString)));
    connect(ui->pushButtonCods,SIGNAL(clicked(bool)),this,SLOT(edtCods()));
    connect(ui->pushButtonXML,SIGNAL(clicked(bool)),this,SLOT(goXml()));
    connect(ui->pushButtonLoad,SIGNAL(clicked(bool)),this,SLOT(loadData()));

    updPol();
}

FormShipCons::~FormShipCons()
{
    savesettings();
    delete ui;
}

void FormShipCons::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("ship_cons_splitter_width").toByteArray());
}

void FormShipCons::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ship_cons_splitter_width",ui->splitter->saveState());
}

QDomElement FormShipCons::newElement(QString nam, QString val, QDomDocument *doc)
{
    QDomElement l = doc->createElement(nam);
    l.appendChild(doc->createTextNode(val));
    return l;
}

void FormShipCons::updShip()
{
    if ((this->sender()==ui->comboBoxPolFlt && ui->checkBoxOnly->isChecked() && ui->comboBoxPolFlt->currentIndex()>=0) || (this->sender()!=ui->comboBoxPolFlt)){
        int id_pol=-1;
        if (ui->checkBoxOnly->isChecked()){
            id_pol=ui->comboBoxPolFlt->model()->data(ui->comboBoxPolFlt->model()->index(ui->comboBoxPolFlt->currentIndex(),0),Qt::EditRole).toInt();
        }
        modelShip->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_pol);
    }
}

void FormShipCons::updPol()
{
    if (sender()==ui->pushButtonUpd){
        modelShip->refreshRelsModel();
        modelEl->refreshRelsModel();
        modelWire->refreshRelsModel();
    }
    updShip();
}

void FormShipCons::setCurrentShip(int index)
{
    int id_ship=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,0),Qt::EditRole).toInt();
    QString id_pol=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,3),Qt::EditRole).toString();
    ui->lineEditPol->setText(Models::instance()->relPol->getDisplayValue(id_pol,"snam"));
    modelEl->refresh(id_ship);
    modelWire->refresh(id_ship);
}

void FormShipCons::edtCods()
{
    DialogCods d;
    d.exec();
    modelEl->refreshState();
    modelWire->refreshState();
}

void FormShipCons::goXml()
{
    QSqlQuery query;
    const int id_ship = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),0),Qt::EditRole).toInt();
    const QString num = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),1),Qt::EditRole).toString();
    const QDate date = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),2),Qt::EditRole).toDate();
    const int id_pol = modelShip->data(modelShip->index(ui->tableViewShip->currentIndex().row(),3),Qt::EditRole).toInt();
    int i=1;
    QString namPol, codPol, inn;
#if defined(Q_OS_WIN)
    QDir dir("C:/rotex");
#else
    QDir dir(QDir::homePath()+"/xml");
#endif
    if (!dir.exists()) dir.mkdir(dir.path());
    QFile file(dir.path()+"/srtf_"+num+"_"+date.toString("ddMMyyyy")+".xml");

    query.clear();
    query.prepare("Select naim, ch_id, substring(innkpp from '\\m\\d*') from poluch where id = :id_pol");
    query.bindValue(":id_pol",id_pol);
    if (query.exec()){
        while (query.next()){
            namPol=query.value(0).toString();
            codPol=query.value(1).toString();
            inn=query.value(2).toString();
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }

    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""));
    QDomElement root = doc.createElement(QString::fromUtf8("root"));
    doc.appendChild(root);
    QDomElement rec = doc.createElement(QString::fromUtf8("РеквизитыШапки"));
    rec.appendChild(newElement(QString::fromUtf8("КодОпер"),QString::fromUtf8("РеализацияТоваровУслуг"),&doc));
    rec.appendChild(newElement(QString::fromUtf8("НомерДок"),num,&doc));
    rec.appendChild(newElement(QString::fromUtf8("ДатаДок"),date.toString("dd.MM.yyyy"),&doc));
    root.appendChild(rec);
    QDomElement podr = doc.createElement(QString::fromUtf8("Подразделение"));
    podr.appendChild(newElement(QString::fromUtf8("Код"),QString::fromUtf8("00010002"),&doc));
    podr.appendChild(newElement(QString::fromUtf8("Наименование"),QString::fromUtf8("Судиславль"),&doc));
    rec.appendChild(podr);
    QDomElement contr = doc.createElement(QString::fromUtf8("Контрагент"));
    contr.appendChild(newElement(QString::fromUtf8("Код"),codPol,&doc));
    contr.appendChild(newElement(QString::fromUtf8("Наименование"),namPol,&doc));
    contr.appendChild(newElement(QString::fromUtf8("ИНН"),inn,&doc));
    contr.appendChild(newElement(QString::fromUtf8("ВидДоговора"),QString::fromUtf8("СПокупателем"),&doc));
    contr.appendChild(newElement(QString::fromUtf8("КодДоговора"),QString::fromUtf8("00001"),&doc));
    contr.appendChild(newElement(QString::fromUtf8("НаименованиеДоговора"),QString::fromUtf8("Основной договор"),&doc));
    rec.appendChild(contr);
    QDomElement tov = doc.createElement(QString::fromUtf8("ТЧТовары"));
    root.appendChild(tov);

    query.clear();
    query.prepare("select e.marka||"+tr("' д-'")+"||d.sdim, sum(o.massa), ke.cod "
                                                 "from otpusk o "
                                                 "inner join parti p on o.id_part=p.id "
                                                 "inner join elrtr e on p.id_el=e.id "
                                                 "inner join diam d on d.diam=p.diam "
                                                 "left outer join td_keys_el as ke on ke.id_el=p.id_el and ke.id_diam=d.id and ke.id_pack=p.id_pack and ke.id_var=p.id_var "
                                                 "where o.id_sert = :id_sert "
                                                 "group by e.marka, d.sdim, ke.cod "
                                                 "order by e.marka, d.sdim");
    query.bindValue(":id_sert", id_ship);
    if (query.exec()){
        while (query.next()) {
            QDomElement stroka = doc.createElement(QString::fromUtf8("СтрокаТЧ"));
            stroka.appendChild(newElement(QString::fromUtf8("НомерСтроки"),QString::number(i),&doc));
            QDomElement nomen = doc.createElement(QString::fromUtf8("Номенклатура"));
            nomen.appendChild(newElement(QString::fromUtf8("Код"),query.value(2).toString(),&doc));
            nomen.appendChild(newElement(QString::fromUtf8("Наименование"),query.value(0).toString(),&doc));
            stroka.appendChild(nomen);
            QDomElement edizm = doc.createElement(QString::fromUtf8("ЕдИзм"));
            edizm.appendChild(newElement(QString::fromUtf8("Код"),QString::fromUtf8("168"),&doc));
            edizm.appendChild(newElement(QString::fromUtf8("Наименование"),QString::fromUtf8("тн."),&doc));
            stroka.appendChild(edizm);
            stroka.appendChild(newElement(QString::fromUtf8("Количество"),QString::number(query.value(1).toDouble()/1000.0,10,5),&doc));
            tov.appendChild(stroka);
            i++;
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }

    query.clear();
    query.prepare("select pr.nam||"+tr("' д-'")+"||d.sdim||' '||k.nam, sum(w.m_netto), kw.cod "
                                                "from wire_shipment_consist as w "
                                                "inner join wire_parti as p on p.id=w.id_wparti "
                                                "inner join wire_parti_m as m on p.id_m=m.id "
                                                "inner join provol as pr on pr.id=m.id_provol "
                                                "inner join diam as d on d.id=m.id_diam "
                                                "inner join wire_pack_kind as k on p.id_pack=k.id "
                                                "left outer join td_keys_wire as kw on kw.id_prov=m.id_provol and kw.id_diam=m.id_diam and kw.id_spool=p.id_pack and kw.id_pack=p.id_pack_type "
                                                "where w.id_ship= :id_ship "
                                                "group by pr.nam, d.sdim, k.nam, kw.cod "
                                                "order by pr.nam, d.sdim, k.nam");
    query.bindValue(":id_ship", id_ship);
    if (query.exec()){
        while (query.next()) {
            QDomElement stroka = doc.createElement(QString::fromUtf8("СтрокаТЧ"));
            stroka.appendChild(newElement(QString::fromUtf8("НомерСтроки"),QString::number(i),&doc));
            QDomElement nomen = doc.createElement(QString::fromUtf8("Номенклатура"));
            nomen.appendChild(newElement(QString::fromUtf8("Код"),query.value(2).toString(),&doc));
            nomen.appendChild(newElement(QString::fromUtf8("Наименование"),query.value(0).toString(),&doc));
            stroka.appendChild(nomen);
            QDomElement edizm = doc.createElement(QString::fromUtf8("ЕдИзм"));
            edizm.appendChild(newElement(QString::fromUtf8("Код"),QString::fromUtf8("168"),&doc));
            edizm.appendChild(newElement(QString::fromUtf8("Наименование"),QString::fromUtf8("тн."),&doc));
            stroka.appendChild(edizm);
            stroka.appendChild(newElement(QString::fromUtf8("Количество"),QString::number(query.value(1).toDouble()/1000.0,10,5),&doc));
            tov.appendChild(stroka);
            i++;
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }

    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        doc.save(stream,1);
        file.close();
    }
}

void FormShipCons::loadData()
{
    int id_ship = mapper->modelData(mapper->currentIndex(),0).toInt();
    DialogShipLoad d(id_ship);
    if (d.exec()==QDialog::Accepted){
        modelEl->select();
        modelWire->select();
    }
}

ModelShipCons::ModelShipCons(QObject *parent) : DbTableModel("sertifikat",parent)
{
    DbSqlRelation *relType = new DbSqlRelation("sert_type","id","nam",this);
    relType->setFilter("sert_type.id in (1,2)");
    addColumn("id",tr("id"));
    addColumn("nom_s",tr("Номер"));
    addColumn("dat_vid",tr("Дата"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),relType);
    setSort("sertifikat.dat_vid, sertifikat.nom_s");
    setDefaultValue(4,1);
}

void ModelShipCons::refresh(QDate beg, QDate end, int id_pol)
{
    QString filter="sertifikat.dat_vid between '"+beg.toString("yyyy-MM-dd")+"' and '"
            +end.toString("yyyy-MM-dd")+"'";
    if (id_pol!=-1){
        filter+=" and sertifikat.id_pol = "+QString::number(id_pol);
    }
    this->setFilter(filter);
    this->select();
}

bool ModelShipCons::insertRow(int row, const QModelIndex &parent)
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

ModelShipConsEl::ModelShipConsEl(QObject *parent) : DbTableModel("otpusk", parent)
{
    addColumn("id",tr("id"));
    addColumn("id_sert", tr("id_sert"));
    addColumn("id_part",tr("Партия"),Models::instance()->relElPart);
    addColumn("massa",tr("Масса, кг"));
    setSort("otpusk.id");
    currentIdShip=-1;
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

QVariant ModelShipConsEl::data(const QModelIndex &index, int role) const
{
    if(role == Qt::BackgroundRole) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),2),Qt::EditRole).toInt());
        if(area == 4) return QVariant(QColor(255,170,170)); else
            if(area == 5) return QVariant(QColor(Qt::yellow)); else
                if(area == 6) return QVariant(QColor(Qt::gray)); else
                    if(area == 7) return QVariant(QColor(170,255,170)); else
                        return QVariant(QColor(255,200,100));
    } else return DbTableModel::data(index,role);
}

void ModelShipConsEl::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter("otpusk.id_sert = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    select();
}

bool ModelShipConsEl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день отгрузки: ")+QString::number(getStock(index))+tr(" кг"));
    }
    return ok;
}

bool ModelShipConsEl::submit()
{
    bool ok = false;
    if (this->isEdt()){
        double kvo=this->data(this->index(currentEdtRow(),3),Qt::EditRole).toDouble();
        double m=getStock(this->index(currentEdtRow(),3));
        if (kvo>=0 && m>=kvo){
            ok=DbTableModel::submit();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день отгрузки числится ")+
                                  QLocale().toString(m,'f',2)+tr(" кг электродов этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
        }
    } else {
        ok=DbTableModel::submit();
    }
    if (ok) emit sigStock("");
    return ok;
}

void ModelShipConsEl::revert()
{
    emit sigStock("");
    return DbTableModel::revert();
}

double ModelShipConsEl::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),2),Qt::EditRole).toInt();
        int id_ship = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select kvoRs from calc_parti_one(:id_part, (select dat_vid from sertifikat where id = :id_ship ))");
        query.bindValue(":id_part",id_part);
        query.bindValue(":id_ship",id_ship);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return kvo;
}

void ModelShipConsEl::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("select otpusk.id_part, "
                  "(select case when exists (select id_chem from sert_chem where id_part=otpusk.id_part) "
                  "then 1 else 0 end "
                  "+ "
                  "case when exists(select id_mech from sert_mech where id_part=otpusk.id_part) "
                  "then 2 else 0 end "
                  "+ "
                  "case when exists(select cod from td_keys_el where id_el=(select id_el from parti where id=otpusk.id_part) "
                  "and id_diam=(select id from diam as d where d.diam=(select diam from parti where id=otpusk.id_part)) "
                  "and id_pack=(select id_pack from parti where id=otpusk.id_part) "
                  "and id_var=(select id_var from parti where id=otpusk.id_part)) "
                  "then 4 else 0 end "
                  "as r) from otpusk where otpusk.id_sert = :id ");
    query.bindValue(":id",currentIdShip);
    if (query.exec()){
        colorState.clear();
        while (query.next()){
            colorState[query.value(0).toInt()]=query.value(1).toInt();
        }
        emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
}

void ModelShipConsEl::calcSum()
{
    double sum=0;
    QString title = tr("Электроды");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelShipConsWire::ModelShipConsWire(QObject *parent) : DbTableModel("wire_shipment_consist", parent)
{
    addColumn("id",tr("id"));
    addColumn("id_ship",tr("id_sert"));
    addColumn("id_wparti",tr("Партия"),Models::instance()->relWirePart);
    addColumn("m_netto",tr("Масса, кг"));
    setSort("wire_shipment_consist.id");
    currentIdShip=-1;
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

QVariant ModelShipConsWire::data(const QModelIndex &index, int role) const
{
    if(role == Qt::BackgroundRole) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),2),Qt::EditRole).toInt());
        if(area == 4) return QVariant(QColor(255,170,170)); else
            if(area == 5) return QVariant(QColor(Qt::yellow)); else
                if(area == 6) return QVariant(QColor(Qt::gray)); else
                    if(area == 7) return QVariant(QColor(170,255,170)); else
                        return QVariant(QColor(255,200,100));
    } else return DbTableModel::data(index,role);
}

void ModelShipConsWire::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter("wire_shipment_consist.id_ship = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    select();
}

bool ModelShipConsWire::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день отгрузки: ")+QString::number(getStock(index))+tr(" кг"));
    }
    return ok;
}

bool ModelShipConsWire::submit()
{
    bool ok = false;
    if (this->isEdt()){
        double kvo=this->data(this->index(currentEdtRow(),3),Qt::EditRole).toDouble();
        double m=getStock(this->index(currentEdtRow(),3));
        if (kvo>=0 && m>=kvo){
            ok=DbTableModel::submit();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день отгрузки числится ")+
                                  QLocale().toString(m,'f',2)+tr(" кг проволоки этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
        }
    } else {
        ok=DbTableModel::submit();
    }
    if (ok) emit sigStock("");
    return ok;
}

void ModelShipConsWire::revert()
{
    emit sigStock("");
    return DbTableModel::revert();
}

double ModelShipConsWire::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),2),Qt::EditRole).toInt();
        int id_ship = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select st from wire_calc_stock((select dat_vid from sertifikat where id = :id_ship)) where id_wparti= :id_part");
        query.bindValue(":id_part",id_part);
        query.bindValue(":id_ship",id_ship);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return kvo;
}

void ModelShipConsWire::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("select wire_shipment_consist.id_wparti, "
                  "(select case when exists(select id from wire_parti_chem "
                  "where id_part=(select p.id_m from wire_parti as p where p.id = wire_shipment_consist.id_wparti)) "
                  "then 1 else 0 end "
                  "+ "
                  "case when exists(select id from wire_parti_mech "
                  "where id_part=(select p.id_m from wire_parti as p where p.id = wire_shipment_consist.id_wparti)) "
                  "then 2 else 0 end "
                  "+ "
                  "case when exists(select cod from td_keys_wire "
                  "where id_prov=(select m.id_provol from wire_parti as wp inner join wire_parti_m as m on wp.id_m=m.id where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_diam=(select m.id_diam from wire_parti as wp inner join wire_parti_m as m on wp.id_m=m.id where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_spool=(select wp.id_pack from wire_parti as wp where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_pack=(select wp.id_pack_type from wire_parti as wp where wp.id=wire_shipment_consist.id_wparti)) "
                  "then 4 else 0 end "
                  "as r) from wire_shipment_consist where wire_shipment_consist.id_ship = :id ");
    query.bindValue(":id",currentIdShip);
    if (query.exec()){
        colorState.clear();
        while (query.next()){
            colorState[query.value(0).toInt()]=query.value(1).toInt();
        }
        emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
}

void ModelShipConsWire::calcSum()
{
    double sum=0;
    QString title = tr("Проволока");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
