#include "formdatael.h"
#include "ui_formdatael.h"

FormDataEl::FormDataEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDataEl)
{
    ui->setupUi(this);
    loadSettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->checkBoxOk->setEnabled(false);
    QPalette pal=ui->checkBoxOk->palette();
    pal.setColor(QPalette::Disabled, QPalette::Text, pal.color(QPalette::Active,QPalette::Text));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, pal.color(QPalette::Active,QPalette::WindowText));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, pal.color(QPalette::Active,QPalette::ButtonText));
    ui->checkBoxOk->setPalette(pal);

    modelGost = new ModelRo(this);
    ui->listViewGost->setModel(modelGost);

    modelAmp = new ModelRo(this);
    modelAmp->setDecimal(1);
    ui->tableViewAmp->setModel(modelAmp);

    modelPacker = new QSqlQueryModel(this);

    ui->comboBoxPack->setModel(modelPacker);
    ui->comboBoxPack->setModelColumn(0);

    modelPart = new ModelPart(this);

    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->verticalHeader()->setDefaultSectionSize(ui->tableViewPart->verticalHeader()->fontMetrics().height()*1.5);

    mapper = new QDataWidgetMapper(this);

    mapper->setModel(modelPart);

    mapper->addMapping(ui->lineEditPart,1);
    mapper->addMapping(ui->dateEditPart,2);
    mapper->addMapping(ui->lineEditMark,3);
    mapper->addMapping(ui->lineEditDiam,4);
    mapper->addMapping(ui->lineEditPackEd,6);
    mapper->addMapping(ui->lineEditPackGr,7);
    mapper->addMapping(ui->lineEditKvoEd,8);
    mapper->addMapping(ui->lineEditKvoGr,9);
    mapper->addMapping(ui->lineEditEanEd,10);
    mapper->addMapping(ui->lineEditEanGr,11);
    mapper->addMapping(ui->lineEditTypeGost,12);
    mapper->addMapping(ui->lineEditPost,13);
    mapper->addMapping(ui->lineEditZnam,14);
    mapper->addMapping(ui->lineEditVl,15);
    mapper->addMapping(ui->lineEditProc,16);
    mapper->addMapping(ui->plainTextEditDesc,17);
    mapper->addMapping(ui->lineEditMarkSert,19);
    mapper->addMapping(ui->lineEditVar,20);
    mapper->addMapping(ui->checkBoxOk,21);

    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),mapper,SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshData(QModelIndex)));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updPart()));
    connect(ui->pushButtonGen,SIGNAL(clicked(bool)),this,SLOT(genEan()));
    connect(ui->lineEditMasPal,SIGNAL(textChanged(QString)),this,SLOT(setKvoPack()));

    updPart();
}

FormDataEl::~FormDataEl()
{
    saveSettings();
    delete ui;
}

QString FormDataEl::marka()
{
    return ui->lineEditMark->text();
}

QString FormDataEl::markaSert()
{
    return ui->lineEditMarkSert->text();
}

QString FormDataEl::diametr()
{
    return QLocale().toString(ui->lineEditDiam->text().toDouble(),'f',1);
}

QString FormDataEl::part()
{
    return ui->lineEditPart->text();
}

QString FormDataEl::datePart()
{
    return ui->dateEditPart->date().toString("dd.MM.yyyy");
}

QString FormDataEl::datePack()
{
    return ui->dateEditPack->date().toString("dd.MM.yyyy");
}

QString FormDataEl::packer()
{
    return ui->comboBoxPack->currentText();
}

QString FormDataEl::packEd()
{
    return ui->lineEditPackEd->text();
}

QString FormDataEl::packGr()
{
    return ui->lineEditPackGr->text();
}

QString FormDataEl::masEd()
{
    return ui->lineEditKvoEd->text().replace(".",",");
}

QString FormDataEl::masGr()
{
    return ui->lineEditKvoGr->text().replace(".",",");
}

