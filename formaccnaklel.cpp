#include "formaccnaklel.h"
#include "ui_formaccnaklel.h"

FormAccNaklEl::FormAccNaklEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAccNaklEl)
{
    ui->setupUi(this);
    loadsettings();
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    ui->comboBoxType->setModel(Models::instance()->relAccTypeEl->model());
    ui->comboBoxType->setModelColumn(1);

    for (int i=0; i<ui->comboBoxType->model()->rowCount(); i++){
        if (ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(i,0),Qt::EditRole).toInt()==1){
            ui->comboBoxType->setCurrentIndex(i);
            break;
        }
    }

    modelNakl = new ModelRo(this);
    ui->tableViewNakl->setModel(modelNakl);

    modelNaklCont = new ModelRo(this);
    ui->tableViewCont->setModel(modelNaklCont);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refreshNakl()));
    connect(ui->tableViewNakl->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshNaklCont(QModelIndex)));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshNakl()));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
}

FormAccNaklEl::~FormAccNaklEl()
{
    savesettings();
    delete ui;
}

void FormAccNaklEl::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("accnaklel_splitter_width").toByteArray());
}

void FormAccNaklEl::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("accnaklel_splitter_width",ui->splitter->saveState());
}

void FormAccNaklEl::refreshNakl()
{
    int r=ui->comboBoxType->currentIndex();
    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(r,0),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("select distinct date_part('doy',pn.dat)::integer, pn.dat, pn.id_ist "
                  "from prod_nakl pn "
                  "where pn.dat between :d1 and :d2 and pn.id_ist = :t "
                  "order by pn.dat");
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

void FormAccNaklEl::refreshNaklCont(QModelIndex index)
{
    if (index.isValid()){
        QDate dat=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(index.row(),1),Qt::EditRole).toDate();
        int id_type=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(index.row(),2),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select e.marka, p2.diam, p2.n_s||'-'||date_part('year',p.dat), p.kvo "
                      "from prod p "
                      "inner join parti p2 on p2.id = p.id_part "
                      "inner join elrtr e on e.id = p2.id_el "
                      "where p.dat = :d and p.id_ist = :id_ist "
                      "order by p.id");
        query.bindValue(":d",dat);
        query.bindValue(":id_ist",id_type);
        if (modelNaklCont->execQuery(query)){
            modelNaklCont->setHeaderData(0,Qt::Horizontal,"Марка");
            modelNaklCont->setHeaderData(1,Qt::Horizontal,"Диаметр");
            modelNaklCont->setHeaderData(2,Qt::Horizontal,"Партия");
            modelNaklCont->setHeaderData(3,Qt::Horizontal,"Масса, кг");
            ui->tableViewCont->resizeToContents();
            double sum=0.0;
            for (int i=0; i<modelNaklCont->rowCount();i++){
                sum+=modelNaklCont->data(modelNaklCont->index(i,3),Qt::EditRole).toDouble();
            }
            ui->labelSum->setText(QString("Итого по накладной %1 кг").arg(QLocale().toString(sum,'f',2)));
        }
    } else {
        modelNaklCont->clear();
        ui->labelSum->clear();
    }
}

void FormAccNaklEl::printNakl()
{
    int row=ui->tableViewNakl->currentIndex().row();
    QDate dat=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(row,1),Qt::EditRole).toDate();
    int id_type=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(row,2),Qt::EditRole).toInt();
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost("127.0.0.1", 5555);
    if (tcpSocket.waitForConnected()) {
        tcpSocket.write((QString("%1:%2:%3:%4").arg(0).arg(4).arg(dat.toString("dd.MM.yyyy")).arg(id_type)).toLocal8Bit().data());
        tcpSocket.waitForBytesWritten();
        tcpSocket.disconnectFromHost();
    } else {
        QMessageBox::critical(this,tr("Ошибка"),tcpSocket.errorString(),QMessageBox::Ok);
    }
}
