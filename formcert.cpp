#include "formcert.h"
#include "ui_formcert.h"

FormCert::FormCert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormCert)
{
    ui->setupUi(this);
    reader = new Reader(this);
    reader->setScale(73);
    ui->verticalLayoutView->addWidget(reader);

    modelDataShip = new ModelDataShip(this);
    ui->tableViewShipData->setModel(modelDataShip);

    connect(ui->tableViewShipData->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(currentRowChanged(QModelIndex)));
    connect(ui->pushButtonPrintAll,SIGNAL(clicked(bool)),this,SLOT(printAll()));
    connect(ui->pushButtonSaveAll,SIGNAL(clicked(bool)),this,SLOT(pdfAll()));
}

FormCert::~FormCert()
{
    delete ui;
}

void FormCert::setIdShip(int id)
{
    QSqlQuery query;
    query.prepare("select s.nom_s, s.dat_vid, p.naim from sertifikat s inner join poluch p on p.id=s.id_pol where s.id = :id");
    query.bindValue(":id",id);
    if (query.exec()){
        if (query.next()){
            n_s=query.value(0).toString();
            year=query.value(1).toDate().year();
            ui->label->setText(tr("Отгрузка № ")+n_s+tr(" от ")+query.value(1).toDate().toString("dd.MM.yyyy")+tr(" Получатель: ")+query.value(2).toString());
        }
        modelDataShip->refresh(id);
        ui->tableViewShipData->setColumnHidden(0,true);
        ui->tableViewShipData->setColumnWidth(1,65);
        ui->tableViewShipData->setColumnWidth(2,75);
        ui->tableViewShipData->setColumnWidth(3,250);
        ui->tableViewShipData->setColumnWidth(4,65);
        ui->tableViewShipData->setColumnWidth(5,45);
        ui->tableViewShipData->setColumnHidden(6,true);
        ui->tableViewShipData->setColumnHidden(7,true);
        if (modelDataShip->rowCount()){
            ui->tableViewShipData->selectRow(0);
        } else {
            reader->clear();
        }

    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        modelDataShip->clear();
        reader->clear();
    }

}

void FormCert::currentRowChanged(QModelIndex index)
{
    int id_ship=modelDataShip->data(modelDataShip->index(index.row(),0),Qt::EditRole).toInt();
    QString prefix=modelDataShip->data(modelDataShip->index(index.row(),1),Qt::EditRole).toString();
    QString name = prefix+"_"+modelDataShip->data(modelDataShip->index(index.row(),2),Qt::EditRole).toString();
    name+="_"+n_s;
    name=name.replace(QRegularExpression("[^\\w]"), "_");
    reader->setCurrentIdShip(id_ship,name,prefix);
}

void FormCert::printAll()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec()) {
        QProgressDialog* pprd = new QProgressDialog(tr("Идет формирование документа..."),"", 0, modelDataShip->rowCount(), this);
        pprd->setCancelButton(0);
        pprd->setMinimumDuration(0);
        pprd->setWindowTitle(tr("Пожалуйста, подождите"));

        QPainter painter(&printer);
        QString lang=reader->getCurrentLang();
        for (int i=0; i<modelDataShip->rowCount(); i++){
            QCoreApplication::processEvents();
            pprd->setValue(i);
            int id_ship=modelDataShip->data(modelDataShip->index(i,0),Qt::EditRole).toInt();
            QString prefix=modelDataShip->data(modelDataShip->index(i,1),Qt::EditRole).toString();
            QByteArray data;
            bool ok = HttpSyncManager::sendGet("/s3/img/"+prefix+"/"+QString::number(id_ship)+"/"+lang+"/300",data);
            if (ok && data.size()){
                QImage img = QImage::fromData(data,"png");
                painter.drawImage(painter.window(),img);
                if (i<modelDataShip->rowCount()-1){
                    printer.newPage();
                }
            }
        }
        delete pprd;
    }
}

void FormCert::pdfAll()
{
    QProgressDialog* pprd = new QProgressDialog(tr("Идет сохранение документов..."),"", 0, modelDataShip->rowCount(), this);
    pprd->setCancelButton(0);
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle(tr("Пожалуйста, подождите"));
    QString lang=reader->getCurrentLang();
    for (int i=0; i<modelDataShip->rowCount(); i++){
        QCoreApplication::processEvents();
        pprd->setValue(i);
        int id_ship=modelDataShip->data(modelDataShip->index(i,0),Qt::EditRole).toInt();
        QString prefix = modelDataShip->data(modelDataShip->index(i,1),Qt::EditRole).toString();
        QByteArray data;
        bool ok = HttpSyncManager::sendGet("/s3/local/"+prefix+"/"+QString::number(id_ship)+"/"+lang,data);
        if (ok && data.size()){
            QString name = prefix+"_"+modelDataShip->data(modelDataShip->index(i,2),Qt::EditRole).toString();
            name+="_"+n_s;
            name=name.replace(QRegularExpression("[^\\w]"), "_");
            QDir dir(QDir::homePath()+"/sertificat");
            if (!dir.exists()) dir.mkdir(dir.path());
            dir.setPath(dir.path()+"/"+QString::number(year));
            if (!dir.exists()) dir.mkdir(dir.path());
            dir.setPath(dir.path()+"/"+n_s);
            if (!dir.exists()) dir.mkdir(dir.path());
            QFile file(dir.path()+"/"+name+"_"+lang+"_sig.pdf");
            if (file.open(QFile::WriteOnly)){
                file.write(data);
                file.close();
            }
        }
    }
    delete pprd;
}

