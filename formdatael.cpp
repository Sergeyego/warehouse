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

    modelPacker = new QSqlQueryModel(this);
    updPacker();

    ui->comboBoxPack->setModel(modelPacker);
    ui->comboBoxPack->setModelColumn(0);

    modelPart = new ModelRo(this);

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

QString FormDataEl::barCode()
{
    QString ean=ui->lineEditEanGr->text();
    ean.resize(13,' ');
    QString part=ui->lineEditPart->text();
    part.resize(4,' ');
    QString id='e'+currentData(0).toString();
    id.resize(8,'_');
    QString year=QString::number(ui->dateEditPart->date().year());
    year.resize(4,' ');
    return ean+id+part+'-'+year;
}

QString FormDataEl::barCodePack()
{
    QString base=barCode();
    double kvoM=ui->lineEditMasPal->text().toDouble();
    int kvoP = ui->lineEditKvoPack->text().toInt();
    int ikvoM=kvoM*100;
    base+=QString("%1").arg(ikvoM,6,'d',0,QChar('0'));
    base+=QString("%1").arg(kvoP,4,'d',0,QChar('0'));
    return base;
}


bool FormDataEl::selectPart()
{
    QSqlQuery query;
    query.prepare("select p.id, p.n_s, p.dat_part, e.marka, p.diam, i.nam, ep.pack_ed, ep.pack_group, ep.mass_ed, ep.mass_group, ee.ean_ed, ee.ean_group "
                  "from parti as p "
                  "inner join elrtr as e on p.id_el=e.id "
                  "inner join istoch as i on p.id_ist=i.id "
                  "inner join el_pack as ep on ep.id=p.id_pack "
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

void FormDataEl::refreshData(QModelIndex /*index*/)
{
    ui->pushButtonGen->setEnabled(ui->lineEditEanEd->text().isEmpty());
    setKvoPack();
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
