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

    modelReqEl = new ModelReqShipEl(this);
    ui->tableViewReqEl->setModel(modelReqEl);
    ui->tableViewReqEl->setColumnHidden(0,true);
    ui->tableViewReqEl->setColumnWidth(1,300);
    ui->tableViewReqEl->setColumnWidth(2,80);

    modelReqWire = new ModelReqShipWire(this);
    ui->tableViewReqWire->setModel(modelReqWire);
    ui->tableViewReqWire->setColumnHidden(0,true);
    ui->tableViewReqWire->setColumnWidth(1,300);
    ui->tableViewReqWire->setColumnWidth(2,80);

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
    ui->tableViewEl->setColumnWidth(4,300);

    modelWire = new ModelShipConsWire(this);
    ui->tableViewWire->setModel(modelWire);
    ui->tableViewWire->setColumnHidden(0,true);
    ui->tableViewWire->setColumnHidden(1,true);
    ui->tableViewWire->setColumnWidth(2,400);
    ui->tableViewWire->setColumnWidth(3,100);
    ui->tableViewWire->setColumnWidth(4,300);

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
    mapper->addEmptyLock(ui->pushButtonXMLPart);
    mapper->addEmptyLock(ui->pushButtonLoad);
    mapper->addEmptyLock(ui->pushButtonCert);
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
    connect(ui->pushButtonXMLPart,SIGNAL(clicked(bool)),this,SLOT(goXmlPart()));
    connect(ui->pushButtonLoad,SIGNAL(clicked(bool)),this,SLOT(loadData()));
    connect(ui->tableViewEl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(setCurrentShipDataEl(QModelIndex)));
    connect(modelReqEl,SIGNAL(sigSum(QString)),ui->labelStatReqEl,SLOT(setText(QString)));
    connect(ui->tableViewWire->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(setCurrentShipDataWire(QModelIndex)));
    connect(modelReqWire,SIGNAL(sigSum(QString)),ui->labelStatReqWire,SLOT(setText(QString)));
    connect(ui->pushButtonCert,SIGNAL(clicked(bool)),this,SLOT(showCert()));

    connect(ui->checkBoxReq,SIGNAL(clicked(bool)),this,SLOT(setReqStatVisible(bool)));

    connect(modelReqEl,SIGNAL(sigUpd()),modelEl,SLOT(calcSum()));
    connect(modelReqWire,SIGNAL(sigUpd()),modelWire,SLOT(calcSum()));

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
    bool vis=(settings.value("ship_cons_req_vis").toBool());
    ui->checkBoxReq->setChecked(vis);
    setReqStatVisible(vis);
}

void FormShipCons::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ship_cons_splitter_width",ui->splitter->saveState());
    settings.setValue("ship_cons_req_vis",ui->checkBoxReq->isChecked());
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
    int id_pol=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,3),Qt::EditRole).toInt();
    ui->lineEditPol->setText(Models::instance()->relPol->getDisplayValue(id_pol,"naim"));
    modelEl->refresh(id_ship,id_pol);
    modelWire->refresh(id_ship,id_pol);

    if (ui->tableViewEl->model()->rowCount()){
        ui->tableViewEl->setCurrentIndex(ui->tableViewEl->model()->index(0,2));
    }

    if (ui->tableViewWire->model()->rowCount()){
        ui->tableViewWire->setCurrentIndex(ui->tableViewWire->model()->index(0,2));
    }
}

void FormShipCons::setCurrentShipDataEl(QModelIndex index)
{
    int id_ship_data=ui->tableViewEl->model()->data(ui->tableViewEl->model()->index(index.row(),0),Qt::EditRole).toInt();
    QString strpart=ui->tableViewEl->model()->data(ui->tableViewEl->model()->index(index.row(),2),Qt::DisplayRole).toString();
    double kvo=ui->tableViewEl->model()->data(ui->tableViewEl->model()->index(index.row(),3),Qt::EditRole).toDouble();
    if (!strpart.isEmpty()){
        strpart+=":";
    }
    ui->labelPartEl->setText(strpart);
    modelReqEl->refresh(id_ship_data,kvo);
}

void FormShipCons::setCurrentShipDataWire(QModelIndex index)
{
    int id_ship_data=ui->tableViewWire->model()->data(ui->tableViewWire->model()->index(index.row(),0),Qt::EditRole).toInt();
    QString strpart=ui->tableViewWire->model()->data(ui->tableViewWire->model()->index(index.row(),2),Qt::DisplayRole).toString();
    double kvo=ui->tableViewWire->model()->data(ui->tableViewWire->model()->index(index.row(),3),Qt::EditRole).toDouble();
    if (!strpart.isEmpty()){
        strpart+=":";
    }
    ui->labelPartWire->setText(strpart);
    modelReqWire->refresh(id_ship_data,kvo);
}

