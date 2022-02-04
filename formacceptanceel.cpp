#include "formacceptanceel.h"
#include "ui_formacceptanceel.h"
#include "models.h"

FormAcceptanceEl::FormAcceptanceEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptanceEl)
{
    ui->setupUi(this);
    loadsettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    modelAcceptanceElData = new ModelAcceptanceElData(this);
    ui->tableViewAccData->setModel(modelAcceptanceElData);
    for (int i=0; i<5; i++){
        ui->tableViewAccData->setColumnHidden(i,true);
    }
    ui->tableViewAccData->setColumnWidth(5,350);
    ui->tableViewAccData->setColumnWidth(6,100);
    ui->tableViewAccData->setColumnWidth(7,80);

    modelAcceptanceEl = new ModelAcceptanceEl(this);
    ui->tableViewAcc->setModel(modelAcceptanceEl);
    ui->tableViewAcc->setColumnHidden(0,true);
    ui->tableViewAcc->setColumnWidth(1,80);
    ui->tableViewAcc->setColumnWidth(2,100);
    ui->tableViewAcc->setColumnWidth(3,200);

    mapper = new DbMapper(ui->tableViewAcc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);
    mapper->addEmptyLock(ui->tableViewAccData);
    mapper->addEmptyLock(ui->pushButton1C);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));
    connect(ui->pushButtonUpdPart,SIGNAL(clicked(bool)),Models::instance()->relElPart->model(),SLOT(refresh()));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(modelAcceptanceElData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));

    updAcc();
}

FormAcceptanceEl::~FormAcceptanceEl()
{
    savesettings();
    delete ui;
}

void FormAcceptanceEl::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("accel_splitter_width").toByteArray());
}

void FormAcceptanceEl::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("accel_splitter_width",ui->splitter->saveState());
}

void FormAcceptanceEl::updAcc()
{
    modelAcceptanceEl->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormAcceptanceEl::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceptanceElData->refresh(id_acc);
}

void FormAcceptanceEl::setPartFilter()
{
    Models::instance()->setFilter(ui->comboBoxPart->currentIndex());
}

void FormAcceptanceEl::sync()
{
    Models::instance()->sync1C->syncPriemEl(mapper->modelData(mapper->currentIndex(),0).toInt());
}

ModelAcceptanceEl::ModelAcceptanceEl(QWidget *parent) : DbTableModel("prod_nakl",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_ist",tr("Тип"),Models::instance()->relAccTypeEl);
    setSuffix("inner join prod_nakl_tip on prod_nakl_tip.id = prod_nakl.id_ist");
    setSort("prod_nakl.dat, prod_nakl.num");
    setDefaultValue(3,1);
}

void ModelAcceptanceEl::refresh(QDate beg, QDate end)
{
    QString filter=name()+".dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"' and prod_nakl_tip.en = true";
    setFilter(filter);
    select();
}

bool ModelAcceptanceEl::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
        setDefaultValue(2,QDate::currentDate());
    }
    return DbTableModel::insertRow(row,parent);
}

ModelAcceptanceElData::ModelAcceptanceElData(QObject *parent) : DbTableModel("prod",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_nakl",tr("id_nakl"));
    addColumn("docs",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_ist",tr("Тип"));
    addColumn("id_part",tr("Партия"),Models::instance()->relElPart);
    addColumn("kvo",tr("Масса, кг"));
    addColumn("numcont",tr("№ поддона"));
    setDecimals(6,2);
    this->setSort("prod.id");

    connect(this,SIGNAL(sigUpd()),this,SLOT(caclSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(caclSum()));
}

void ModelAcceptanceElData::refresh(int id_acc)
{
    setDefaultValue(7,1);
    QSqlQuery query;
    query.prepare("select id, num, dat, id_ist from prod_nakl where id = :id");
    query.bindValue(":id",id_acc);
    if (query.exec()){
        while (query.next()){
            for (int i=0; i<query.record().count(); i++){
                setDefaultValue(i+1,query.value(i));
            }
        }
        this->setFilter("prod.id_nakl = "+QString::number(id_acc));
        this->select();
    } else {
        this->setFilter("prod.id_nakl = -1");
        this->select();
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

bool ModelAcceptanceElData::insertRow(int row, const QModelIndex &parent)
{
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,7),Qt::EditRole).toInt();
        setDefaultValue(7,old_num+1);
    }
    return DbTableModel::insertRow(row,parent);
}

void ModelAcceptanceElData::caclSum()
{
    double sum=0;
    QString title = "Передача";
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,6),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
