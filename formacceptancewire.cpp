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

    actionPrintLblAll = new QAction("Напечатать все",this);
    actionPrintLblOne = new QAction("Напечатать одну",this);

    ui->toolButtonPal->addAction(actionPrintLblAll);
    ui->toolButtonPal->addAction(actionPrintLblOne);

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
    ui->tableViewAccData->setColumnHidden(5,true);

    mapper = new DbMapper(ui->tableViewAcc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);
    mapper->addEmptyLock(ui->tableViewAccData);
    mapper->addEmptyLock(ui->toolButtonPal);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));
    connect(ui->pushButtonUpdPart,SIGNAL(clicked(bool)),Models::instance()->relWirePart->model(),SLOT(refresh()));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(modelAcceptanceWireData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(actionPrintLblAll,SIGNAL(triggered(bool)),this,SLOT(printPalAll()));
    connect(actionPrintLblOne,SIGNAL(triggered(bool)),this,SLOT(printPalOne()));

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

void FormAcceptanceWire::printPalAll()
{
    if (!modelAcceptanceWireData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        LabelWirePal l(id_acc);
        l.printLabel();
    }
}

void FormAcceptanceWire::printPalOne()
{
    if (!modelAcceptanceWireData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        bool ok=false;
        int n=QInputDialog::getInt(this,tr("Ввод номера поддона"),tr("Введите номер поддона"),1,1,100,1,&ok);
        if (ok){
            LabelWirePal l(id_acc,n);
            l.printLabel();
        }
    }
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
    addColumn("chk",tr("check"));
    setDefaultValue(5,false);
    setSort(name()+".id");
    setDecimals(3,2);

    connect(this,SIGNAL(sigUpd()),this,SLOT(caclSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(caclSum()));
}

QVariant ModelAcceptanceWireData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundColorRole){
        if (DbTableModel::data(this->index(index.row(),5),Qt::EditRole).toBool()){
            return QColor(170,255,170);
        } else {
            return QVariant();
        }
    }
    return DbTableModel::data(index,role);
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

LabelWirePal::LabelWirePal(int id_acc, int cont, QObject *parent) : LabelBase("Этикетка_45*70_пров_поддон",45,70,2,parent)
{
    setPrintCmdMode(true);
    QSqlQuery query;
    QString filter="ww.id_waybill = :id_acc";
    if (cont>0){
        filter+=QString(" and ww.numcont = %1").arg(cont);
    }
    query.prepare("select 'EUR-'||wwbt.prefix||date_part('year',www.dat)||'-'||www.num ||'-'||numcont, "
                  "p.nam ||' ф '||d.sdim ||' '||wpk.short, wpm.n_s||' '||date_part('year',wpm.dat), ww.m_netto "
                  "from wire_warehouse ww "
                  "inner join wire_whs_waybill www on www.id = ww.id_waybill "
                  "inner join wire_way_bill_type wwbt on wwbt.id = www.id_type "
                  "inner join wire_parti wp on wp.id = ww.id_wparti "
                  "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                  "inner join provol p on p.id = wpm.id_provol "
                  "inner join diam d on d.id = wpm.id_diam "
                  "inner join wire_pack_kind wpk on wpk.id = wp.id_pack "
                  "where "+filter+" order by ww.id");
    query.bindValue(":id_acc",id_acc);
    if (query.exec()){
        while (query.next()){
            QString cnam=query.value(0).toString();
            accInfo info;
            info.nameNom=query.value(1).toString();
            info.namePart=query.value(2).toString();
            info.kvo=query.value(3).toDouble();
            hash.insert(cnam,info);
        }
        setCutKvo(hash.uniqueKeys().size());
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

QString LabelWirePal::getCod()
{
    QString lbl=LabelBase::getCod();
    lbl+=logo(2,2);
    QStringList pals=hash.uniqueKeys();
    bool first=true;
    for (QString pal : pals){
        if (!first){
            lbl+=cls();
        }
        lbl+=text(5,22,pal,11);
        lbl+=dataMatrix(16,27,15,0.85,pal);
        QList<accInfo> list = hash.values(pal);
        QString str;
        int n=1;
        for (accInfo a : list){
            if (!str.isEmpty()){
                str+="\n";
            }
            if (n>3){
                str+="...";
                break;
            }
            str+=a.nameNom+"\n п."+a.namePart+" - "+QLocale().toString(a.kvo,'f',1)+" кг";
            n++;
        }
        lbl+=block(2.5,44,40,23,str,10);
        lbl+=print(1);
        if (first){
            first=false;
        }
    }
    return lbl;
}
