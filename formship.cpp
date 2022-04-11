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
    push->addLock(ui->cmdEdtCods);

    ui->comboBoxOnly->setModel(Models::instance()->relPol->model());
    ui->comboBoxOnly->setModelColumn(Models::instance()->relPol->columnDisplay());
    ui->comboBoxOnly->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->comboBoxOnly->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    shipContInfo ei;
    ei.tablename="otpusk";
    ei.namId="id";
    ei.namIdDoc="id_sert";
    ei.namKis="kis";
    ei.namIdPart="id_part";
    ei.namKvo="massa";
    ei.modelBalence=modelBalance;
    ei.prefix="e";
    ei.relPart = new DbRelation(Models::instance()->modelElPart,0,1,this);
    ei.queryState=QString("select otpusk.id_part, "
                          "(select case when exists (select id_chem from sert_chem where id_part=otpusk.id_part) "
                          "then 1 else 0 end "
                          "+ "
                          "case when exists(select id_mech from sert_mech where id_part=otpusk.id_part) "
                          "then 2 else 0 end "
                          "+ "
                          "case when exists(select cod from td_keys_el where id_el=(select id_el from parti where id=otpusk.id_part) and id_diam=(select id from diam as d where d.diam=(select diam from parti where id=otpusk.id_part)) and id_pack=(select id_pack from parti where id=otpusk.id_part)) "
                          "then 4 else 0 end "
                          "as r) from otpusk where otpusk.id_sert = :id ");
    modelShipEl = new ModelShipData(ei,this);
    ui->tableViewEl->setModel(modelShipEl);
    ui->tableViewEl->setColumnHidden(0,true);
    ui->tableViewEl->setColumnHidden(1,true);
    ui->tableViewEl->setColumnWidth(2,200);
    ui->tableViewEl->setColumnWidth(3,350);
    ui->tableViewEl->setColumnWidth(4,100);

    shipContInfo wi;
    wi.tablename="wire_shipment_consist";
    wi.namId="id";
    wi.namIdDoc="id_ship";
    wi.namKis="kis";
    wi.namIdPart="id_wparti";
    wi.namKvo="m_netto";
    wi.modelBalence=modelBalance;
    wi.prefix="w";
    wi.relPart = new DbRelation(Models::instance()->modelWirePart,0,1,this);
    wi.queryState=QString("select wire_shipment_consist.id_wparti, "
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
        ui->cmdEdtCods->setEnabled(false);
        ui->tableViewShip->setMenuEnabled(false);
        ui->tableViewEl->setMenuEnabled(false);
        ui->tableViewWire->setMenuEnabled(false);
    }

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
    connect(ui->cmdEdtCods,SIGNAL(clicked(bool)),this,SLOT(edtCods()));
    connect(ui->tableViewEl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(ui->tableViewWire->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updKisBalance(QModelIndex)));
    connect(modelBalance,SIGNAL(sigUpd()),ui->tableViewBal,SLOT(resizeToContents()));
    connect(ui->pushButtonEdt,SIGNAL(clicked(bool)),this,SLOT(updBalance()));
    connect(push,SIGNAL(sigWrite()),this,SLOT(updBalance()));
    connect(modelShipEl, SIGNAL(sigSum(QString)),ui->labelSumEl,SLOT(setText(QString)));
    connect(modelShipWire, SIGNAL(sigSum(QString)),ui->labelSumWire,SLOT(setText(QString)));
    connect(ui->pushButtonXml,SIGNAL(clicked(bool)),this,SLOT(goXml()));

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
}

void FormShip::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ship_splitter_width",ui->splitter->saveState());
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

void FormShip::goXml()
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
                                                 "left outer join td_keys_el as ke on ke.id_el=p.id_el and ke.id_diam=d.id and ke.id_pack=p.id_pack "
                                                 "where o.id_sert= :id_sert "
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

void FormShip::edtCods()
{
    DialogCods d;
    d.exec();
    modelShipEl->refreshState();
    modelShipWire->refreshState();
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

QDomElement FormShip::newElement(QString nam, QString val, QDomDocument *doc)
{
    QDomElement l = doc->createElement(nam);
    l.appendChild(doc->createTextNode(val));
    return l;
}

ModelShip::ModelShip(QObject *parent) : DbTableModel("sertifikat",parent)
{
    addColumn("id",tr("id"));
    addColumn("nom_s",tr("Номер"));
    addColumn("dat_vid",tr("Дата"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),Models::instance()->relShipType);
    setSort("sertifikat.dat_vid, sertifikat.nom_s");
    setDefaultValue(4,1);
}

void ModelShip::refresh(QDate beg, QDate end, int id_pol)
{
    QString filter="sertifikat.dat_vid between '"+beg.toString("yyyy-MM-dd")+"' and '"
            +end.toString("yyyy-MM-dd")+"'";
    if (id_pol!=-1){
        filter+=" and sertifikat.id_pol = "+QString::number(id_pol);
    }
    this->setFilter(filter);
    this->select();
}

bool ModelShip::insertRow(int row, const QModelIndex &parent)
{
    select();
    int old_num=0;
    if (rowCount()>0) old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
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
    QMultiHash<QString, partInfo>::const_iterator i = part.constBegin();
    while (i != part.constEnd()) {
        partInfo pinfo=i.value();
        contInfo cnt = cont.value(pinfo.contKey);
        if (cnt.zone!="Упаковка" && pinfo.id_part_kis==ide){
            kvo+=(pinfo.kvo);
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
    for (partInfo i : list){
        QVector<QVariant> row;
        contInfo cnt = cont.value(i.contKey);
        if (cnt.zone!="Упаковка"){
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
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

QVariant ModelShipData::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::BackgroundColorRole)) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),3),Qt::EditRole).toInt());
        if (area == 4) return QVariant(QColor(255,170,170)); else
            if (area == 5) return QVariant(QColor(Qt::yellow)); else
                if (area == 6) return QVariant(QColor(Qt::gray)); else
                    if (area == 7) return QVariant(QColor(170,255,170)); else
                        return QVariant(QColor(255,200,100));
    } else return DbTableModel::data(index,role);
}

void ModelShipData::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter(info.tablename+"."+info.namIdDoc+" = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    select();
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
    /*bool ok = false;
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
        return DbTableModel::submit();
    }*/
    bool ok = DbTableModel::submit();
    if (ok) emit sigStock("");
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

double ModelShipData::getStock(QModelIndex index)
{
    int id_part = this->data(this->index(index.row(),3),Qt::EditRole).toInt();
    return info.modelBalence->getStock(info.prefix+":"+QString::number(id_part));
}

void ModelShipData::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(info.queryState);
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

void ModelShipData::setPartFlt(int ind)
{
    fltind=ind;
    setFlt("");
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