ModelDataShip::ModelDataShip(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
    mapProd.insert("elrtr",tr("ЭЛЕК."));
    mapProd.insert("wire",tr("ПРОВ."));
}

void ModelDataShip::refresh(int id_ship)
{
    QSqlQuery query;
    query.prepare("select z.id, z.prefix, z.parti, z.mark, z.massa, z.ds_status, z.r, z.id_part from ( "
                  "(select o.id, 'elrtr' as prefix, p.n_s||'-'||date_part('year',p.dat_part) as parti, "
                  "e.marka||' ф '||cast(p.diam as varchar(3))||CASE WHEN p.id_var <> 1 THEN (' /'::text || ev.nam::text) || '/'::text ELSE ''::text END AS mark, "
                  "o.massa as massa, o.ds_status as ds_status, "
                  "(select case when exists(select id_chem from sert_chem where id_part=p.id) then 1 else 0 end + "
                  "case when exists(select id_mech from sert_mech where id_part=p.id) then 2 else 0 end ) as r, p.id as id_part "
                  "from otpusk o inner join parti p on o.id_part=p.id "
                  "inner join elrtr e on e.id=p.id_el "
                  "inner join istoch i on i.id=p.id_ist "
                  "inner join elrtr_vars ev on ev.id=p.id_var "
                  "where o.id_sert = :id_ship order by p.n_s, p.dat_part) "
                  "union "
                  "(select w.id, 'wire', m.n_s||'-'||date_part('year',m.dat), "
                  "pr.nam||' ф '|| d.sdim ||' '||k.short ||' '|| CASE WHEN p.id_var <> 1 THEN (' /'::text || ev.nam::text) || '/'::text ELSE ''::text END AS mark, "
                  "w.m_netto, w.ds_status, "
                  "(select case when exists(select id from wire_parti_chem where id_part=(select p.id_m from wire_parti as p where p.id = w.id_wparti)) then 1 else 0 end + "
                  "case when exists(select id from wire_parti_mech where id_part=(select p.id_m from wire_parti as p where p.id = w.id_wparti)) then 2 else 0 end) as r, "
                  "p.id "
                  "from wire_shipment_consist w "
                  "inner join wire_parti p on p.id=w.id_wparti "
                  "inner join wire_parti_m m on p.id_m=m.id "
                  "inner join provol pr on pr.id=m.id_provol "
                  "inner join diam d on d.id=m.id_diam "
                  "inner join wire_pack_kind k on p.id_pack=k.id "
                  "inner join elrtr_vars ev on ev.id=p.id_var "
                  "where w.id_ship= :id_ship "
                  "order by pr.nam, d.sdim, k.short, m.n_s) "
                  ") as z order by z.prefix, z.mark, z.parti");
    query.bindValue(":id_ship",id_ship);
    if (execQuery(query)){
        setHeaderData(1, Qt::Horizontal,tr("Продукц."));
        setHeaderData(2, Qt::Horizontal,tr("Партия"));
        setHeaderData(3, Qt::Horizontal,tr("Марка"));
        setHeaderData(4, Qt::Horizontal,tr("Масса, кг"));
        setHeaderData(5, Qt::Horizontal,tr("ЭЦП"));
    }
}

QVariant ModelDataShip::data(const QModelIndex &index, int role) const
{
    if((role == Qt::BackgroundRole)&&(this->columnCount()>3)) {
        if (index.column()!=5){
            int area = record(index.row()).value(6).toInt();
            if(area == 0) {
                return QVariant(QColor(255,170,170));
            } else if(area == 1) {
                return QVariant(QColor(Qt::yellow));
            } else if(area == 2) {
                return QVariant(QColor(Qt::gray));
            } else if(area == 3) {
                return QVariant(QColor(170,255,170));
            }
        } else {
            int stat = record(index.row()).value(5).toInt();
            if(stat == 0) {
                return QVariant(QColor(255,170,170));
            } else if(stat == 1) {
                return QVariant(QColor(Qt::yellow));
            } else if(stat == 2) {
                return QVariant(QColor(170,255,170));
            }
        }
    }

    if (role == Qt::DisplayRole){
        if (index.column()==1){
            return mapProd.value(record(index.row()).value(1).toString(),record(index.row()).value(1).toString());
        } else if (index.column()==5){
            int stat = record(index.row()).value(5).toInt();
            if(stat == 1) {
                return QString("..");
            } else if(stat == 2) {
                return QString("OK");
            } else {
                return QString("-");
            }
        }
    }
    return ModelRo::data(index, role);
}
