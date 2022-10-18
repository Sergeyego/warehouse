#include "formretwire.h"
#include "ui_formretwire.h"

FormRetWire::FormRetWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRetWire)
{
    ui->setupUi(this);
    loadSettings();

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    QSqlQueryModel *typeModel = new QSqlQueryModel(this);
    typeModel->setQuery("select id, nam from wire_way_bill_type where id in (4,5,7) order by nam");
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
    ui->tableViewNaklData->setColumnWidth(2,400);
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
    mapper->addEmptyLock(ui->pushButtonNakl);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
    connect(modelNaklData,SIGNAL(sigStock(QString)),ui->labelStock,SLOT(setText(QString)));

    upd();
}

FormRetWire::~FormRetWire()
{
    saveSettings();
    delete ui;
}

void FormRetWire::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("ret_wire_splitter_width").toByteArray());
}

void FormRetWire::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ret_wire_splitter_width",ui->splitter->saveState());
}

void FormRetWire::upd()
{
    if (sender()==ui->pushButtonUpd){
        modelNaklData->refreshRelsModel();
    }
    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0)).toInt();
    modelNakl->refresh(id_type,ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormRetWire::updData(int index)
{
    int id_nakl = mapper->modelData(index,0).toInt();
    modelNaklData->refresh(id_nakl);
}

void FormRetWire::printNakl()
{
    int id_nakl=mapper->modelData(mapper->currentIndex(),0).toInt();
    int id_type=mapper->modelData(mapper->currentIndex(),3).toInt();
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost("127.0.0.1", 5555);
    if (tcpSocket.waitForConnected()) {
        tcpSocket.write((QString("%1:%2:%3:%4").arg(1).arg(1).arg(id_nakl).arg(id_type)).toLocal8Bit().data());
        tcpSocket.waitForBytesWritten();
        tcpSocket.disconnectFromHost();
    } else {
        QMessageBox::critical(this,tr("Ошибка"),tcpSocket.errorString(),QMessageBox::Ok);
    }
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
    setDefaultValue(1,"0001");
    select();
}

bool ModelNaklRetWire::insertRow(int row, const QModelIndex &parent)
{
    select();
    if (rowCount()>0 && !isAdd()) {
        int old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
        setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
    }
    setDefaultValue(2,QDate::currentDate());
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

    QSqlQuery query;
    query.prepare("select id_type from wire_whs_waybill where id = :id_n");
    query.bindValue(":id_n",id_nakl);
    if (query.exec()){
        if (query.next()){
            id_type=query.value(0).toInt();
        }
    } else {
        QMessageBox::critical(NULL,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

bool ModelNaklRetWireData::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день передачи: ")+QLocale().toString(getStock(index),'f',2)+tr(" кг"));
    }
    return ok;
}

bool ModelNaklRetWireData::submit()
{
    bool ok=false;
    if (this->isEdt()){
        QModelIndex indKvo=this->index(currentEdtRow(),3);
        if (this->data(indKvo,Qt::EditRole).isNull()){
            emit sigStock("");
            return DbTableModel::submit();
        }
        double kvo=this->data(indKvo,Qt::EditRole).toDouble();
        if (id_type==5){
            double m=getStock(indKvo);
            if (kvo>0 && m>=kvo){
                ok=DbTableModel::submit();
            } else {
                QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на дату передачи числится ")+
                                      QLocale().toString(m,'f',2)+tr(" кг проволоки этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Ok);
            }
        } else {
            if (id_type==7 || kvo>0){
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

void ModelNaklRetWireData::revert()
{
    emit sigStock("");
    return DbTableModel::revert();
}

double ModelNaklRetWireData::getStock(QModelIndex index)
{
    double kvo=0;
    int id_nakl=this->data(this->index(index.row(),1),Qt::EditRole).toInt();
    int id_part=this->data(this->index(index.row(),2),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("select st from wire_calc_stock((select dat from wire_whs_waybill where id = :id_nakl)) where id_wparti = :id_part ");
    query.bindValue(":id_nakl",id_nakl);
    query.bindValue(":id_part",id_part);
    if (query.exec()){
        if (query.next()){
            kvo=query.value(0).toDouble();
        }

    } else {
        QMessageBox::critical(NULL,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
    return kvo;
}
