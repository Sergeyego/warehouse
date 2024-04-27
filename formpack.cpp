#include "formpack.h"
#include "ui_formpack.h"

FormPack::FormPack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPack)
{
    ui->setupUi(this);
    QDate currentDate=QDate::currentDate();
    ui->dateEditBeg->setDate(currentDate.addDays(-currentDate.day()+1));
    ui->dateEditEnd->setDate(QDate(currentDate.year(),12,31));

    modelDate = new ModelRo(this);
    ui->tableViewDat->setModel(modelDate);

    modelPack = new ModelPack(this);
    ui->tableViewOp->setModel(modelPack);
    ui->tableViewOp->setColumnHidden(0,true);
    ui->tableViewOp->setColumnWidth(1,120);
    ui->tableViewOp->setColumnWidth(2,80);
    ui->tableViewOp->setColumnWidth(3,140);
    ui->tableViewOp->setColumnWidth(4,130);
    ui->tableViewOp->setColumnWidth(5,330);
    ui->tableViewOp->setColumnWidth(6,110);
    ui->tableViewOp->setColumnWidth(7,80);
    ui->tableViewOp->setColumnWidth(8,140);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonPack,SIGNAL(clicked(bool)),this,SLOT(updCont()));
    connect(ui->radioButtonTermoPack,SIGNAL(clicked(bool)),this,SLOT(updCont()));
    connect(ui->tableViewDat->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updCont()));
    connect(modelPack,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(packList()));

    upd();
}

FormPack::~FormPack()
{
    delete ui;
}

void FormPack::upd()
{
    QSqlQuery query;
    query.prepare("select distinct epo.dtm::date as dat "
                  "from el_pallet_op epo "
                  "where epo.dtm between :d1 and :d2 "
                  "order by dat");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    if (modelDate->execQuery(query)){
        modelDate->setHeaderData(0,Qt::Horizontal,tr("Дата"));
    }
    if (modelDate->rowCount()){
        ui->tableViewDat->setColumnWidth(0,100);
        ui->tableViewDat->selectRow(ui->tableViewDat->model()->rowCount()-1);
    }
}

void FormPack::updCont()
{
    QDate dat=ui->tableViewDat->model()->data(ui->tableViewDat->model()->index(ui->tableViewDat->currentIndex().row(),0),Qt::EditRole).toDate();
    modelPack->refresh(dat,ui->radioButtonPack->isChecked());
    if (modelPack->rowCount()){
        ui->tableViewOp->selectRow(ui->tableViewOp->model()->rowCount()-1);
    }
}

void FormPack::packList()
{
    int id = ui->tableViewOp->model()->data(ui->tableViewOp->model()->index(ui->tableViewOp->currentIndex().row(),0),Qt::EditRole).toInt();
    DialogWebView d;
    d.sendGetReq("packlists/elrtr/"+QString::number(id));
    d.exec();
}

ModelPack::ModelPack(QWidget *parent) : DbTableModel("el_pallet_op",parent)
{
    addColumn("id",tr("id"));
    addColumn("dtm",tr("Время"));
    addColumn("id_cex",tr("Цех"),Models::instance()->relCex);
    addColumn("id_rab",tr("Работник"),Models::instance()->relRabPack);
    addColumn("id_op",tr("Операция"),Models::instance()->relPackOp);
    addColumn("id_parti",tr("Партия"),Models::instance()->relElPart);
    addColumn("id_pallet",tr("Поддон"),Models::instance()->relPallet);
    addColumn("kvo",tr("Кол-во, кг"));
    addColumn("id_main_rab",tr("Мастер"),Models::instance()->relMaster);
    setSort("el_pallet_op.dtm");

    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
}

void ModelPack::refresh(QDate dat, bool is_pack)
{
    setFilter("el_pallet_op.dtm::date = '"+dat.toString("yyyy-MM-dd")+"' and el_pallet_op.id_op in ("+(is_pack ? "1, 3" : "2")+")");
    setDefaultValue(1,dat);
    select();
}

void ModelPack::calcSum()
{
    double sum=0;
    QString title = "Упаковано";
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,7),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
