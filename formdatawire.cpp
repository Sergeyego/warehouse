#include "formdatawire.h"
#include "ui_formdatawire.h"

FormDataWire::FormDataWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDataWire)
{
    ui->setupUi(this);

    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    refreshDocType();

    modelTu = new ModelRo(this);
    ui->listViewGost->setModel(modelTu);

    modelOtk = new ModelRo(this);
    modelNam = new ModelRo(this);
    modelMaster = new ModelRo(this);

    modelOdobr = new ModelRo(this);
    ui->tableViewOdobr->setModel(modelOdobr);
    ui->tableViewOdobr->verticalHeader()->setDefaultSectionSize(ui->tableViewOdobr->verticalHeader()->fontMetrics().height()*1.5);

    ui->comboBoxOtk->setModel(modelOtk);
    ui->comboBoxNam->setModel(modelNam);
    ui->comboBoxMaster->setModel(modelMaster);

    modelPartOrig = new ModelRo(this);
    ui->comboBoxOPart->setModel(modelPartOrig);
    ui->comboBoxOPart->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->comboBoxOPart->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    modelPart = new ModelRo(this);

    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->verticalHeader()->setDefaultSectionSize(ui->tableViewPart->verticalHeader()->fontMetrics().height()*1.5);

    mapper = new QDataWidgetMapper(this);

    mapper->setModel(modelPart);

    mapper->addMapping(ui->lineEditPart,1);
    mapper->addMapping(ui->lineEditMark,2);
    mapper->addMapping(ui->lineEditDiam,3);
    mapper->addMapping(ui->lineEditSpool,4);
    mapper->addMapping(ui->dateEdit,6);
    mapper->addMapping(ui->lineEditPlav,7);
    mapper->addMapping(ui->lineEditKvo,8);
    mapper->addMapping(ui->plainTextEdit,9);
    mapper->addMapping(ui->lineEditEanEd,10);
    mapper->addMapping(ui->lineEditEanGr,11);

    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshData(QModelIndex)));
    connect(ui->comboBoxOPart->lineEdit(),SIGNAL(editingFinished()),this,SLOT(setOrigPart()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updPart()));
    connect(ui->pushButtonGen,SIGNAL(clicked(bool)),this,SLOT(genEan()));

    updPart();
}

FormDataWire::~FormDataWire()
{
    delete ui;
}

QString FormDataWire::getSert()
{
    QString srtStr;
    QMultiMap <int, QString> srt;
    for (int i=0; i<modelOdobr->rowCount(); i++){
        int id_doc_t=modelOdobr->data(modelOdobr->index(i,0),Qt::EditRole).toInt();
        QString ved=modelOdobr->data(modelOdobr->index(i,1),Qt::EditRole).toString();
        QString grade=modelOdobr->data(modelOdobr->index(i,2),Qt::EditRole).toString();

        QString s=ved;
        if (!grade.isEmpty()){
            s+=QString::fromUtf8(" категория ")+grade;
        }

        if (srt.contains(id_doc_t, ved) && (ved!=s)){
            srt.remove(id_doc_t,ved);
        }

        QStringList list(srt.values(id_doc_t));

        if (list.indexOf(QRegExp(QString("^"+s+".*")))==-1){
            srt.insert(id_doc_t,s);
        }
    }

    QList<int> keys = srt.uniqueKeys();

    for (int i=0; i<keys.size(); ++i){
        if (!srtStr.isEmpty()){
            srtStr+="\n";
        }
        srtStr+=docType.value(keys.at(i))+": ";
        QList<QString> v = srt.values(keys.at(i));
        qSort(v.begin(),v.end());
        bool first=true;
        for (QString st:v){
            if (!first){
                srtStr+="; ";
            }
            first=false;
            srtStr+=st;
        }
    }
    srtStr.replace("\"","");
    return srtStr;
}

QString FormDataWire::marka()
{
    return ui->lineEditMark->text();
}

QString FormDataWire::diametr()
{
    return QLocale().toString(ui->lineEditDiam->text().toDouble(),'f',1);
}