void FormShipCons::edtCods()
{
    DialogCods d;
    d.exec();
    modelEl->refreshState();
    modelWire->refreshState();
}

void FormShipCons::goXmlPart()
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
    query.prepare("select * from ( "
                  "(select e.marka||' д-'||d.sdim as marka, o.massa as kvo, ke.cod as cod, "
                  "p.n_s||'-'||date_part('year',p.dat_part) as part, s.dat_vid + make_interval(0,e.shelf_life) as god, "
                  "'№'||p.n_s||'-'||date_part('year',p.dat_part)||'/'||s.nom_s||' от '||to_char(s.dat_vid, 'DD.MM.YYYY') as nom, 'e' as typ, o.hash, o.id "
                  "from otpusk o "
                  "inner join sertifikat s on s.id = o.id_sert "
                  "inner join parti p on o.id_part=p.id "
                  "inner join elrtr e on p.id_el=e.id "
                  "inner join diam d on d.diam=p.diam "
                  "left outer join td_keys_el as ke on ke.id_el=p.id_el and ke.id_diam=d.id and ke.id_pack=p.id_pack and ke.id_var=p.id_var "
                  "where o.id_sert = :id_sert1 )"
                  "union "
                  "(select pr.nam||' д-'||d.sdim||' '||k.nam, w.m_netto, kw.cod , m.n_s||'-'||date_part('year',m.dat), "
                  "s.dat_vid + make_interval(0,pr.shelf_life), "
                  "'№'||m.n_s||'-'||date_part('year',m.dat)||'/'||s.nom_s||' от '||to_char(s.dat_vid, 'DD.MM.YYYY'), 'w', w.hash, w.id "
                  "from wire_shipment_consist as w "
                  "inner join sertifikat s on s.id = w.id_ship "
                  "inner join wire_parti as p on p.id=w.id_wparti "
                  "inner join wire_parti_m as m on p.id_m=m.id "
                  "inner join provol as pr on pr.id=m.id_provol "
                  "inner join diam as d on d.id=m.id_diam "
                  "inner join wire_pack_kind as k on p.id_pack=k.id "
                  "left outer join td_keys_wire as kw on kw.id_prov=m.id_provol and kw.id_diam=m.id_diam and kw.id_spool=p.id_pack and kw.id_pack=p.id_pack_type "
                  "where w.id_ship = :id_sert2 )"
                  ") as z order by z.typ, z.marka, z.part");
    query.bindValue(":id_sert1", id_ship);
    query.bindValue(":id_sert2", id_ship);
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
            stroka.appendChild(newElement(QString::fromUtf8("Партия"),query.value(3).toString(),&doc));
            stroka.appendChild(newElement(QString::fromUtf8("СрокХранения"),query.value(4).toDate().toString("dd.MM.yyyy"),&doc));
            stroka.appendChild(newElement(QString::fromUtf8("Сертификат"),query.value(5).toString(),&doc));
            stroka.appendChild(newElement(QString::fromUtf8("СсылкаНаСертификат"),"https://certificates.czcm-weld.ru/"+query.value(7).toString()+"/"+query.value(8).toString()+"-ru.pdf",&doc));
            tov.appendChild(stroka);
            i++;
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }

    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
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

void FormShipCons::setReqStatVisible(bool b)
{
    ui->tableViewReqEl->setVisible(b);
    ui->tableViewReqWire->setVisible(b);
    ui->labelStatReqEl->setVisible(b);
    ui->labelStatReqWire->setVisible(b);
    ui->labelPartEl->setVisible(b);
    ui->labelPartWire->setVisible(b);
}

void FormShipCons::showCert()
{
    int id_ship = mapper->modelData(mapper->currentIndex(),0).toInt();
    FormCert *f = new FormCert();
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->setIdShip(id_ship);
    f->show();
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
    QString num = QString::number(old_num+1);
    num=num.rightJustified(4,'0',true);
    setDefaultValue(1,num);
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

ModelShipConsEl::ModelShipConsEl(QObject *parent) : DbTableModel("otpusk", parent)
{
    addColumn("id",tr("id"));
    addColumn("id_sert", tr("id_sert"));
    addColumn("id_part",tr("Партия"),Models::instance()->relElPart);
    addColumn("massa",tr("Масса, кг"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    setSort("otpusk.id");
    currentIdShip=-1;
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

QVariant ModelShipConsEl::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),2),Qt::EditRole).toInt());
        if (!(area & 4)){
            return QVariant(QColor(255,200,100));
        } else if (area & 8){
            return QVariant(QColor(170,255,170));
        } else if ((area & 1)&&(area & 2)){
            return QVariant(QColor(190,210,190));
        } else if (area & 1){
            return QVariant(QColor(Qt::yellow));
        } else if (area & 2){
            return QVariant(QColor(Qt::gray));
        } else {
            return QVariant(QColor(255,170,170));
        }
    } else return DbTableModel::data(index,role);
}

