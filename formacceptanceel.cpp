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

    modelAcceptanceElData = new ModelAcceptanceElData(this);
    ui->tableViewAccData->setModel(modelAcceptanceElData);
    for (int i=0; i<5; i++){
        ui->tableViewAccData->setColumnHidden(i,true);
    }
    ui->tableViewAccData->setColumnHidden(9,true);

    ui->tableViewAccData->setColumnWidth(5,350);
    ui->tableViewAccData->setColumnWidth(6,100);
    ui->tableViewAccData->setColumnWidth(7,80);
    ui->tableViewAccData->setColumnWidth(8,130);

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
    mapper->addEmptyLock(ui->pushButtonNakl);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(modelAcceptanceElData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));

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
    if (sender()==ui->pushButtonUpd){
        modelAcceptanceEl->refreshRelsModel();
        modelAcceptanceElData->refreshRelsModel();
    }
    modelAcceptanceEl->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormAcceptanceEl::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceptanceElData->refresh(id_acc);
}

void FormAcceptanceEl::sync()
{
    ui->pushButtonNakl->setEnabled(false);
    Models::instance()->sync1C->syncPriemEl(mapper->modelData(mapper->currentIndex(),0).toInt());
    ui->pushButtonNakl->setEnabled(true);
}

void FormAcceptanceEl::printNakl()
{
    QString id_ist=modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),3),Qt::EditRole).toString();
    QString year=QString::number(modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),2),Qt::EditRole).toDate().year());
    QString num=modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),1),Qt::EditRole).toString();
    QString kis=modelAcceptanceEl->sqlRelation(3)->getDisplayValue(id_ist,"prefix")+year+"-"+num;

    /*PackNaklDoc doc(kis);
    DialogPrintPackList d(&doc);
    d.setWindowTitle("Накладная "+kis);
    d.setSingle(false);
    d.exec();*/

    DialogWebView d;
    if (d.sendGetReq("packnakl/nakl/"+kis)){
        d.exec();
    }
}

ModelAcceptanceEl::ModelAcceptanceEl(QWidget *parent) : DbTableModel("prod_nakl",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_ist",tr("Тип"),Models::instance()->relAccTypeEl);
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
    addColumn("barcodecont",tr("Штрихкод поддона"));
    addColumn("chk",tr("check"));
    setDecimals(6,2);
    setDefaultValue(9,false);
    this->setSort("prod.id");

    connect(this,SIGNAL(sigUpd()),this,SLOT(caclSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(caclSum()));
}

QVariant ModelAcceptanceElData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        if (DbTableModel::data(this->index(index.row(),9),Qt::EditRole).toBool()){
            return QColor(170,255,170);
        } else {
            return QVariant();
        }
    }
    return DbTableModel::data(index,role);
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
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
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

Qt::ItemFlags ModelAcceptanceElData::flags(const QModelIndex &index) const
{
    if ((index.column()==8 || index.column()==5) && DbTableModel::data(this->index(index.row(),9),Qt::EditRole).toBool()){
        return Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return DbTableModel::flags(index);
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
