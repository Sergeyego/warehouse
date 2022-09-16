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
    ui->comboBoxPart->setCurrentIndex(Models::instance()->relElPart->currentFilter());

    actionPrintLblAll = new QAction("Напечатать все",this);
    actionPrintLblOne = new QAction("Напечатать одну",this);

    ui->toolButtonPal->addAction(actionPrintLblAll);
    ui->toolButtonPal->addAction(actionPrintLblOne);

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
    mapper->addEmptyLock(ui->toolButtonPal);
    mapper->addEmptyLock(ui->pushButtonNakl);
    mapper->addLock(ui->pushButtonUpd);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updAcc()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updAccData(int)));
    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),Models::instance()->relElPart,SLOT(setFilter(int)));
    connect(ui->pushButton1C,SIGNAL(clicked(bool)),this,SLOT(sync()));
    connect(modelAcceptanceElData,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(actionPrintLblAll,SIGNAL(triggered(bool)),this,SLOT(printPalAll()));
    connect(actionPrintLblOne,SIGNAL(triggered(bool)),this,SLOT(printPalOne()));
    connect(ui->pushButtonNakl,SIGNAL(clicked(bool)),this,SLOT(printNakl()));
    connect(Models::instance()->relElPart,SIGNAL(filterChanged(int)),this,SLOT(setCurrentFilter(int)));

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
    QDate minDate=ui->dateEditBeg->date().addYears(-4);
    QSqlQuery query;
    query.prepare("select min(p2.dat_part) from prod_nakl pn "
                  "inner join prod p on p.id_nakl = pn.id "
                  "inner join parti p2 on p2.id = p.id_part "
                  "where pn.dat between :d1 and :d2");
    query.bindValue(":d1",ui->dateEditBeg->date());
    query.bindValue(":d2",ui->dateEditEnd->date());
    if (query.exec()){
        query.next();
        QDate dt=query.value(0).toDate();
        if (dt<minDate){
            minDate=dt;
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    Models::instance()->modelElPart->setMinDate(minDate,(sender()==ui->pushButtonUpd));
    modelAcceptanceEl->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormAcceptanceEl::updAccData(int index)
{
    int id_acc=mapper->modelData(index,0).toInt();
    modelAcceptanceElData->refresh(id_acc);
}

void FormAcceptanceEl::sync()
{
    Models::instance()->sync1C->syncPriemEl(mapper->modelData(mapper->currentIndex(),0).toInt());
}

void FormAcceptanceEl::printPalAll()
{
    if (!modelAcceptanceElData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        LabelElPal l(id_acc);
        l.printLabel();
    }
}

void FormAcceptanceEl::printPalOne()
{
    if (!modelAcceptanceElData->isEmpty()){
        int id_acc=mapper->modelData(mapper->currentIndex(),0).toInt();
        bool ok=false;
        int n=QInputDialog::getInt(this,tr("Ввод номера поддона"),tr("Введите номер поддона"),1,1,100,1,&ok);
        if (ok){
            LabelElPal l(id_acc,n);
            l.printLabel();
        }
    }
}

void FormAcceptanceEl::printNakl()
{
    QString id_ist=modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),3),Qt::EditRole).toString();
    QString year=QString::number(modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),2),Qt::EditRole).toDate().year());
    QString num=modelAcceptanceEl->data(modelAcceptanceEl->index(mapper->currentIndex(),1),Qt::EditRole).toString();
    QString kis=modelAcceptanceEl->relation(3)->data(id_ist,2).toString()+year+"-"+num;

    PackNaklDoc doc(kis);
    DialogPrintPackList d(&doc);
    d.setWindowTitle("Накладная "+kis);
    d.setSingle(false);
    d.exec();
}

void FormAcceptanceEl::setCurrentFilter(int ind)
{
    ui->comboBoxPart->blockSignals(true);
    ui->comboBoxPart->setCurrentIndex(ind);
    ui->comboBoxPart->blockSignals(false);
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
    if (role==Qt::BackgroundColorRole){
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

LabelElPal::LabelElPal(int id_acc, int cont, QObject *parent) : LabelBase("Этикетка_50*40_электр_поддон",50,40,2.5,parent)
{
    setPrintCmdMode(true);
    QSqlQuery query;
    QString filter="p.id_nakl = :id_acc";
    if (cont>0){
        filter+=QString(" and p.numcont = %1").arg(cont);
    }
    query.prepare("select 'EUR-'||pnt.prefix||date_part('year',p.dat)||'-'||p.docs||'-'||p.numcont, e.marka||' ф'||p2.diam::numeric(2,1), "
                  "p2.n_s||'-'|| date_part('year',p2.dat_part), p.kvo, p.barcodecont "
                  "from prod p "
                  "inner join parti p2 on p2.id = p.id_part "
                  "inner join elrtr e on e.id = p2.id_el "
                  "inner join prod_nakl_tip pnt on pnt.id = p.id_ist "
                  "where "+filter+" order by p.id ");
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

QString LabelElPal::getCod()
{
    QString lbl=LabelBase::getCod();
    QStringList pals=hash.uniqueKeys();
    bool first=true;
    for (QString pal : pals){
        if (!first){
            lbl+=cls();
        }
        lbl+=text(9,3,pal,11);
        lbl+=dataMatrix(17,9,15,0.85,pal);
        QList<accInfo> list = hash.values(pal);
        QString str;
        int n=1;
        for (accInfo a : list){
            if (!str.isEmpty()){
                str+="\n";
            }
            if (n>2){
                str+="...";
                break;
            }
            str+=a.nameNom+"\n п."+a.namePart+" - "+QLocale().toString(a.kvo,'f',1)+" кг";
            n++;
        }
        lbl+=block(2.5,25,45,14,str,10);
        lbl+=print(1);
        if (first){
            first=false;
        }
    }
    return lbl;
}