QString FormDataWire::plavka()
{
    return ui->lineEditPlav->text();
}

QString FormDataWire::part()
{
    return ui->lineEditPart->text();
}

QString FormDataWire::datePart()
{
    return ui->dateEdit->date().toString("dd.MM.yyyy");
}

QString FormDataWire::datePack()
{
    return ui->dateEditPack->date().toString("dd.MM.yyyy");
}

QString FormDataWire::spool()
{
    return ui->lineEditSpool->text();
}

QString FormDataWire::mas()
{
    return ui->lineEditKvo->text();
}

QString FormDataWire::eanEd()
{
    return ui->lineEditEanEd->text().left(12);
}

QString FormDataWire::eanGr()
{
    QString ean;
    if (!ui->lineEditEanGr->text().isEmpty()){
        ean=ui->lineEditEanGr->text().left(12);
    } else if (!ui->lineEditEanEd->text().isEmpty()){
        ean=ui->lineEditEanEd->text().left(12);
    }
    return ean;
}

QString FormDataWire::qrCode()
{
    QString t;
    t+="c="+ui->lineEditEanEd->text()+"\n";
    t+="p="+ui->lineEditPart->text()+"-"+QString::number(ui->dateEdit->date().year())+"\n";
    t+="m="+ui->lineEditKvo->text()+"\n";
    t+=site;
    return t;
}

QString FormDataWire::codeProd()
{
    int n=ui->comboBoxOPart->currentIndex();
    QString npart=ui->comboBoxOPart->model()->data(ui->comboBoxOPart->model()->index(n,2),Qt::EditRole).toString();
    QString year=ui->comboBoxOPart->model()->data(ui->comboBoxOPart->model()->index(n,3),Qt::EditRole).toDate().toString("yy");
    year=year.rightJustified(2,QChar('0'));
    npart=npart.rightJustified(4,QChar('0'));
    return year+npart+getNum(ui->comboBoxNam);
}

QString FormDataWire::otkNum()
{
    QString num;
    if (ui->comboBoxOtk->findText(ui->comboBoxOtk->currentText())!=-1){
        num=getNum(ui->comboBoxOtk);
    }
    return num;
}

QString FormDataWire::otkNam()
{
    return ui->comboBoxOtk->currentText();
}

QString FormDataWire::gost()
{
    QString s;
    for (int i=0; i<modelTu->rowCount(); i++){
        if (!s.isEmpty()){
            s+="\n";
        }
        s+=modelTu->data(modelTu->index(i,0),Qt::EditRole).toString();
    }
    return s;
}

QString FormDataWire::description()
{
    return ui->plainTextEdit->toPlainText();
}

QString FormDataWire::upkNum()
{
    return ui->lineEditUpk->text();
}

QString FormDataWire::adr()
{
    return strAdr;
}

QString FormDataWire::kvoSpool()
{
    return ui->lineEditKvoSpool->text();
}

QString FormDataWire::master()
{
    return ui->comboBoxMaster->currentText();
}

bool FormDataWire::selectPart()
{
    QSqlQuery query;
    query.prepare("select p.id, m.n_s, w.nam, d.sdim, k.short, i.nam, m.dat, b.n_plav, wp.mas_ed, w.description, we.ean_ed, we.ean_group "
                  "from wire_parti as p "
                  "inner join wire_parti_m as m on p.id_m=m.id "
                  "inner join provol as w on m.id_provol=w.id "
                  "inner join diam as d on m.id_diam=d.id "
                  "inner join wire_pack_kind as k on p.id_pack=k.id "
                  "inner join wire_source as i on m.id_source=i.id "
                  "inner join prov_buht as b on m.id_buht=b.id "
                  "inner join wire_pack as wp on wp.id = p.id_pack_type "
                  "left join wire_ean as we on we.id_prov=m.id_provol and we.id_diam=m.id_diam and we.id_spool=p.id_pack and we.id_pack=p.id_pack_type "
                  "where m.dat between :d1 and :d2 "
                  "order by m.dat, m.n_s");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    bool ok=modelPart->execQuery(query);
    if (ok){
        ui->tableViewPart->setColumnHidden(0,true);
        ui->tableViewPart->resizeColumnsToContents();
        modelPart->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Парт."));
        modelPart->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Марка"));
        modelPart->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Диам."));
        modelPart->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Носитель"));
        modelPart->setHeaderData(5,Qt::Horizontal,QString::fromUtf8("Источник"));
        modelPart->setHeaderData(6,Qt::Horizontal,QString::fromUtf8("Дата"));
        for (int i=7; i<modelPart->columnCount(); i++){
            ui->tableViewPart->setColumnHidden(i,true);
        }
    }
    return ok;
}

