#include "formacceptancewire.h"
#include "ui_formacceptancewire.h"

FormAcceptanceWire::FormAcceptanceWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptanceWire)
{
    ui->setupUi(this);
    ui->comboBoxLblType->addItem(tr("Этикетка 45*70"));
    ui->comboBoxLblType->addItem(tr("Этикетка 50*40"));

    loadsettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    //ui->comboBoxPart->setCurrentIndex(Models::instance()->relWirePart->currentFilter());

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
    ui->tableViewAccData->setColumnWidth(5,130);
    ui->tableViewAccData->setColumnHidden(6,true);

    mapper = new DbMapper(ui->tableViewAcc,this);
    ui->horizontalLayoutMapper->insertWidget(0,mapper);
    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxType,3);
    mapper->addEmptyLock(ui->tableViewAccData);
    mapper->addEmptyLock(ui->toolButtonPal);
    mapper->addEmptyLock(ui->pushButtonNakl);
    mapper->addLock(ui->pushButtonUpd);

    //connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),Models::instance()->relWirePart,SLOT(setFilter(int)));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(modelAcceptanceWireData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(actionPrintLblAll,SIGNAL(triggered(bool)),this,SLOT(printPalAll()));
    connect(actionPrintLblOne,SIGNAL(triggered(bool)),this,SLOT(printPalOne()));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
    //connect(Models::instance()->relWirePart,SIGNAL(filterChanged(int)),this,SLOT(setCurrentFilter(int)));

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
    ui->comboBoxLblType->setCurrentIndex(settings.value("wire_cont_lbl_type",0).toInt());
}

void FormAcceptanceWire::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("accwire_splitter_width",ui->splitter->saveState());
    settings.setValue("wire_cont_lbl_type",ui->comboBoxLblType->currentIndex());
}

void FormAcceptanceWire::printPal(int id_acc, int cont)
{
    double w,h,g;
    if (ui->comboBoxLblType->currentIndex()==0){
        w=45;
        h=70;
        g=2;
    } else {
        w=50;
        h=40;
        g=2.5;
    }
    LabelWirePal l(id_acc,cont,w,h,g);
    l.printLabel();
}

void FormAcceptanceWire::updAcc()
{
    if (sender()==ui->pushButtonUpd){
        modelAcceptanceWire->refreshRelsModel();
        modelAcceptanceWireData->refreshRelsModel();
    }
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

void FormAcceptanceWire::printPalAll()
{
    if (!modelAcceptanceWireData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        printPal(id_acc,-1);
    }
}

void FormAcceptanceWire::printPalOne()
{
    if (!modelAcceptanceWireData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        bool ok=false;
        int n=QInputDialog::getInt(this,tr("Ввод номера поддона"),tr("Введите номер поддона"),1,1,100,1,&ok);
        if (ok){
            printPal(id_acc,n);
        }
    }
}

void FormAcceptanceWire::printNakl()
{
    QString id_ist=modelAcceptanceWire->data(modelAcceptanceWire->index(mapper->currentIndex(),3),Qt::EditRole).toString();
    QString year=QString::number(modelAcceptanceWire->data(modelAcceptanceWire->index(mapper->currentIndex(),2),Qt::EditRole).toDate().year());
    QString num=modelAcceptanceWire->data(modelAcceptanceWire->index(mapper->currentIndex(),1),Qt::EditRole).toString();
    QString kis=modelAcceptanceWire->sqlRelation(3)->getDisplayValue(id_ist,"prefix")+year+"-"+num;

    PackNaklDoc doc(kis);
    DialogPrintPackList d(&doc);
    d.setWindowTitle("Накладная "+kis);
    d.setSingle(false);
    d.exec();
}

void FormAcceptanceWire::setCurrentFilter(int ind)
{
    ui->comboBoxPart->blockSignals(true);
    ui->comboBoxPart->setCurrentIndex(ind);
    ui->comboBoxPart->blockSignals(false);
}

ModelAcceptanceWire::ModelAcceptanceWire(QObject *parent) : DbTableModel("wire_whs_waybill",parent)
{
    addColumn("id","id");
    addColumn("num",tr("Номер"));
    addColumn("dat",tr("Дата"));
    addColumn("id_type",tr("Тип"),Models::instance()->relAccTypeWire);
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
    addColumn("barcodecont",tr("Штрихкод поддона"));
    addColumn("chk",tr("check"));
    setDefaultValue(6,false);
    setSort(name()+".id");
    setDecimals(3,2);

    connect(this,SIGNAL(sigUpd()),this,SLOT(caclSum()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(caclSum()));
}

QVariant ModelAcceptanceWireData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundColorRole){
        if (DbTableModel::data(this->index(index.row(),6),Qt::EditRole).toBool()){
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

Qt::ItemFlags ModelAcceptanceWireData::flags(const QModelIndex &index) const
{
    if ((index.column()==5 || index.column()==2) && DbTableModel::data(this->index(index.row(),6),Qt::EditRole).toBool()){
        return Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return DbTableModel::flags(index);
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

LabelWirePal::LabelWirePal(int id_acc, int cont, double w, double h, double g, QObject *parent) : LabelBase("Этикетка_пров_поддон",w,h,g,parent)
{
    setPrintCmdMode(true);
    QSqlQuery query;
    QString filter="ww.id_waybill = :id_acc";
    if (cont>0){
        filter+=QString(" and ww.numcont = %1").arg(cont);
    }
    query.prepare("select 'EUR-'||wwbt.prefix||date_part('year',www.dat)||'-'||www.num ||'-'||numcont, "
                  "p.nam ||' ф '||d.sdim ||' '||wpk.short, wpm.n_s||' '||date_part('year',wpm.dat), ww.m_netto, ww.barcodecont "
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
            QString cnam=query.value(4).toString().isEmpty() ? query.value(0).toString() : query.value(4).toString();
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
    QStringList pals=hash.uniqueKeys();
    bool first=true;
    for (QString pal : pals){
        if (!first){
            lbl+=cls();
        } else {
            first=false;
        }
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
        if (getHeight()>69){
            lbl+=logo(2,2);
            lbl+=text(5,22,pal,11);
            lbl+=dataMatrix(16,27,15,0.85,pal);
            lbl+=block(2.5,44,40,23,str,10);
        } else {
            lbl+=text(9,3,pal,11);
            lbl+=dataMatrix(17,9,15,0.85,pal);
            lbl+=block(2.5,25,45,14,str,10);
        }
        lbl+=print(1);
    }
    return lbl;
}
