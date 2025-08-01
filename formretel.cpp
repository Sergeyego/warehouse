#include "formretel.h"
#include "ui_formretel.h"

FormRetEl::FormRetEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRetEl)
{
    ui->setupUi(this);
    loadSettings();

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    QSqlQueryModel *typeModel = new QSqlQueryModel(this);
    typeModel->setQuery("select id, nam from prod_nakl_tip where id in (4,5,9,11,13) order by nam");
    if (typeModel->lastError().isValid()){
        QMessageBox::critical(this,tr("Ошибка"),typeModel->lastError().text(),QMessageBox::Cancel);
    } else {
        ui->comboBoxType->setModel(typeModel);
        ui->comboBoxType->setModelColumn(1);
    }

    modelNaklData = new ModelNaklRetElData(this);
    ui->tableViewNaklData->setModel(modelNaklData);
    ui->tableViewNaklData->setColumnHidden(0,true);
    ui->tableViewNaklData->setColumnHidden(1,true);
    ui->tableViewNaklData->setColumnHidden(2,true);
    ui->tableViewNaklData->setColumnHidden(3,true);
    ui->tableViewNaklData->setColumnHidden(4,true);
    ui->tableViewNaklData->setColumnWidth(5,400);
    ui->tableViewNaklData->setColumnWidth(6,80);

    modelNakl = new ModelNaklRetEl(this);
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
    mapper->addEmptyLock(ui->pushButtonNakl);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
    connect(modelNaklData,SIGNAL(sigStock(QString)),ui->labelStock,SLOT(setText(QString)));

    upd();
}

FormRetEl::~FormRetEl()
{
    saveSettings();
    delete ui;
}

void FormRetEl::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("ret_el_splitter_width").toByteArray());
}

void FormRetEl::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ret_el_splitter_width",ui->splitter->saveState());
}

void FormRetEl::upd()
{
    if (sender()==ui->pushButtonUpd){
        modelNaklData->refreshRelsModel();
    }
    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0)).toInt();
    modelNakl->refresh(id_type,ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormRetEl::updData(int index)
{
    int id_nakl = mapper->modelData(index,0).toInt();
    modelNaklData->refresh(id_nakl);
}

void FormRetEl::printNakl()
{
    int id_nakl=mapper->modelData(mapper->currentIndex(),0).toInt();
    QString vid=tr("Электроды");
    QString type=tr("Склад");
    QString filename=ui->comboBoxType->currentText().toUpper()+"_"+mapper->modelData(mapper->currentIndex(),1).toString();
    int year=mapper->modelData(mapper->currentIndex(),2).toDate().year();
    Models::instance()->invoiceManager->getInvoice("invoices/elrtr/warehouse/"+QString::number(id_nakl),vid,type,filename,year);
}

ModelNaklRetEl::ModelNaklRetEl(QObject *parent) : DbTableModel("prod_nakl",parent)
{
    addColumn("id",tr("id"));
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_ist",tr("Тип"));
    setSort("prod_nakl.dat, prod_nakl.num");
}

void ModelNaklRetEl::refresh(int id_type, QDate begDate, QDate endDate)
{
    QString filter="prod_nakl.dat between '"+begDate.toString("yyyy-MM-dd")+"' and '"+endDate.toString("yyyy-MM-dd")+"' and prod_nakl.id_ist = "+QString::number(id_type);
    type=id_type;
    setFilter(filter);
    setDefaultValue(3,type);
    setDefaultValue(1,"001");
    select();
}

bool ModelNaklRetEl::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        QString num = QString::number(old_num+1);
        num=num.rightJustified(3,'0',true);
        setDefaultValue(1,num);
    }
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

ModelNaklRetElData::ModelNaklRetElData(QObject *parent) : DbTableModel("prod",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_nakl",tr("id_nakl"));
    addColumn("docs",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_ist",tr("Тип"));
    addColumn("id_part",tr("Партия"),Models::instance()->relElPart);
    addColumn("kvo",tr("Масса, кг"));
    setDecimals(6,2);
    this->setSort("prod.id");
}

void ModelNaklRetElData::refresh(int id_nakl)
{
    QSqlQuery query;
    query.prepare("select id, num, dat, id_ist from prod_nakl where id= :id");
    query.bindValue(":id",id_nakl);
    if (query.exec()){
        while (query.next()){
            for (int i=0; i<query.record().count(); i++){
                setDefaultValue(i+1,query.value(i));
            }
        }
        this->setFilter("prod.id_nakl = "+QString::number(id_nakl));
        this->select();
    } else {
        this->setFilter("prod.id_nakl = -1");
        this->select();
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
}

bool ModelNaklRetElData::submit()
{
    bool ok = false;
    if (this->isEdt()){
        QModelIndex indKvo=this->index(currentEdtRow(),6);
        if (this->data(indKvo,Qt::EditRole).isNull()){
            emit sigStock("");
            return DbTableModel::submit();
        }
        double kvo=this->data(indKvo,Qt::EditRole).toDouble();
        if (defaultValue(4).toInt()==11 || defaultValue(4).toInt()==9){
            double m=getStock(indKvo);
            if (kvo>=0 && m>=kvo){
                ok=DbTableModel::submit();
            } else {
                QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день передачи числится ")+
                                      QLocale().toString(m,'f',2)+tr(" кг электродов этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Ok);
            }
        } else {
            if (kvo>0 || defaultValue(4).toInt()==13){
                ok=DbTableModel::submit();
            } else {
                QMessageBox::critical(NULL,tr("Ошибка"),tr("Масса должна быть больше нуля."),QMessageBox::Ok);
            }
        }
    } else {
        ok=DbTableModel::submit();
    }
    if (ok) {
        emit sigStock("");
    }
    return ok;
}

void ModelNaklRetElData::revert()
{
    emit sigStock("");
    return DbTableModel::revert();
}

bool ModelNaklRetElData::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день передачи: ")+QLocale().toString(getStock(index),'f',2)+tr(" кг"));
    }
    return ok;
}

double ModelNaklRetElData::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),5),Qt::EditRole).toInt();
        int id_nakl = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select kvoRs from calc_parti_one(:id_part, (select dat from prod_nakl where id = :id_nakl) )");
        query.bindValue(":id_part",id_part);
        query.bindValue(":id_nakl",id_nakl);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        }
    }
    return kvo;
}