QString FormDataEl::masPal()
{
    return ui->lineEditMasPal->text();
}

QString FormDataEl::kvoPackPal()
{
    return ui->lineEditKvoPack->text();
}

QString FormDataEl::eanEd()
{
    return ui->lineEditEanEd->text().left(12);
}

QString FormDataEl::eanGr()
{
    return ui->lineEditEanGr->text().left(12);
}

QString FormDataEl::codePart()
{
    QString part=ui->lineEditPart->text();
    part.resize(4,' ');
    QString id='e'+currentData(0).toString();
    id.resize(8,'_');
    QString year=QString::number(ui->dateEditPart->date().year());
    year.resize(4,' ');
    return id+part+'-'+year;
}

QString FormDataEl::barCodeEd()
{
    QString ean=ui->lineEditEanEd->text();
    return ean.length()==13 ? ean+codePart() : "";
}

QString FormDataEl::barCodeGr()
{
    QString ean=ui->lineEditEanGr->text();
    return ean.length()==13 ? ean+codePart() : "";
}

QString FormDataEl::barCodePack()
{
    QString base=barCodeGr();
    if (base.length()==30){
        double kvoM=ui->lineEditMasPal->text().toDouble();
        int kvoP = ui->lineEditKvoPack->text().toInt();
        int ikvoM=kvoM*100;
        base+=QString("%1").arg(ikvoM,6,'d',0,QChar('0'));
        base+=QString("%1").arg(kvoP,4,'d',0,QChar('0'));
    }
    return base;
}

QString FormDataEl::descr()
{
    return ui->plainTextEditDesc->toPlainText();
}

QString FormDataEl::numerator()
{
    QString  num;
    QString tgost=ui->lineEditTypeGost->text();
    if (!tgost.isEmpty() && tgost!="-"){
        num+=tgost+"-";
    }
    num+=markaSert()+"-Ф"+ui->lineEditDiam->text().replace(".",",");
    QString post=ui->lineEditPost->text();
    if (!post.isEmpty() && post!="-"){
        num+="-"+post;
    }
    return num;
}

QString FormDataEl::znam()
{
    return ui->lineEditZnam->text();
}

QString FormDataEl::vendorCode()
{
    return eanEd().length()==12 ? eanEd().right(5) : "";
}

QString FormDataEl::gost()
{
    QString s;
    QString sep = (modelGost->rowCount()>4) ? QString(", ") : QString("\n");
    for (int i=0; i<modelGost->rowCount(); i++){
        if (!s.isEmpty()){
            s+=sep;
        }
        QString gost = modelGost->data(modelGost->index(i,0),Qt::EditRole).toString();
        gost = gost.replace(QChar(' '),QChar(0x00A0));
        s+=gost;
    }
    return s;
}

QString FormDataEl::sert()
{
    return ui->plainTextEditSert->toPlainText();
}

QString FormDataEl::proc()
{
    return ui->lineEditProc->text();
}

QString FormDataEl::vl()
{
    return ui->lineEditVl->text();
}

QString FormDataEl::adr()
{
    return strAdr;
}

ampInfo FormDataEl::amp()
{
    ampInfo ai;
    if (modelAmp->rowCount()>0){
        ai.diam=modelAmp->data(modelAmp->index(0,0),Qt::DisplayRole).toString();
        ai.bot=modelAmp->data(modelAmp->index(0,1),Qt::DisplayRole).toString();
        ai.vert=modelAmp->data(modelAmp->index(0,2),Qt::DisplayRole).toString();
        ai.ceil=modelAmp->data(modelAmp->index(0,3),Qt::DisplayRole).toString();
    }
    return ai;
}

int FormDataEl::posPix()
{
    QModelIndex ind=mapper->model()->index(mapper->currentIndex(),18);
    int id_pix=mapper->model()->data(ind,Qt::EditRole).toInt();
    return picMap.value(id_pix).cod;
}

