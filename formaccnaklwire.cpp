#include "formaccnaklwire.h"
#include "ui_formaccnaklwire.h"

FormAccNaklWire::FormAccNaklWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAccNaklWire)
{
    ui->setupUi(this);
    loadsettings();
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    colVal defaultType;
    defaultType.val=3;
    ui->comboBoxType->setEditable(false);
    ui->comboBoxType->setModel(Models::instance()->relAccTypeWire->model());
    ui->comboBoxType->setCurrentData(defaultType);


    modelNakl = new ModelRo(this);
    ui->tableViewNakl->setModel(modelNakl);

    modelNaklCont = new ModelRo(this);
    ui->tableViewCont->setModel(modelNaklCont);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refreshNakl()));
    connect(ui->tableViewNakl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshNaklCont(QModelIndex)));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshNakl()));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
}

FormAccNaklWire::~FormAccNaklWire()
{
    savesettings();
    delete ui;
}

void FormAccNaklWire::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("accnaklwire_splitter_width").toByteArray());
}

void FormAccNaklWire::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("accnaklwire_splitter_width",ui->splitter->saveState());
}

void FormAccNaklWire::refreshNakl()
{
    if (sender()==ui->pushButtonUpd){
        Models::instance()->relAccTypeWire->refreshModel();
    }
    int r=ui->comboBoxType->currentIndex();
    if (r<0){
        return;
    }
    int id_type=ui->comboBoxType->getCurrentData().val.toInt();
    QSqlQuery query;
    query.prepare("select distinct date_part('doy',www.dat)::integer, www.dat, www.id_type "
                  "from wire_whs_waybill www "
                  "where www.dat between :d1 and :d2 and www.id_type = :t "
                  "order by www.dat");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    query.bindValue(":t",id_type);
    if (modelNakl->execQuery(query)){
        modelNakl->setHeaderData(0,Qt::Horizontal,"Номер");
        modelNakl->setHeaderData(1,Qt::Horizontal,"Дата");
        ui->tableViewNakl->setColumnHidden(2,true);
        ui->tableViewNakl->resizeToContents();
        if (modelNakl->rowCount()){
            ui->tableViewNakl->selectRow(modelNakl->rowCount()-1);
            ui->tableViewNakl->scrollToBottom();
            ui->pushButtonNakl->setEnabled(true);
        } else {
            modelNaklCont->clear();
            ui->labelSum->clear();
            ui->pushButtonNakl->setEnabled(false);
        }
    }
}

void FormAccNaklWire::refreshNaklCont(QModelIndex index)
{
    if (index.isValid()){
        QDate dat=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(index.row(),1),Qt::EditRole).toDate();
        int id_type=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(index.row(),2),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select p.nam, d.sdim, wpk.short, wpm.n_s||'-'||date_part('year',www.dat), ww.m_netto "
                      "from wire_warehouse ww "
                      "inner join wire_whs_waybill www on www.id = ww.id_waybill "
                      "inner join wire_parti wp on wp.id = ww.id_wparti "
                      "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                      "inner join provol p on p.id = wpm.id_provol "
                      "inner join diam d on d.id = wpm.id_diam "
                      "inner join wire_pack_kind wpk on wpk.id = wp.id_pack "
                      "where www.dat = :d and www.id_type = :id_ist "
                      "order by ww.id");
        query.bindValue(":d",dat);
        query.bindValue(":id_ist",id_type);
        if (modelNaklCont->execQuery(query)){
            modelNaklCont->setHeaderData(0,Qt::Horizontal,"Марка");
            modelNaklCont->setHeaderData(1,Qt::Horizontal,"Диаметр");
            modelNaklCont->setHeaderData(2,Qt::Horizontal,"Носитель");
            modelNaklCont->setHeaderData(3,Qt::Horizontal,"Партия");
            modelNaklCont->setHeaderData(4,Qt::Horizontal,"Масса, кг");
            ui->tableViewCont->resizeToContents();
            double sum=0.0;
            for (int i=0; i<modelNaklCont->rowCount();i++){
                sum+=modelNaklCont->data(modelNaklCont->index(i,4),Qt::EditRole).toDouble();
            }
            ui->labelSum->setText(QString("Итого по накладной %1 кг").arg(QLocale().toString(sum,'f',2)));
        }
    } else {
        modelNaklCont->clear();
        ui->labelSum->clear();
    }
}

void FormAccNaklWire::printNakl()
{
    int row=ui->tableViewNakl->currentIndex().row();
    QDate dat=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(row,1),Qt::EditRole).toDate();
    int id_type=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(row,2),Qt::EditRole).toInt();
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost("127.0.0.1", 5555);
    if (tcpSocket.waitForConnected()) {
        tcpSocket.write((QString("%1:%2:%3:%4").arg(1).arg(4).arg(dat.toString("dd.MM.yyyy")).arg(id_type)).toLocal8Bit().data());
        tcpSocket.waitForBytesWritten();
        tcpSocket.disconnectFromHost();
    } else {
        QMessageBox::critical(this,tr("Ошибка"),tcpSocket.errorString(),QMessageBox::Ok);
    }
}