QString FormDataWire::getNum(QComboBox *c)
{
    int n=0;
    if (c->findText(c->currentText())!=-1 && c->model()->columnCount()>2){
        n=c->model()->data(c->model()->index(c->currentIndex(),2),Qt::EditRole).toInt();
    }
    return QString("%1").arg((n),2,'d',0,QChar('0'));
}

void FormDataWire::refreshData(QModelIndex index)
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toInt();
    ui->pushButtonGen->setEnabled(ui->lineEditEanEd->text().isEmpty());
    QSqlQuery tuQuery;
    tuQuery.prepare("select g.nam from wire_parti_gost as w "
                    "inner join gost_new as g on w.id_gost=g.id "
                    "where w.id_parti = (select p.id_m from wire_parti as p where p.id = :id) order by g.nam");
    tuQuery.bindValue(":id",id_part);
    if (modelTu->execQuery(tuQuery)){
        ui->listViewGost->setModelColumn(1);
        modelTu->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Название"));
    }

    QSqlQuery sertQuery;
    sertQuery.prepare("select i.id_doc_t, i.ved_short, i.grade "
                      "from zvd_get_wire_sert( "
                      "(select dat from wire_parti_m where id= (select p.id_m from wire_parti as p where p.id= :id1 ) ), "
                      "(select id_provol from wire_parti_m where id= (select p.id_m from wire_parti as p where p.id= :id2 ) ), "
                      "(select id_diam from wire_parti_m where id= (select p.id_m from wire_parti as p where p.id= :id3 ) ) "
                      ") as i group by i.id_doc_t, i.ved_short, i.grade order by i.id_doc_t ");
    sertQuery.bindValue(":id1",id_part);
    sertQuery.bindValue(":id2",id_part);
    sertQuery.bindValue(":id3",id_part);
    if (modelOdobr->execQuery(sertQuery)){
        ui->tableViewOdobr->setColumnHidden(0,true);
        ui->tableViewOdobr->resizeColumnsToContents();
        modelOdobr->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Ведомство"));
        modelOdobr->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Категория"));
    }

    for (int i=0; i<ui->comboBoxOPart->model()->rowCount(); i++){
        QModelIndex ind=ui->comboBoxOPart->model()->index(i,0);
        if (ui->comboBoxOPart->model()->data(ind,Qt::EditRole).toInt()==id_part){
            ui->comboBoxOPart->setCurrentIndex(i);
            break;
        }
    }
}

void FormDataWire::genEan()
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(ui->tableViewPart->currentIndex().row(),0),Qt::EditRole).toInt();
    int indop=ui->comboBoxOPart->currentIndex();
    QString mas=ui->lineEditKvo->text();
    QSqlQuery query;
    query.prepare("select * from add_ean_wire(:id_part)");
    query.bindValue(":id_part",id_part);
    if (query.exec()){
        selectPart();
        for (int i=0; i<ui->tableViewPart->model()->rowCount(); i++){
            int id=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(i,0),Qt::EditRole).toInt();
            if (id==id_part){
                ui->tableViewPart->selectRow(i);
                break;
            }
        }
        ui->comboBoxOPart->setCurrentIndex(indop);
        ui->lineEditKvo->setText(mas);
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

