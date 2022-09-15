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
    ui->comboBoxFlt->setCurrentIndex(Models::instance()->relWirePart->currentFilter());

    QSqlQueryModel *typeModel = new QSqlQueryModel(this);
    typeModel->setQuery("select id, nam from wire_way_bill_type where (id=4 or id=5 or id=7) order by id");
    if (typeModel->lastError().isValid()){
        QMessageBox::critical(this,tr("Ошибка"),typeModel->lastError().text(),QMessageBox::Cancel);
    } else {
        ui->comboBoxType->setModel(typeModel);
        ui->comboBoxType->setModelColumn(1);
    }

    modelNaklData = new ModelNaklRetWireData(this);
    ui->tableViewNaklData->setModel(modelNaklData);
    ui->tableViewNaklData->setColumnHidden(0,true);
    ui->tableViewNaklData->setColumnHidden(1,true);
    ui->tableViewNaklData->setColumnWidth(2,300);
    ui->tableViewNaklData->setColumnWidth(3,80);

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
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));
    connect(ui->comboBoxFlt,SIGNAL(currentIndexChanged(int)),Models::instance()->relWirePart,SLOT(setFilter(int)));
    connect(Models::instance()->relWirePart,SIGNAL(filterChanged(int)),this,SLOT(setCurrentFilter(int)));

    upd();
}

FormRetWire::~FormRetWire()
{
    delete ui;
}

void FormRetWire::upd()
{
    QDate minDate=ui->dateEditBeg->date().addYears(-4);
    QSqlQuery query;
    query.prepare("select min(wpm.dat) from wire_whs_waybill www "
                  "inner join wire_warehouse ww on ww.id_waybill = www.id "
                  "inner join wire_parti wp on wp.id = ww.id_wparti "
                  "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                  "where www.dat between :d1 and :d2");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    if (query.exec()){
        query.next();
        QDate dt=query.value(0).toDate();
        if (dt<minDate){
            minDate=dt;
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    Models::instance()->modelWirePart->setMinDate(minDate);
    if (sender()==ui->pushButtonUpd){
        Models::instance()->modelWirePart->refresh();
    }

    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0)).toInt();
    modelNakl->refresh(id_type,ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormRetWire::updData(int index)
{
    int id_nakl = mapper->modelData(index,0).toInt();
    modelNaklData->refresh(id_nakl);
}

void FormRetWire::setCurrentFilter(int num)
{
    ui->comboBoxFlt->blockSignals(true);
    ui->comboBoxFlt->setCurrentIndex(num);
    ui->comboBoxFlt->blockSignals(false);
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

ModelNaklRetWireData::ModelNaklRetWireData(QObject *parent) : DbTableModel("wire_warehouse",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_waybill",tr("id_waybill"));
    addColumn("id_wparti",tr("Партия"),Models::instance()->relWirePart);
    addColumn("m_netto",tr("Кол-во, кг"));
    setSort("wire_warehouse.id");
}

void ModelNaklRetWireData::refresh(int id_nakl)
{
    setFilter("wire_warehouse.id_waybill = "+QString::number(id_nakl));
    setDefaultValue(1,id_nakl);
    select();
}
