#include "formmovel.h"
#include "ui_formmovel.h"

FormMovEl::FormMovEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMovEl)
{
    ui->setupUi(this);
    loadSettings();

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    executorPart = new Executor(this);
    executorOst = new Executor(this);

    modelPartElInfo = new ModelPartElInfo(this);
    modelPartElInfo->refresh(-1);

    mapperInfo = new QDataWidgetMapper(this);
    mapperInfo->setModel(modelPartElInfo);
    mapperInfo->addMapping(ui->lineEditIn,0);
    mapperInfo->addMapping(ui->lineEditShip,1);
    mapperInfo->addMapping(ui->lineEditOst,2);
    mapperInfo->addMapping(ui->checkBoxFinish,3);

    modelPressEl = new ModelPressEl(this);
    ui->tableViewPress->setModel(modelPressEl);

    modelPackEl = new ModelPackEl(this);
    ui->tableViewPack->setModel(modelPackEl);

    modelPerePackEl = new ModelPerePackEl(this);
    ui->tableViewPerePack->setModel(modelPerePackEl);

    modelBreakEl = new ModelBreakEl(this);
    ui->tableViewBreak->setModel(modelBreakEl);

    modelSelfEl = new ModelSelfEl(this);
    ui->tableViewSelf->setModel(modelSelfEl);

    modelStockEl = new ModelStockEl(this);
    ui->tableViewStock->setModel(modelStockEl);

    modelShipEl = new ModelShipEl(this);
    ui->tableViewShip->setModel(modelShipEl);

    modelOstEl = new ModelOstEl(this);
    ui->tableViewOst->setModel(modelOstEl);

    modelPart = new TableModel(this);

    QStringList header;
    header<<tr("id")<<tr("Партия")<<tr("Марка")<<tr("Ф")<<tr("Вариант")<<tr("Упаковка")<<tr("Источник")<<tr("Остаток");

    modelPart->setHeader(header);

    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->resizeToContents();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(executorPart,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->pushButtonCalcOst,SIGNAL(clicked(bool)),this,SLOT(calcOst()));

    connect(modelPressEl,SIGNAL(sigSum(QString)),ui->labelPress,SLOT(setText(QString)));
    connect(modelPackEl,SIGNAL(sigSum(QString)),ui->labelPack,SLOT(setText(QString)));
    connect(modelPerePackEl,SIGNAL(sigSum(QString)),ui->labelPerePack,SLOT(setText(QString)));
    connect(modelBreakEl,SIGNAL(sigSum(QString)),ui->labelBreak,SLOT(setText(QString)));
    connect(modelSelfEl,SIGNAL(sigSum(QString)),ui->labelSelf,SLOT(setText(QString)));
    connect(modelStockEl,SIGNAL(sigSum(QString)),ui->labelStock,SLOT(setText(QString)));
    connect(modelShipEl,SIGNAL(sigSum(QString)),ui->labelShip,SLOT(setText(QString)));

    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updInfo(QModelIndex)));
}

FormMovEl::~FormMovEl()
{
    saveSettings();
    delete ui;
}

void FormMovEl::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("mov_el_splitter_width").toByteArray());
}

void FormMovEl::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("mov_el_splitter_width",ui->splitter->saveState());
}

void FormMovEl::startUpd()
{
    QString query = QString("select p.id, p.n_s||'-'||date_part('year',p.dat_part) as part, e.marka, p.diam, ev.nam, ep.pack_ed, i.nam, c.kvo "
                            "from calc_parti_new('3000-01-01') as c "
                            "inner join parti as p on c.id_part=p.id "
                            "inner join el_pack as ep on ep.id=p.id_pack "
                            "inner join elrtr as e on p.id_el=e.id "
                            "inner join istoch as i on p.id_ist=i.id "
                            "inner join elrtr_vars ev on ev.id = p.id_var "
                            "where p.dat_part between '%1' and '%2' "
                            "order by p.n_s, p.dat_part").arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    executorPart->setQuery(query);
    executorPart->start();
}

void FormMovEl::upd()
{
    QVector<QVector<QVariant>> data = executorPart->getData();
    modelPart->setModelData(data);
    ui->tableViewPart->resizeToContents();
    if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->selectRow(0);
    }
}

void FormMovEl::updInfo(QModelIndex index)
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toInt();

    modelPartElInfo->refresh(id_part);
    mapperInfo->toFirst();

    modelPressEl->refresh(id_part);
    ui->tableViewPress->resizeToContents();

    modelPackEl->refresh(id_part);
    ui->tableViewPack->resizeToContents();

    modelPerePackEl->refresh(id_part);
    ui->tableViewPerePack->resizeToContents();

    modelBreakEl->refresh(id_part);
    ui->tableViewBreak->resizeToContents();

    modelSelfEl->refresh(id_part);
    ui->tableViewSelf->resizeToContents();

    modelStockEl->refresh(id_part);
    ui->tableViewStock->resizeToContents();

    modelShipEl->refresh(id_part);
    ui->tableViewShip->resizeToContents();

    modelOstEl->refresh(id_part);
    ui->tableViewOst->setColumnHidden(0,true);
    ui->tableViewOst->resizeToContents();
}