void FormDataWire::setOrigPart()
{
    ui->comboBoxOPart->setCurrentIndex(ui->comboBoxOPart->findText(ui->comboBoxOPart->currentText()));
}

void FormDataWire::refreshDocType()
{
    docType.clear();
    QSqlQuery query;
    query.prepare("select id, nam from zvd_doc_type order by nam");
    bool ok=query.exec();
    if (ok){
        while(query.next()){
            docType.insert(query.value(0).toInt(),query.value(1).toString());
        }
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

void FormDataWire::updPart()
{
    QSqlQuery queryOtk;
    queryOtk.prepare("select o.id, o.nam ||' ('||o.num||')', o.num from otk as o where o.num<>0 order by o.nam");
    if (modelOtk->execQuery(queryOtk)){
        ui->comboBoxOtk->setModelColumn(1);
    }

    QSqlQuery queryNam;
    queryNam.prepare("select r.id, r.first_name||' '||substr(r.last_name,1,1)||'. '||substr(r.middle_name,1,1)||'. '||'('||n.num||')' as rab, n.num "
                     "from wire_namoch as n "
                     "inner join wire_empl as r on n.id_rab = r.id "
                     "where r.id <> 0 and n.id_pr = 3 "
                     "order by rab");
    if (modelNam->execQuery(queryNam)){
        ui->comboBoxNam->setModelColumn(1);
    }

    QSqlQuery queryMaster;
    queryMaster.prepare("select oj.id, e.first_name||' '||substr(e.last_name,1,1)||'. '||substr(e.middle_name,1,1)||'.' "
                        "from wire_empl as e "
                        "left outer join "
                        "(select md.id_empl as id, p.nam as nam, p.id as idp from "
                        "(select id_empl, max(dat) as mdat from (select * from wire_empl_pos_h where dat<= :dat ) as th group by id_empl) as md "
                        "inner join wire_empl_pos_h h on (h.id_empl=md.id_empl and h.dat=md.mdat and h.id_op<>3) "
                        "inner join wire_empl_pos as p on p.id=h.id_pos) as oj on e.id=oj.id "
                        "where oj.idp=50 "
                        "order by e.first_name, e.last_name, e.middle_name");
    queryMaster.bindValue(":dat",ui->dateEditEnd->date());
    if (modelMaster->execQuery(queryMaster)){
        ui->comboBoxMaster->setModelColumn(1);
    }

    QSqlQuery queryOPart;
    queryOPart.prepare("select p.id, m.n_s ||'-'||date_part('year',m.dat) ||' '||pr.nam ||' '|| d.sdim || ' '|| k.short  as part, m.n_s, m.dat "
                       "from wire_parti as p "
                       "inner join wire_parti_m as m on p.id_m=m.id "
                       "inner join provol as pr on pr.id=m.id_provol "
                       "inner join diam as d on d.id=m.id_diam "
                       "inner join wire_pack_kind as k on k.id=p.id_pack "
                       "where m.dat>= :dat "
                       "order by part desc");
    queryOPart.bindValue(":dat",ui->dateEditBeg->date().addYears(-1));
    if (modelPartOrig->execQuery(queryOPart)){
        ui->comboBoxOPart->setModelColumn(1);
    }

    QSqlQuery queryAdr;
    queryAdr.prepare("select nam_lbl, adr, site from hoz where id=1");
    if (queryAdr.exec()){
        while (queryAdr.next()){
            strAdr=QString::fromUtf8("Изготовитель ")+queryAdr.value(0).toString()+QString::fromUtf8(", ")+queryAdr.value(1).toString();
            site=queryAdr.value(2).toString();
        }
        strAdr.replace("\"","");
    } else {
        QMessageBox::critical(this,tr("Error"),queryAdr.lastError().text(),QMessageBox::Ok);
    }

    if (selectPart()){
        if (modelPart->rowCount()){
            ui->tableViewPart->selectRow(modelPart->rowCount()-1);
        }
    }
    ui->dateEditPack->setDate(QDate::currentDate());
}
