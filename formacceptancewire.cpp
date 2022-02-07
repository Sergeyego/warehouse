#include "formacceptancewire.h"
#include "ui_formacceptancewire.h"

FormAcceptanceWire::FormAcceptanceWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptanceWire)
{
    ui->setupUi(this);
    loadsettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    modelAcceptanceWire = new ModelAcceptanceWire(this);
    ui->tableViewAcc->setModel(modelAcceptanceWire);
    ui->tableViewAcc->setColumnHidden(0,true);
    ui->tableViewAcc->setColumnWidth(1,80);
    ui->tableViewAcc->setColumnWidth(2,100);
    ui->tableViewAcc->setColumnWidth(3,200);

    modelAcceptanceWireData = new ModelAcceptanceWireData(this);
    ui->tableViewAccData->setModel(modelAcceptanceWireData);
    ui->tableViewAccData->setColumnHidden(0,true);
    ui->tableViewAccData->setColumnHidden(1,true);
    ui->tableViewAccData->setColumnWidth(2,400);
    ui->tableViewAccData->setColumnWidth(3,80);
    ui->tableViewAccData->setColumnWidth(4,100);

    mapper = new DbMapper(ui->tableViewAcc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);
    mapper->addEmptyLock(ui->tableViewAccData);
    mapper->addLock(ui->pushButtonUpd);


    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));
    connect(ui->pushButtonUpdPart,SIGNAL(clicked(bool)),Models::instance()->relWirePart->model(),SLOT(refresh()));

    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));

    connect(modelAcceptanceWireData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));

    updAcc();
}

FormAcceptanceWire::~FormAcceptanceWire()
{
    savesettings();
    delete ui;
}

void FormAcceptanceWire::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("accwire_splitter_width").toByteArray());
}

void FormAcceptanceWire::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("accwire_splitter_width",ui->splitter->saveState());
}

void FormAcceptanceWire::updAcc()
{
    modelAcceptanceWire->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormAcceptanceWire::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceptanceWireData->refresh(id_acc);
}

void FormAcceptanceWire::sync()
{
    Models::instance()->sync1C->syncPriemWire(mapper->modelData(mapper->currentIndex(),0).toInt());
}


void FormAcceptanceWire::setPartFilter()
{
    Models::instance()->setFilter(ui->comboBoxPart->currentIndex());
}

ModelAcceptanceWire::ModelAcceptanceWire(QObject *parent) : DbTableModel("wire_whs_waybill",parent)
{
    addColumn("id","id");
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_type",tr("Тип"),Models::instance()->relAccTypeWire);
    setSuffix("inner join wire_way_bill_type on wire_way_bill_type.id = wire_whs_waybill.id_type");
    setDefaultValue(3,3);
    setSort(name()+".num, "+name()+".dat");
}

void ModelAcceptanceWire::refresh(QDate beg, QDate end)
{
    QString filter=name()+".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"' and wire_way_bill_type.en = true";
    setFilter(filter);
    select();
}

bool ModelAcceptanceWire::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
        setDefaultValue(2,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}

ModelAcceptanceWireData::ModelAcceptanceWireData(QObject *parent) : DbTableModel("wire_warehouse",parent)
{
    addColumn("id","id");
    addColumn("id_waybill","id_waybill");
    addColumn("id_wparti",tr("Партия"),Models::instance()->relWirePart);
    addColumn("m_netto",tr("Масса, кг"));
    addColumn("numcont",tr("№ поддона"));
    setSort(name()+".id");
    setDecimals(3,2);

    connect(this,SIGNAL(sigUpd()),this,SLOT(caclSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(caclSum()));
}

void ModelAcceptanceWireData::refresh(int id_acc)
{
    setFilter(name()+".id_waybill = "+QString::number(id_acc));
    setDefaultValue(1,id_acc);
    setDefaultValue(4,1);
    select();
}

bool ModelAcceptanceWireData::insertRow(int row, const QModelIndex &parent)
{
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,4),Qt::EditRole).toInt();
        setDefaultValue(4,old_num+1);
    }
    return DbTableModel::insertRow(row,parent);
}

void ModelAcceptanceWireData::caclSum()
{
    double sum=0;
    QString title = "Передача";
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