void ModelShipConsEl::refresh(int id_ship, int id_pol)
{
    currentIdShip=id_ship;
    setFilter("otpusk.id_sert = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    setDefaultValue(4,id_pol);
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
        bool is_pol=true;
        for (int i=0; i<4; i++){
            is_pol=is_pol && (this->oldRow().at(i).val==this->newRow().at(i).val);
        }
        if ((kvo>=0 && m>=kvo) || is_pol){
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
                  "+ "
                  "case when otpusk.ds_status=2 then 8 else 0 end) "
                  "from otpusk where otpusk.id_sert = :id ");
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
    double sumReq=0.0;
    QSqlQuery query;
    query.prepare("select sum(rse.kvo) from requests_ship_el rse "
                  "inner join otpusk o on o.id = rse.id_ship_data "
                  "where o.id_sert = :id_ship ");
    query.bindValue(":id_ship",currentIdShip);
    if (query.exec()){
        if (query.next()){
            sumReq=query.value(0).toDouble();
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    double sum=0;
    QString title = tr("Электроды");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг; По заявкам: ")+QLocale().toString(sumReq,'f',1)+tr(" кг;") ) : title;

    if (sum>0 && sumReq==sum){
        s="<font color='green'>"+s+"</font>";
    }

    emit sigSum(s);
}

ModelShipConsWire::ModelShipConsWire(QObject *parent) : DbTableModel("wire_shipment_consist", parent)
{
    addColumn("id",tr("id"));
    addColumn("id_ship",tr("id_sert"));
    addColumn("id_wparti",tr("Партия"),Models::instance()->relWirePart);
    addColumn("m_netto",tr("Масса, кг"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
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
        if (!(area & 4)){
            return QVariant(QColor(255,200,100));
        } else if (area & 8){
            return QVariant(QColor(170,255,170));
        } else if ((area & 1)&&(area & 2)){
            return QVariant(QColor(190,210,190));
        } else if (area & 1){
            return QVariant(QColor(Qt::yellow));
        } else if (area & 2){
            return QVariant(QColor(Qt::gray));
        } else {
            return QVariant(QColor(255,170,170));
        }
    } else return DbTableModel::data(index,role);
}

void ModelShipConsWire::refresh(int id_ship, int id_pol)
{
    currentIdShip=id_ship;
    setFilter("wire_shipment_consist.id_ship = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    setDefaultValue(4,id_pol);
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
        bool is_pol=true;
        for (int i=0; i<4; i++){
            is_pol=is_pol && (this->oldRow().at(i).val==this->newRow().at(i).val);
        }
        if ((kvo>=0 && m>=kvo) || is_pol){
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
                  "+ "
                  "case when wire_shipment_consist.ds_status=2 then 8 else 0 end) "
                  "from wire_shipment_consist where wire_shipment_consist.id_ship = :id ");
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
    double sumReq=0.0;
    QSqlQuery query;
    query.prepare("select sum(rsw.kvo) from requests_ship_wire rsw "
                  "inner join wire_shipment_consist o on o.id = rsw.id_ship_data "
                  "where o.id_ship = :id_ship ");
    query.bindValue(":id_ship",currentIdShip);
    if (query.exec()){
        if (query.next()){
            sumReq=query.value(0).toDouble();
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    double sum=0;
    QString title = tr("Проволока");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }

    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг; По заявкам: ")+QLocale().toString(sumReq,'f',1)+tr(" кг;") ) : title;

    if (sum>0 && sumReq==sum){
        s="<font color='green'>"+s+"</font>";
    }
    emit sigSum(s);
}

ModelReqShipEl::ModelReqShipEl(QObject *parent) : DbTableModel("requests_ship_el",parent)
{
    addColumn("id_ship_data",tr("id_ship_data"));
    addColumn("id_req",tr("Заявка"),Models::instance()->relReq);
    addColumn("kvo",tr("Кол-во, кг"));
    setSort("requests.num, requests.dat");

    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

void ModelReqShipEl::refresh(int id_ship_data, double kvo)
{
    kvoShip=kvo;
    setFilter("requests_ship_el.id_ship_data = "+QString::number(id_ship_data));
    setDefaultValue(0,id_ship_data);
    select();
}

bool ModelReqShipEl::submit()
{
    int id_ship_data=this->data(this->index(currentEdtRow(),0),Qt::EditRole).toInt();
    int id_req=this->data(this->index(currentEdtRow(),1),Qt::EditRole).toInt();

    int count=0;
    QSqlQuery query;
    query.prepare("select count(*) "
                  "from requests_el re "
                  "where re.id_req = :id_req "
                  "and re.id_el = (select p.id_el from otpusk o inner join parti p on o.id_part=p.id where o.id = :id_ship_data1 ) "
                  "and re.id_diam = (select d.id from otpusk o inner join parti p on o.id_part=p.id inner join diam d on d.diam = p.diam where o.id = :id_ship_data2 ) "
                  "and re.id_pack = (select p.id_pack from otpusk o inner join parti p on o.id_part=p.id where o.id = :id_ship_data3 )");
    query.bindValue(":id_req",id_req);
    query.bindValue(":id_ship_data1",id_ship_data);
    query.bindValue(":id_ship_data2",id_ship_data);
    query.bindValue(":id_ship_data3",id_ship_data);
    if (query.exec()){
        if (query.next()){
            count=query.value(0).toInt();
        }
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }

    bool ok = false;
    if (this->isEdt()){
        if (count>0){
            ok=DbTableModel::submit();
        } else {
            QMessageBox::critical(nullptr,tr("Ошибка"),tr("В этой заявке отсутствуют электроды данной марки, диаметра и типа упаковки"),QMessageBox::Cancel);
        }
    } else {
        ok=DbTableModel::submit();
    }
    return ok;
}

void ModelReqShipEl::calcSum()
{
    double sum=0;
    QString title = tr("Отгружено: ")+QLocale().toString(kvoShip,'f',1)+tr(" кг; По заявкам: ");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,2),Qt::EditRole).toDouble();
    }
    QString s;
    s = title+QLocale().toString(sum,'f',1)+tr(" кг;");
    if (kvoShip>0 && kvoShip==sum){
        s="<font color='green'>"+s+"</font>";
    }
    emit sigSum(s);
}

ModelReqShipWire::ModelReqShipWire(QObject *parent) : DbTableModel("requests_ship_wire",parent)
{
    addColumn("id_ship_data",tr("id_ship_data"));
    addColumn("id_req",tr("Заявка"),Models::instance()->relReq);
    addColumn("kvo",tr("Кол-во, кг"));
    setSort("requests.num, requests.dat");

    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
}

void ModelReqShipWire::refresh(int id_ship_data, double kvo)
{
    kvoShip=kvo;
    setFilter("requests_ship_wire.id_ship_data = "+QString::number(id_ship_data));
    setDefaultValue(0,id_ship_data);
    select();
}

bool ModelReqShipWire::submit()
{
    int id_ship_data=this->data(this->index(currentEdtRow(),0),Qt::EditRole).toInt();
    int id_req=this->data(this->index(currentEdtRow(),1),Qt::EditRole).toInt();

    int count=0;
    QSqlQuery query;
    query.prepare("select count(*) from requests_wire rw "
                  "where rw.id_req = :id_req "
                  "and rw.id_provol = (select wpm.id_provol from wire_shipment_consist wsc inner join wire_parti wp on wp.id = wsc.id_wparti inner join wire_parti_m wpm on wpm.id = wp.id_m  where wsc.id = :id_ship_data1 ) "
                  "and rw.id_diam = (select wpm.id_diam from wire_shipment_consist wsc inner join wire_parti wp on wp.id = wsc.id_wparti inner join wire_parti_m wpm on wpm.id = wp.id_m  where wsc.id = :id_ship_data2 ) "
                  "and rw.id_spool = (select wp.id_pack from wire_shipment_consist wsc inner join wire_parti wp on wp.id = wsc.id_wparti  where wsc.id = :id_ship_data3 )");
    query.bindValue(":id_req",id_req);
    query.bindValue(":id_ship_data1",id_ship_data);
    query.bindValue(":id_ship_data2",id_ship_data);
    query.bindValue(":id_ship_data3",id_ship_data);
    if (query.exec()){
        if (query.next()){
            count=query.value(0).toInt();
        }
    } else {
        QMessageBox::critical(nullptr,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }

    bool ok = false;
    if (this->isEdt()){
        if (count>0){
            ok=DbTableModel::submit();
        } else {
            QMessageBox::critical(nullptr,tr("Ошибка"),tr("В этой заявке отсутствует проволока данной марки, диаметра и типом носителя"),QMessageBox::Cancel);
        }
    } else {
        ok=DbTableModel::submit();
    }
    return ok;
}

void ModelReqShipWire::calcSum()
{
    double sum=0;
    QString title = tr("Отгружено: ")+QLocale().toString(kvoShip,'f',1)+tr(" кг; По заявкам: ");
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,2),Qt::EditRole).toDouble();
    }
    QString s;
    s = title+QLocale().toString(sum,'f',1)+tr(" кг;");
    if (kvoShip>0 && kvoShip==sum){
        s="<font color='green'>"+s+"</font>";
    }
    emit sigSum(s);
}