bool FormDataEl::check()
{
    QString err;
    bool dop=true;
    if (!updPart()){
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),QString::fromUtf8("Партия не найдена!"),QMessageBox::Ok);
        return false;
    }
    if (eanEd().length()!=12){
        err+=QString::fromUtf8("Отсутствует штрихкод. Нажмите кнопку \"Сгенерировать\".\n");
        dop=false;
    }
    if (modelGost->rowCount()<1){
        err+=QString::fromUtf8("Отсутствует нормативная документация.\n");
    }
    if (znam().isEmpty()){
        err+=QString::fromUtf8("Отсутствует знаменатель.\n");
    }
    if (modelAmp->rowCount()<1){
        err+=QString::fromUtf8("Отсутствуют рекомендуемые значения токов.\n");
    }
    if (proc().isEmpty()){
        err+=QString::fromUtf8("Отсутствует режим повторной прокалки.\n");
    }
    if (vl().isEmpty()){
        err+=QString::fromUtf8("Отсутствует допустимое содержание влаги.\n");
    }
    if (descr().isEmpty()){
        err+=QString::fromUtf8("Отсутствует описание.\n");
    }
    if (!currentData(21).toBool()){
        err+=QString::fromUtf8("Отсутствует подтверждение \"Испытания в норме\".\n");
    }

    bool ok=err.isEmpty();

    if (!ok){
        if (dop){
            err+=QString::fromUtf8("Обратитесь к начальнику ОТК.");
        }
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),err,QMessageBox::Ok);
    }
    return ok;
}


bool FormDataEl::updPart()
{
    int id_part = (mapper->currentIndex()>=0)? currentData(0).toInt() : -1;

    bool ok = modelPart->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());

    if (ok){
        ui->tableViewPart->setColumnHidden(0,true);
        ui->tableViewPart->resizeColumnsToContents();
        for (int i=7; i<modelPart->columnCount(); i++){
            ui->tableViewPart->setColumnHidden(i,true);
        }
    }

    updPacker();
    refreshDocType();
    refreshPicMap();

    QSqlQuery queryAdr;
    queryAdr.prepare("select nam_lbl, adr from hoz where id=1");
    if (queryAdr.exec()){
        while (queryAdr.next()){
            strAdr=QString::fromUtf8("Изготовитель: ")+queryAdr.value(0).toString()+QString::fromUtf8(", ")+queryAdr.value(1).toString();
        }
        strAdr.replace("\"","");
    } else {
        QMessageBox::critical(this,tr("Error"),queryAdr.lastError().text(),QMessageBox::Ok);
    }

    ui->dateEditPack->setDate(QDate::currentDate());

    bool findOk=false;

    if (id_part>0){
        for (int i=0; i<ui->tableViewPart->model()->rowCount(); i++){
            int id=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(i,0),Qt::EditRole).toInt();
            if (id==id_part){
                ui->tableViewPart->selectRow(i);
                findOk=true;
                break;
            }
        }
    } else if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->selectRow(ui->tableViewPart->model()->rowCount()-1);
    }
    return findOk;
}

void FormDataEl::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("datael_splitter_width").toByteArray());
}

void FormDataEl::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("datael_splitter_width",ui->splitter->saveState());
}

QVariant FormDataEl::currentData(int row)
{
    return mapper->model()->data(mapper->model()->index(mapper->currentIndex(),row),Qt::EditRole);
}