void FormMovEl::calcOst()
{
    DialogRest d;
    if (d.exec()==QDialog::Accepted){
        QDate date = d.getDate();
        if (date.month()!=date.addDays(1).month()){
            QString query=QString("select * from calc_rest_new('%1')").arg(date.toString("yyyy-MM-dd"));
            executorOst->setQuery(query);
            executorOst->start();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("Дата должна быть последним числом месяца."),QMessageBox::Ok);
        }
    }
}

ModelPartElInfo::ModelPartElInfo(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelPartElInfo::refresh(int id_part)
{
    QSqlQuery query;
    query.prepare("select c.sumIn, c.sumOt, c.kvoRs, p.fini, c.id_p from "
                  "calc_parti_one(:id_part, '3000-01-01') as c "
                  "inner join parti as p on c.id_p=p.id");
    query.bindValue(":id_part",id_part);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
    }
}

QVariant ModelPartElInfo::data(const QModelIndex &item, int role) const
{
    if (item.column()<3){
        if (role==Qt::DisplayRole || role==Qt::EditRole){
            QVariant origData = QSqlQueryModel::data(item,Qt::EditRole);
            return origData.isNull()? QString() : QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',1);
        } else if (role==Qt::TextAlignmentRole){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelPartElInfo::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok = false;
    if (index.column()==3){
        QSqlQuery query;
        query.prepare("update parti set fini = :val where id= :id");
        query.bindValue(":val",value.toBool());
        query.bindValue(":id",QSqlQueryModel::data(this->index(index.row(),4),Qt::EditRole).toInt());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return ok;
}

Qt::ItemFlags ModelPartElInfo::flags(const QModelIndex &index) const
{
    if (index.column()==3){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return QSqlQueryModel::flags(index);
}

ModelPressEl::ModelPressEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelPressEl::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Опрессовка");
    QSqlQuery query;
    query.prepare("select pp.dat, rb.snam, p.nam, pp.kvo "
                  "from part_prod pp "
                  "inner join rab_brig rb on rb.id = pp.id_brig "
                  "inner join pres p on p.id = pp.id_press "
                  "where pp.id_part = :id_part order by pp.dat");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("Бригадир"));
        setHeaderData(2,Qt::Horizontal,tr("Пресс"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelPackEl::ModelPackEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelPackEl::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Упаковка");
    QSqlQuery query;
    query.prepare("select pn.dat, pn.num, pp.kvo  from parti_pack pp "
                  "inner join parti_nakl pn on pn.id = pp.id_nakl "
                  "where pp.id_part = :id_part order by pn.dat, pn.num");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,2),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelPerePackEl::ModelPerePackEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelPerePackEl::refresh(int id_part)
{
    double sum=0;
    double sumBreak=0;
    QString title=tr("Переупаковка");
    QSqlQuery query;
    query.prepare(QString::fromUtf8("select * from ( "
                  "(select pn.dat as dat, pn.num as num, "
                  "(case when pp.id_new_part<>0 then'Переуп. в парт. '||p.n_s ||'-'||date_part('year',p.dat_part) else 'Брак при переуп.' end) as part, "
                  "pp.kvo*(-1) as kvo, pp.kvo_break as break "
                  "from parti_perepack pp "
                  "inner join parti_nakl pn on pn.id = pp.id_nakl "
                  "inner join parti p on p.id = pp.id_new_part "
                  "where pp.id_part = :id_part and  pn.tip = 7 ) "
                  "union "
                  "(select pn.dat as dat, pn.num as num, 'Переуп. из парт. '||p.n_s ||'-'||date_part('year',p.dat_part) as part, "
                  "pp.kvo as kvo, NULL as break "
                  "from parti_perepack pp "
                  "inner join parti_nakl pn on pn.id = pp.id_nakl "
                  "inner join parti p on p.id = pp.id_part "
                  "where pp.id_new_part = :id_new_part and  pn.tip = 7 ) "
                  ") as z order by z.dat, z.num"));
    query.bindValue(":id_part",id_part);
    query.bindValue(":id_new_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Операция"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        setHeaderData(4,Qt::Horizontal,tr("Брак, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
            sumBreak+=data(index(i,4),Qt::EditRole).toDouble();
        }
    }

    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    if (sumBreak!=0){
        s+=tr(" брак ")+QLocale().toString(sumBreak,'f',1)+tr(" кг");
    }
    emit sigSum(s);

}

ModelBreakEl::ModelBreakEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelBreakEl::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Брак");
    QSqlQuery query;
    query.prepare("select pn.dat, pn.num, pb.kvo "
                  "from parti_break pb "
                  "inner join parti_nakl pn on pn.id = pb.id_nakl "
                  "where pn.tip=2 and pb.id_part = :id_part "
                  "order by pn.dat, pn.num");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,2),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelSelfEl::ModelSelfEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelSelfEl::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Собственное потребление");
    QSqlQuery query;
    query.prepare("select ps.dat, ps.num, ps.kto, psi.kvo*sc.koef "
                  "from prod_self_items psi "
                  "inner join prod_self ps on ps.id = psi.id_self "
                  "inner join self_cons sc on sc.id = psi.id_cons "
                  "where psi.id_part = :id_part order by ps.dat, ps.num");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Куда"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelStockEl::ModelStockEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelStockEl::refresh(int id_part)
{
    QString title=tr("Передача на склад");
    QMap <QString,double> map;
    QSqlQuery query;
    query.prepare("select pn.dat, pn.num, i.nam, p.kvo*i.koef "
                  "from prod p "
                  "inner join prod_nakl pn on pn.id=p.id_nakl "
                  "inner join istoch i on i.id=pn.id_ist "
                  "where p.id_part = :id_part "
                  "order by pn.dat, pn.num");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Источник"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            QString key=data(index(i,2),Qt::EditRole).toString();
            double val=data(index(i,3),Qt::EditRole).toDouble();
            if (map.contains(key)){
                double oldval=map.value(key);
                map[key]=oldval+val;
            } else {
                map[key]=val;
            }
        }
    }
    QString s;
    QStringList list = map.keys();
    for (QString l : list) {
        if (!s.isEmpty()){
            s+=tr("; ");
        }
        s+=l+tr(": ")+QLocale().toString(map.value(l),'f',1)+tr(" кг");
    }
    emit sigSum(title+tr(" ")+s);
}

ModelShipEl::ModelShipEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
}

void ModelShipEl::refresh(int id_part)
{
    double sum=0;
    QString title=tr("Отгрузки");
    QSqlQuery query;
    query.prepare("select s.dat_vid, s.nom_s, p.short, o.massa "
                  "from otpusk o "
                  "inner join sertifikat s on o.id_sert=s.id "
                  "inner join poluch p on s.id_pol=p.id "
                  "where o.id_part = :id_part and s.id_type = 1 "
                  "order by s.dat_vid, s.nom_s");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("Дата"));
        setHeaderData(1,Qt::Horizontal,tr("№ нак."));
        setHeaderData(2,Qt::Horizontal,tr("Получатель"));
        setHeaderData(3,Qt::Horizontal,tr("К-во, кг"));
        for (int i=0; i<rowCount(); i++){
            sum+=data(index(i,3),Qt::EditRole).toDouble();
        }
    }
    QString s;
    s = (sum!=0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',1)+tr(" кг")) : title;
    emit sigSum(s);
}

