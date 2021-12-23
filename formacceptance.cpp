#include "formacceptance.h"
#include "ui_formacceptance.h"

FormAcceptance::FormAcceptance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptance)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    sync1C = new Sync1C(this);

    modelAcceeptance = new ModelAcceptance(this);
    ui->tableViewAc->setModel(modelAcceeptance);
    ui->tableViewAc->setColumnHidden(0,true);
    ui->tableViewAc->setColumnWidth(1,80);
    ui->tableViewAc->setColumnWidth(2,100);
    ui->tableViewAc->setColumnWidth(3,200);

    modelAcceptanceWire = new ModelAcceptanceWire(this);
    ui->tableViewAcWire->setModel(modelAcceptanceWire);
    ui->tableViewAcWire->setColumnHidden(0,true);
    ui->tableViewAcWire->setColumnWidth(1,80);
    ui->tableViewAcWire->setColumnWidth(2,100);
    ui->tableViewAcWire->setColumnWidth(3,200);

    modelAcceeptanceData = new DbTableModel("acceptance_data",this);
    modelAcceeptanceData->addColumn("id","id");
    modelAcceeptanceData->addColumn("id_acceptance","id_acceptance");
    modelAcceeptanceData->addColumn("id_part",tr("Партия"),Models::instance()->relElPart);
    modelAcceeptanceData->addColumn("kvo",tr("Масса, кг"));
    modelAcceeptanceData->setSort("acceptance_data.id");
    ui->tableViewAcCont->setModel(modelAcceeptanceData);
    ui->tableViewAcCont->setColumnHidden(0,true);
    ui->tableViewAcCont->setColumnHidden(1,true);
    ui->tableViewAcCont->setColumnWidth(2,400);
    ui->tableViewAcCont->setColumnWidth(3,80);

    modelAcceeptanceDataWire = new DbTableModel("wire_acceptance_data",this);
    modelAcceeptanceDataWire->addColumn("id","id");
    modelAcceeptanceDataWire->addColumn("id_acceptance","id_acceptance");
    modelAcceeptanceDataWire->addColumn("id_part",tr("Партия"),Models::instance()->relWirePart);
    modelAcceeptanceDataWire->addColumn("kvo",tr("Масса, кг"));
    modelAcceeptanceDataWire->setSort("wire_acceptance_data.id");
    ui->tableViewAcContWire->setModel(modelAcceeptanceDataWire);
    ui->tableViewAcContWire->setColumnHidden(0,true);
    ui->tableViewAcContWire->setColumnHidden(1,true);
    ui->tableViewAcContWire->setColumnWidth(2,400);
    ui->tableViewAcContWire->setColumnWidth(3,80);

    mapper = new DbMapper(ui->tableViewAc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);
    mapper->addEmptyLock(ui->tableViewAcCont);
    mapper->addLock(ui->pushButtonUpd);
    mapper->addLock(ui->radioButtonWire);

    mapperWire = new DbMapper(ui->tableViewAcWire,this);
    ui->horizontalLayoutMapperWire->insertWidget(0,mapperWire);
    mapperWire->addMapping(ui->lineEditNumWire,1);
    mapperWire->addMapping(ui->dateEditWire,2);
    mapperWire->addMapping(ui->comboBoxTypeWire,3);
    mapperWire->addEmptyLock(ui->tableViewAcContWire);
    mapperWire->addLock(ui->pushButtonUpd);
    mapperWire->addLock(ui->radioButtonEl);

    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));
    connect(ui->pushButtonUpdPart,SIGNAL(clicked(bool)),Models::instance()->relElPart->model(),SLOT(refresh()));
    connect(ui->pushButtonUpdPart,SIGNAL(clicked(bool)),Models::instance()->relWirePart->model(),SLOT(refresh()));

    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(mapperWire,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccDataWire(int)));

    connect(ui->radioButtonWire,SIGNAL(clicked(bool)),this,SLOT(setCurrentWidget()));
    connect(ui->radioButtonEl,SIGNAL(clicked(bool)),this,SLOT(setCurrentWidget()));

    updAcc();
}

FormAcceptance::~FormAcceptance()
{
    delete ui;
}

void FormAcceptance::updAcc()
{
    modelAcceeptance->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    modelAcceptanceWire->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormAcceptance::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceeptanceData->setFilter("acceptance_data.id_acceptance = "+QString::number(id_acc));
    modelAcceeptanceData->setDefaultValue(1,id_acc);
    modelAcceeptanceData->select();
}

void FormAcceptance::updAccDataWire(int index)
{
    int id_acc=mapperWire->modelData(index,0).toInt();
    modelAcceeptanceDataWire->setFilter("wire_acceptance_data.id_acceptance = "+QString::number(id_acc));
    modelAcceeptanceDataWire->setDefaultValue(1,id_acc);
    modelAcceeptanceDataWire->select();
}

void FormAcceptance::sync()
{
    if (ui->radioButtonEl->isChecked()){
        sync1C->syncPriemEl(mapper->modelData(mapper->currentIndex(),0).toInt());
    } else {
        sync1C->syncPriemWire(mapperWire->modelData(mapperWire->currentIndex(),0).toInt());
    }
}

void FormAcceptance::setCurrentWidget()
{
    if (ui->radioButtonEl->isChecked()){
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void FormAcceptance::setPartFilter()
{
    Models::instance()->setFilter(ui->comboBoxPart->currentIndex());
}

ModelAcceptance::ModelAcceptance(QObject *parent) : DbTableModel("acceptance",parent)
{
    addColumn("id","id");
    addColumn("num",tr("Номер"));
    addColumn("date",tr("Дата"));
    addColumn("id_type",tr("Тип"),Models::instance()->relAccType);
    setDefaultValue(3,1);
    setSort("acceptance.num, acceptance.date");
}

void ModelAcceptance::refresh(QDate beg, QDate end)
{
    QString filter="acceptance.date between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    setFilter(filter);
    select();
}

bool ModelAcceptance::insertRow(int row, const QModelIndex &parent)
{
    select();
    int old_num=0;
    if (rowCount()>0 && !isAdd()) {
        old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
        setDefaultValue(2,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}

ModelAcceptanceWire::ModelAcceptanceWire(QObject *parent) : DbTableModel("wire_acceptance",parent)
{
    addColumn("id","id");
    addColumn("num",tr("Номер"));
    addColumn("date",tr("Дата"));
    addColumn("id_type",tr("Тип"),Models::instance()->relAccType);
    setDefaultValue(3,1);
    setSort("wire_acceptance.num, wire_acceptance.date");
}

void ModelAcceptanceWire::refresh(QDate beg, QDate end)
{
    QString filter="wire_acceptance.date between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'";
    setFilter(filter);
    select();
}

bool ModelAcceptanceWire::insertRow(int row, const QModelIndex &parent)
{
    select();
    int old_num=0;
    if (rowCount()>0 && !isAdd()) {
        old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
        setDefaultValue(2,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}
