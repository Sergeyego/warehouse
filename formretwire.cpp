#include "formretwire.h"
#include "ui_formretwire.h"

FormRetWire::FormRetWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRetWire)
{
    ui->setupUi(this);
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxFlt->addItem(tr("начиная с текущего года"));
    ui->comboBoxFlt->addItem(tr("начиная с прошлого года"));
    ui->comboBoxFlt->addItem(tr("за всё время"));
    ui->comboBoxFlt->setCurrentIndex(1);

    QSqlQueryModel *typeModel = new QSqlQueryModel(this);
    typeModel->setQuery("select id, nam from wire_way_bill_type where (id=4 or id=5 or id=7) order by id");
    if (typeModel->lastError().isValid()){
        QMessageBox::critical(this,tr("Ошибка"),typeModel->lastError().text(),QMessageBox::Cancel);
    } else {
        ui->comboBoxType->setModel(typeModel);
        ui->comboBoxType->setModelColumn(1);
    }

    modelNakl = new ModelNaklRetWire(this);
    ui->tableViewNakl->setModel(modelNakl);
    ui->tableViewNakl->setColumnHidden(0,true);
    ui->tableViewNakl->setColumnWidth(1,80);
    ui->tableViewNakl->setColumnWidth(2,80);
    ui->tableViewNakl->setColumnHidden(3,true);

    mapper = new DbMapper(ui->tableViewNakl,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addLock(ui->comboBoxType);
    mapper->addEmptyLock(ui->tableViewNaklData);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));

    upd();
}

FormRetWire::~FormRetWire()
{
    delete ui;
}

void FormRetWire::upd()
{
    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0)).toInt();
    modelNakl->refresh(id_type,ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

ModelNaklRetWire::ModelNaklRetWire(QObject *parent) : DbTableModel("wire_whs_waybill",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_type",tr("id_type"));
    setSort("dat, num");
}

void ModelNaklRetWire::refresh(int id_type, QDate begDate, QDate endDate)
{
    setFilter("wire_whs_waybill.dat between '"+begDate.toString("yyyy-MM-dd")+"' and '"+
              endDate.toString("yyyy-MM-dd")+
              "' and wire_whs_waybill.id_type = "+QString::number(id_type));
    type=id_type;
    setDefaultValue(3,type);
    select();
}

bool ModelNaklRetWire::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
        setDefaultValue(2,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}