ModelOstEl::ModelOstEl(QObject *parent) : ModelRo(parent)
{
    setDecimal(1);
    id_p=-1;
}

void ModelOstEl::refresh(int id_part)
{
    id_p=id_part;
    QSqlQuery query;
    query.prepare("select id_part, dat, kvo, calc, corr "
                  "from ostat "
                  "where id_part = :id_part "
                  "order by dat");
    query.bindValue(":id_part",id_part);
    if (execQuery(query)){
        setHeaderData(0,Qt::Horizontal,tr("id"));
        setHeaderData(1,Qt::Horizontal,tr("Дата"));
        setHeaderData(2,Qt::Horizontal,tr("Остаток, кг"));
        setHeaderData(3,Qt::Horizontal,tr("Расч. +, кг"));
        setHeaderData(4,Qt::Horizontal,tr("Корр., кг"));
    }
}

bool ModelOstEl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok = false;
    if (index.column()==2 && role==Qt::EditRole){
        QSqlQuery query;
        query.prepare("update ostat set kvo = :val where id_part= :id_part and dat = :dat ");
        query.bindValue(":val",value.toDouble());
        query.bindValue(":id_part",ModelRo::data(this->index(index.row(),0),Qt::EditRole).toInt());
        query.bindValue(":dat",ModelRo::data(this->index(index.row(),1),Qt::EditRole).toDate());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        }
    }
    if (index.column()==4 && role==Qt::EditRole){
        QSqlQuery query;
        query.prepare("update ostat set corr = :val where id_part= :id_part and dat = :dat ");
        query.bindValue(":val",value.toDouble());
        query.bindValue(":id_part",ModelRo::data(this->index(index.row(),0),Qt::EditRole).toInt());
        query.bindValue(":dat",ModelRo::data(this->index(index.row(),1),Qt::EditRole).toDate());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        }
    }
    if (ok){
        refresh(id_p);
    }
    return ok;
}

Qt::ItemFlags ModelOstEl::flags(const QModelIndex &index) const
{
    if (index.column()==2 || index.column()==4){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return ModelRo::flags(index);
}
