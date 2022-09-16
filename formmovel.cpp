#include "formmovel.h"
#include "ui_formmovel.h"

FormMovEl::FormMovEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMovEl)
{
    ui->setupUi(this);

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    executor = new Executor(this);

    modelPartElInfo = new ModelPartElInfo(this);
    modelPartElInfo->refresh(-1);

    mapperInfo = new QDataWidgetMapper(this);
    mapperInfo->setModel(modelPartElInfo);
    mapperInfo->addMapping(ui->lineEditIn,0);
    mapperInfo->addMapping(ui->lineEditShip,1);
    mapperInfo->addMapping(ui->lineEditOst,2);
    mapperInfo->addMapping(ui->checkBoxFinish,3);

    modelPart = new TableModel(this);

    QStringList header;
    header<<tr("id")<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Упаковка")<<tr("Источник")<<tr("Остаток");

    modelPart->setHeader(header);

    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->resizeToContents();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(executor,SIGNAL(finished()),this,SLOT(upd()));

    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updInfo(QModelIndex)));
}

FormMovEl::~FormMovEl()
{
    delete ui;
}

void FormMovEl::startUpd()
{
    QString query = QString("select p.id, p.n_s||'-'||date_part('year',p.dat_part) as part, e.marka, p.diam, ev.nam, ep.pack_ed, i.nam, c.kvo "
                            "from calc_parti_new('3000-01-01') as c "
                            "inner join parti as p on c.id_part=p.id "
                            "inner join el_pack as ep on ep.id=p.id_pack "
                            "inner join elrtr as e on p.id_el=e.id "
                            "inner join istoch as i on p.id_ist=i.id "
                            "inner join elrtr_vars ev on ev.id = p.id_var "
                            "where p.dat_part between '%1' and '%2' "
                            "order by p.n_s, p.dat_part").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    executor->setQuery(query);
    executor->start();
}

void FormMovEl::upd()
{
    QVector<QVector<QVariant>> data = executor->getData();
    modelPart->setModelData(data);
    ui->tableViewPart->resizeToContents();
    if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->selectRow(0);
    }
}

void FormMovEl::updInfo(QModelIndex index)
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toInt();
    modelPartElInfo->refresh(id_part);
    mapperInfo->toFirst();
}

ModelPartElInfo::ModelPartElInfo(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelPartElInfo::refresh(int id_part)
{
    QSqlQuery query;
    query.prepare("select c.sumIn, c.sumOt, c.kvoRs, p.fini, c.id_p from "
                  "calc_parti_one(:id_part, '3000-01-01') as c "
                  "inner join parti as p on c.id_p=p.id");
    query.bindValue(":id_part",id_part);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
    }
}

QVariant ModelPartElInfo::data(const QModelIndex &item, int role) const
{
    if (item.column()<3){
        if (role==Qt::DisplayRole || role==Qt::EditRole){
            return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',2);
        } else if (role==Qt::TextAlignmentRole){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelPartElInfo::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok = false;
    if (index.column()==3){
        QSqlQuery query;
        query.prepare("update parti set fini = :val where id= :id");
        query.bindValue(":val",value.toBool());
        query.bindValue(":id",QSqlQueryModel::data(this->index(index.row(),4),Qt::EditRole).toInt());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return ok;
}

Qt::ItemFlags ModelPartElInfo::flags(const QModelIndex &index) const
{
    if (index.column()==3){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return QSqlQueryModel::flags(index);
}
