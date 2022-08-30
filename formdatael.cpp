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

    relPosPix = new DbRelation(QString("select id, data, dt from pics"),0,1,this);

    modelGost = new ModelRo(this);
    ui->listViewGost->setModel(modelGost);

    modelAmp = new ModelRo(this);
    modelAmp->setDecimal(1);
    ui->tableViewAmp->setModel(modelAmp);

    modelPacker = new QSqlQueryModel(this);
    updPacker();

    ui->comboBoxPack->setModel(modelPacker);
    ui->comboBoxPack->setModelColumn(0);

    modelPart = new ModelRo(this);

    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->verticalHeader()->setDefaultSectionSize(ui->tableViewPart->verticalHeader()->fontMetrics().height()*1.5);

    mapper = new QDataWidgetMapper(this);
    mapper->setItemDelegate(new CustomDelegate(this));

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
    return ui->lineEditKvoEd->text();
}

QString FormDataEl::masGr()
{
    return ui->lineEditKvoGr->text();
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
    num+=marka()+"-∅"+ui->lineEditDiam->text().replace(".",",");
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
    for (int i=0; i<modelGost->rowCount(); i++){
        if (!s.isEmpty()){
            s+="\n";
        }
        s+=modelGost->data(modelGost->index(i,0),Qt::EditRole).toString();
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
    return relPosPix->data(QString::number(id_pix),2).toInt();
}

bool FormDataEl::check()
{
    QString err;
    if (eanEd().length()!=12){
        err+=QString::fromUtf8("Отсутствует штрихкод. Нажмите кнопку \"Сгенерировать\".\n");
    }
    if (modelGost->rowCount()<1){
        err+=QString::fromUtf8("Отсутствует нормативная документация. Обратитесь к контролёрам.\n");
    }
    if (znam().isEmpty()){
        err+=QString::fromUtf8("Отсутствует знаменатель. Обратитесь к контролёрам.\n");
    }
    if (modelAmp->rowCount()<1){
        err+=QString::fromUtf8("Отсутствуют рекомендуемые значения токов. Обратитесь к начальнику ОТК.\n");
    }
    if (proc().isEmpty()){
        err+=QString::fromUtf8("Отсутствует режим повторной прокалки. Обратитесь к начальнику ОТК.\n");
    }
    if (vl().isEmpty()){
        err+=QString::fromUtf8("Отсутствует допустимое содержание влаги. Обратитесь к начальнику ОТК.\n");
    }
    if (descr().isEmpty()){
        err+=QString::fromUtf8("Отсутствует описание. Обратитесь к начальнику ОТК.\n");
    }

    if (!err.isEmpty()){
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),err,QMessageBox::Ok);
    }
    return err.isEmpty();
}


bool FormDataEl::selectPart()
{
    QSqlQuery query;
    query.prepare("select p.id, p.n_s, p.dat_part, e.marka, p.diam, i.nam, ep.pack_ed, ep.pack_group, ep.mass_ed, ep.mass_group, ee.ean_ed, ee.ean_group, "
                  "g.nam, pu.nam, p.ibco, e.vl, e.pr2, e.descr, e.id_pic "
                  "from parti as p "
                  "inner join elrtr as e on p.id_el=e.id "
                  "inner join istoch as i on p.id_ist=i.id "
                  "inner join el_pack as ep on ep.id=p.id_pack "
                  "inner join gost_types as g on e.id_gost_type=g.id "
                  "inner join purpose as pu on e.id_purpose=pu.id "
                  "left join ean_el ee on ee.id_el = p.id_el and ee.id_diam = (select d.id from diam d where d.diam=p.diam) and ee.id_pack = p.id_pack "
                  "where p.dat_part between :d1 and :d2 "
                  "order by p.dat_part, p.n_s");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    bool ok=modelPart->execQuery(query);
    if (ok){
        ui->tableViewPart->setColumnHidden(0,true);
        ui->tableViewPart->resizeColumnsToContents();
        modelPart->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Парт."));
        modelPart->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Дата"));
        modelPart->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Марка"));
        modelPart->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Диам."));
        modelPart->setHeaderData(5,Qt::Horizontal,QString::fromUtf8("Источник"));
        modelPart->setHeaderData(6,Qt::Horizontal,QString::fromUtf8("Упаковка"));
        for (int i=7; i<modelPart->columnCount(); i++){
            ui->tableViewPart->setColumnHidden(i,true);
        }
    }

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
    return ok;
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
                     "from zvd_get_sert((select dat_part from parti where id = :id_part1 ), "
                     "(select id_el from parti where id = :id_part2 ), "
                     "(select d.id from diam as d where d.diam = (select diam from parti where id = :id_part3 ))) as z order by z.id_doc_t, z.ved_short");
    vedQuery.bindValue(":id_part1",id_part);
    vedQuery.bindValue(":id_part2",id_part);
    vedQuery.bindValue(":id_part3",id_part);
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
        srtStr+=Models::instance()->relDocType->data(QString::number(keys.at(i))).toString()+":";
        QList<QString> v = srt.values(keys.at(i));
        qSort(v.begin(),v.end());
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
    tuQuery.prepare("select gn.nam  from parti_gost pg "
                    "inner join gost_new gn on gn.id = pg.id_gost "
                    "where pg.id_part = :id "
                    "order by gn.nam");
    tuQuery.bindValue(":id",id_part);
    modelGost->execQuery(tuQuery);

    ui->plainTextEditSert->setPlainText(getSrtStr(id_part));

    QSqlQuery queryAmp;
    queryAmp.prepare("select d.diam, a.bot, a.vert, a.ceil "
                     "from amp as a "
                     "inner join diam as d on a.id_diam = d.id "
                     "where a.id_el = (select id_el from parti where id = :id1 ) and d.diam = (select diam from parti where id = :id2 )");
    queryAmp.bindValue(":id1",id_part);
    queryAmp.bindValue(":id2",id_part);
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
    pix.loadFromData(relPosPix->data(QString::number(id_pix)).toByteArray());
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
        selectPart();
        for (int i=0; i<ui->tableViewPart->model()->rowCount(); i++){
            int id=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(i,0),Qt::EditRole).toInt();
            if (id==id_part){
                ui->tableViewPart->selectRow(i);
                break;
            }
        }
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

void FormDataEl::updPart()
{
    if (selectPart()){
        if (modelPart->rowCount()){
            ui->tableViewPart->selectRow(modelPart->rowCount()-1);
        }
    }
}

void FormDataEl::updPacker()
{
    QSqlQuery query;
    query.prepare("select r.snam from rab_rab r inner join rab_qual q on q.id_rab=r.id "
                  "inner join rab_prof p on q.id_prof = p.id "
                  "WHERE q.dat = (select max(dat) from rab_qual where dat <= '2999-04-01' "
                  "and id_rab=r.id) and p.id=65 order by r.snam");
    if (query.exec()){
        modelPacker->setQuery(query);
    } else {
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

CustomDelegate::CustomDelegate(QObject *parent) : QItemDelegate(parent)
{

}

void CustomDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column()==16){
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line){
            QString pr=index.model()->data(index,Qt::EditRole).toString();
            QStringList list=pr.split(":");
            if (list.size()==4){
                line->setText(QString("%1±%2°C %3 %4").arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3)));
            } else {
                line->clear();
            }
        }

    } else {
        QItemDelegate::setEditorData(editor,index);
    }
}
