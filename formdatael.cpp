#include "formdatael.h"
#include "ui_formdatael.h"

FormDataEl::FormDataEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDataEl)
{
    ui->setupUi(this);

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

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

    updPart();
}

FormDataEl::~FormDataEl()
{
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

QString FormDataEl::eanEd()
{
    return ui->lineEditEanEd->text().left(12);
}

QString FormDataEl::eanGr()
{
    return ui->lineEditEanGr->text().left(12);
}

QString FormDataEl::qrCode()
{
    QString t;
    t+=ui->lineEditEanEd->text();
    t+=ui->lineEditPart->text()+"-"+QString::number(ui->dateEditPart->date().year());
    return t;
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
    return ok;
}

void FormDataEl::refreshData(QModelIndex /*index*/)
{
    ui->pushButtonGen->setEnabled(ui->lineEditEanEd->text().isEmpty());
}

void FormDataEl::genEan()
{
    int id_part=mapper->model()->data(mapper->model()->index(mapper->currentIndex(),0),Qt::EditRole).toInt();
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

void FormDataEl::updPart()
{
    if (selectPart()){
        if (modelPart->rowCount()){
            ui->tableViewPart->selectRow(modelPart->rowCount()-1);
        }
    }
}
