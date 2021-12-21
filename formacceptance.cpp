#include "formacceptance.h"
#include "ui_formacceptance.h"

FormAcceptance::FormAcceptance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptance)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    sync1C = new Sync1C(this);

    modelAcceeptance = new DbTableModel("acceptance",this);
    modelAcceeptance->addColumn("id","id");
    modelAcceeptance->addColumn("num",tr("Номер"));
    modelAcceeptance->addColumn("date",tr("Дата"));
    modelAcceeptance->addColumn("id_type",tr("Тип"),Models::instance()->relAccType);
    modelAcceeptance->setDefaultValue(3,1);
    modelAcceeptance->setSort("acceptance.num, acceptance.date");
    ui->tableViewAc->setModel(modelAcceeptance);
    ui->tableViewAc->setColumnHidden(0,true);
    ui->tableViewAc->setColumnWidth(1,80);
    ui->tableViewAc->setColumnWidth(2,100);
    ui->tableViewAc->setColumnWidth(3,200);

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

    mapper = new DbMapper(ui->tableViewAc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);

    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));

    updAcc();
}

FormAcceptance::~FormAcceptance()
{
    delete ui;
}

void FormAcceptance::updAcc()
{
    QString filter="acceptance.date between '"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"' and '"+ui->dateEditEnd->date().toString("yyyy-MM-dd")+"'";
    modelAcceeptance->setFilter(filter);
    modelAcceeptance->select();
}

void FormAcceptance::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceeptanceData->setFilter("acceptance_data.id_acceptance = "+QString::number(id_acc));
    modelAcceeptanceData->setDefaultValue(1,id_acc);
    modelAcceeptanceData->select();
}

void FormAcceptance::sync()
{
    sync1C->syncPriemEl(mapper->modelData(mapper->currentIndex(),0).toInt());
}