QString FormDataEl::getSrtStr(int id_part)
{
    QString srtStr;
    QMultiMap <int, QString> srt;

    QSqlQuery vedQuery;
    vedQuery.prepare("select z.id_doc_t, z.ved_short, z.grade_nam "
                     "from zvd_get_sert_var((select dat_part from parti where id = :id_part1 ), "
                     "(select id_el from parti where id = :id_part2 ), "
                     "(select d.id from diam as d where d.diam = (select diam from parti where id = :id_part3 )), "
                     "(select id_var from parti where id = :id_part4 ) ) as z where z.en = true order by z.id_doc_t, z.ved_short");
    vedQuery.bindValue(":id_part1",id_part);
    vedQuery.bindValue(":id_part2",id_part);
    vedQuery.bindValue(":id_part3",id_part);
    vedQuery.bindValue(":id_part4",id_part);
    if (vedQuery.exec()){
        while (vedQuery.next()){
            int id_doc_t=vedQuery.value(0).toInt();
            QString ved=vedQuery.value(1).toString();
            QString grade=vedQuery.value(2).toString();

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
    } else {
        QMessageBox::critical(NULL,tr("Error"),vedQuery.lastError().text(),QMessageBox::Ok);
    }

    QList<int> keys = srt.uniqueKeys();

    for (int i=0; i<keys.size(); ++i){
        if (!srtStr.isEmpty()){
            srtStr+="\n";
        }
        srtStr+=docType.value(keys.at(i))+":";
        QList<QString> v = srt.values(keys.at(i));
        std::sort(v.begin(),v.end());
        for (QString st:v){
            if (!srtStr.isEmpty()){
                srtStr+="\n";
            }
            srtStr+=st;
        }
    }
    return srtStr;
}

void FormDataEl::refreshData(QModelIndex /*index*/)
{
    ui->pushButtonGen->setEnabled(ui->lineEditEanEd->text().isEmpty());
    setKvoPack();
    int id_part=currentData(0).toInt();

    QSqlQuery tuQuery;
    tuQuery.prepare("select nam "
                    "from zvd_get_tu_var((select dat_part from parti where id = :id1 ), "
                    "(select id_el from parti where id = :id2 ), "
                    "(select d.id from diam as d where d.diam = (select diam from parti where id = :id3 )), "
                    "(select id_var from parti where id = :id4 ) ) ");
    tuQuery.bindValue(":id1",id_part);
    tuQuery.bindValue(":id2",id_part);
    tuQuery.bindValue(":id3",id_part);
    tuQuery.bindValue(":id4",id_part);
    modelGost->execQuery(tuQuery);

    ui->plainTextEditSert->setPlainText(getSrtStr(id_part));

    QSqlQuery queryAmp;
    queryAmp.prepare("select d.diam, a.bot, a.vert, a.ceil "
                     "from amp as a "
                     "inner join diam as d on a.id_diam = d.id "
                     "where a.id_el = (select id_el from parti where id = :id1 ) "
                     "and d.diam = (select diam from parti where id = :id2 ) "
                     "and a.id_var = (select id_var from parti where id = :id3 )");
    queryAmp.bindValue(":id1",id_part);
    queryAmp.bindValue(":id2",id_part);
    queryAmp.bindValue(":id3",id_part);
    if (modelAmp->execQuery(queryAmp)){
        ui->tableViewAmp->setColumnWidth(0,100);
        ui->tableViewAmp->setColumnWidth(1,100);
        ui->tableViewAmp->setColumnWidth(2,100);
        ui->tableViewAmp->setColumnWidth(3,100);
        modelAmp->setHeaderData(0,Qt::Horizontal,tr("Диаметр, мм"));
        modelAmp->setHeaderData(1,Qt::Horizontal,tr("Нижнее"));
        modelAmp->setHeaderData(2,Qt::Horizontal,tr("Вертикальное"));
        modelAmp->setHeaderData(3,Qt::Horizontal,tr("Потолочное"));
    }

    QModelIndex ind=mapper->model()->index(mapper->currentIndex(),18);
    int id_pix=mapper->model()->data(ind,Qt::EditRole).toInt();
    QPixmap pix;
    pix.loadFromData(picMap.value(id_pix).data);
    if (pix.isNull()){
        ui->labelPol->setPixmap(pix);
    } else {
        ui->labelPol->setPixmap(pix.scaled(ui->labelPol->size(),Qt::KeepAspectRatio));
    }
}

void FormDataEl::genEan()
{
    int id_part=currentData(0).toInt();
    QSqlQuery query;
    query.prepare("select * from add_ean_el( :id_part )");
    query.bindValue(":id_part",id_part);
    if (query.exec()){
        updPart();
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

}

void FormDataEl::setKvoPack()
{
    double ost=0;
    double kvo=ui->lineEditMasPal->text().toDouble();
    double ed=currentData(8).toDouble();
    if (ed!=0){
        ui->lineEditKvoPack->setText(QString::number(kvo/ed));
        double b;
        ost=std::modf(kvo/ed, &b);
    }
    QPalette pal=ui->lineEditKvoPack->palette();
    QColor col = (ost==0 && kvo>0) ? QColor(0,0,0) : QColor(255,0,0);
    pal.setColor(QPalette::Text,col);
    ui->lineEditKvoPack->setPalette(pal);
}

void FormDataEl::refreshDocType()
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

void FormDataEl::refreshPicMap()
{
    picMap.clear();
    QSqlQuery query;
    query.prepare("select id, dt, data from pics");
    bool ok=query.exec();
    if (ok){
        while(query.next()){
            picInfo pi;
            pi.cod=query.value(1).toInt();
            pi.data=query.value(2).toByteArray();
            picMap.insert(query.value(0).toInt(),pi);
        }
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

void FormDataEl::updPacker()
{
    QString save_p=ui->comboBoxPack->currentText();
    QSqlQuery query;
    query.prepare("select r.snam from rab_rab r inner join rab_qual q on q.id_rab=r.id "
                  "inner join rab_prof p on q.id_prof = p.id "
                  "WHERE q.dat = (select max(dat) from rab_qual where dat <= '2999-04-01' "
                  "and id_rab=r.id) and p.id=65 order by r.snam");
    if (query.exec()){
        modelPacker->setQuery(query);
        ui->comboBoxPack->setCurrentText(save_p);
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

ModelPart::ModelPart(QObject *parent) : ModelRo(parent)
{

}

bool ModelPart::refresh(QDate dbeg, QDate dend)
{
    QSqlQuery query;
    query.prepare("select p.id, p.n_s, p.dat_part, e.marka, p.diam, i.nam, ep.pack_ed, ep.pack_group, ep.mass_ed, ep.mass_group, ee.ean_ed, ee.ean_group, "
                  "g.nam, pu.nam, coalesce(p.ibco, ev.znam), e.vl, ev.proc, ev.descr, e.id_pic, coalesce(e.marka_sert,e.marka), elv.nam, p.ok "
                  "from parti as p "
                  "inner join elrtr as e on p.id_el=e.id "
                  "inner join istoch as i on p.id_ist=i.id "
                  "inner join el_pack as ep on ep.id=p.id_pack "
                  "inner join gost_types as g on e.id_gost_type=g.id "
                  "inner join purpose as pu on e.id_purpose=pu.id "
                  "left join ean_el ee on ee.id_el = p.id_el and ee.id_diam = (select d.id from diam d where d.diam=p.diam) and ee.id_pack = p.id_pack "
                  "left join el_var ev on ev.id_el = p.id_el and ev.id_var = p.id_var "
                  "inner join elrtr_vars elv on elv.id = p.id_var "
                  "where p.dat_part between :d1 and :d2 "
                  "order by p.dat_part, p.n_s");
    query.bindValue(":d1",dbeg);
    query.bindValue(":d2",dend);
    bool ok=execQuery(query);
    if (ok){
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Парт."));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Дата"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Марка"));
        setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Диам."));
        setHeaderData(5,Qt::Horizontal,QString::fromUtf8("Источник"));
        setHeaderData(6,Qt::Horizontal,QString::fromUtf8("Упаковка"));
    }
    return ok;
}

QVariant ModelPart::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundRole){
        bool ok=this->data(this->index(item.row(),21),Qt::EditRole).toBool();
        return ok ? QVariant(QColor(170,255,170)) : QVariant(QColor(255,170,170));
    }
    return ModelRo::data(item,role);
}

