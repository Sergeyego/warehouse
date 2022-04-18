#include "formbalance.h"
#include "ui_formbalance.h"

FormBalance::FormBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBalance)
{
    ui->setupUi(this);
    loadSettings();

    ui->tableViewPart->hide();
    ui->dateEdit->setDate(QDate::currentDate());

    partModel = new TableModel(this);
    proxyPartModel = new ProxyModel(this);
    proxyPartModel->setSourceModel(partModel);
    ui->tableViewPart->setModel(proxyPartModel);
    proxyPartModel->sort(2);

    balanceModel = new BalanceModel(this);
    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(balanceModel);
    proxyModel->sort(1);

    partModel->setHeader(balanceModel->getPartHeader());

    ui->tableView->setModel(proxyModel);
    ui->tableView->resizeToContents();
    setFilter();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(ui->checkBoxWire,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->tableView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updPart(QModelIndex)));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(createPackList()));
}

FormBalance::~FormBalance()
{
    saveSettings();
    delete ui;
}

void FormBalance::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("balance_splitter_width").toByteArray());
}

void FormBalance::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("balance_splitter_width",ui->splitter->saveState());
}

void FormBalance::refresh()
{
    bool byp = ui->radioButtonPart->isChecked();
    balanceModel->refresh(ui->dateEdit->date(),byp);
    if (byp){
        ui->tableView->setColumnHidden(12,true);
    }
    ui->tableView->resizeToContents();
    ui->tableViewPart->setHidden(byp);
    double sum=0;
    int col = byp? 6 : 1;
    for (int i=0; i<proxyModel->rowCount();i++){
        sum+=proxyModel->data(proxyModel->index(i,col),Qt::EditRole).toDouble();
    }
    ui->labelSum->setText("Итого: "+QLocale().toString(sum,'f',2)+" кг");
    if (ui->tableView->model()->rowCount()){
        ui->tableView->selectRow(0);
    }
}

void FormBalance::setFilter()
{
    proxyModel->setNomFilret(ui->checkBoxEl->isChecked(),ui->checkBoxWire->isChecked());
}

void FormBalance::save()
{
    ui->tableView->save(tr("Остатки на ")+ui->dateEdit->date().toString("dd.MM.yy"),1,true);
}

void FormBalance::updPart(QModelIndex index)
{
    if (!ui->radioButtonPart->isChecked()){
        QString kis=ui->tableView->model()->data(ui->tableView->model()->index(index.row(),0),Qt::EditRole).toString();
        QVector<QVector<QVariant>> data;
        balanceModel->getPartData(kis,data);
        partModel->setModelData(data);
        ui->tableViewPart->setColumnHidden(12,true);
        ui->tableViewPart->resizeToContents();
        if (ui->tableViewPart->model()->rowCount()){
            ui->tableViewPart->selectRow(0);
        }
    } else {
        partModel->clear();
    }
}

void FormBalance::createPackList()
{
    QString kis;
    QTableView *tableView = ui->radioButtonPart->isChecked() ? ui->tableView : ui->tableViewPart;
    QModelIndex ind=tableView->model()->index(tableView->currentIndex().row(),12);
    if (ind.isValid()){
        kis=tableView->model()->data(ind,Qt::EditRole).toString();
    }
    if (!kis.isEmpty()){
        qDebug()<<kis;
    }
}

BalanceModel::BalanceModel(QObject *parent): TableModel(parent)
{
    byp=true;
    headerPart<<"t"<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг"<<"Зона"<<"Ячейка"<<"Поддон"<<"id";
    headerMark<<"t"<<"Номенклатура"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг";
}

QVariant BalanceModel::data(const QModelIndex &index, int role) const
{
    if (index.column()==1 && role==Qt::DisplayRole){
        QString kis=TableModel::data(index,role).toString();
        return Models::instance()->relKis->data(kis);
    }
    return TableModel::data(index,role);
}

void BalanceModel::refresh(QDate dat, bool bypart)
{
    byp=bypart;
    Models::instance()->sync1C->getBalance(dat,part);
    Models::instance()->sync1C->getContBalance(dat,cont);
    QVector<QVector<QVariant>> tmpd;
    if (byp){
        QMultiHash<QString, partInfo>::const_iterator i = part.constBegin();
        while (i != part.constEnd()) {
            QVector<QVariant> row;
            partInfo pinfo=i.value();
            contInfo cinfo = cont.value(pinfo.contKey);
            row.push_back(Models::instance()->relKis->data(pinfo.id_kis,2));
            row.push_back(pinfo.id_kis);
            row.push_back(getPackName(pinfo.id_part_kis));
            row.push_back(pinfo.number);
            row.push_back(pinfo.ist);
            row.push_back(pinfo.rcp);
            row.push_back(getDesc(pinfo.id_part_kis,pinfo.desc));
            row.push_back(pinfo.kvo);
            row.push_back(pinfo.prich);
            row.push_back(pinfo.rasch);
            row.push_back(cinfo.zone);
            row.push_back(cinfo.cell);
            row.push_back(cinfo.name);
            row.push_back(pinfo.id_part_kis);
            tmpd.push_back(row);
            ++i;
        }
        setModelData(tmpd,headerPart);
    } else {
        QList<QString> mlist = part.uniqueKeys();
        for (QString kis : mlist){
            QVector<QVariant> row;
            row.push_back(Models::instance()->relKis->data(kis,2));
            row.push_back(kis);
            double kvo=0;
            double prich=0;
            double rasch=0;
            QList <partInfo> plist = part.values(kis);
            for (partInfo pinfo : plist){
                kvo+=pinfo.kvo;
                prich+=pinfo.prich;
                rasch+=pinfo.rasch;
            }
            row.push_back(kvo);
            row.push_back(prich);
            row.push_back(rasch);
            tmpd.push_back(row);
        }
        setModelData(tmpd,headerMark);
    }
}

QString BalanceModel::getPackName(QString id_part_kis)
{
    QStringList idpl = id_part_kis.split(":");
    QString pack;
    if (idpl.size()>1){
        QString id_part=idpl.at(1);
        if (idpl.at(0)=="w"){
            pack=Models::instance()->relWirePart->data(id_part,3).toString();
        } else if (idpl.at(0)=="e"){
            pack=Models::instance()->relElPart->data(id_part,3).toString();
        }
    }
    return pack;
}

QString BalanceModel::getDesc(QString id_part_kis, QString defval)
{
    QStringList idpl = id_part_kis.split(":");
    QString desc=defval;
    if (idpl.size()>1){
        QString id_part=idpl.at(1);
        if (idpl.at(0)=="e"){
            QString prim=Models::instance()->relElPart->data(id_part,4).toString();
            if (!prim.isEmpty()){
                desc=prim;
            }
        }
    }
    return desc;
}

QStringList BalanceModel::getPartHeader()
{
    return headerPart;
}

void BalanceModel::getPartData(QString kis, QVector<QVector<QVariant> > &data)
{
    QList<partInfo> list = part.values(kis);
    data.clear();
    for (partInfo i : list){
        QVector<QVariant> row;
        contInfo cnt = cont.value(i.contKey);
        row.push_back(Models::instance()->relKis->data(i.id_kis,2));
        row.push_back(i.name);
        row.push_back(getPackName(i.id_part_kis));
        row.push_back(i.number);
        row.push_back(i.ist);
        row.push_back(i.rcp);
        row.push_back(getDesc(i.id_part_kis,i.desc));
        row.push_back(i.kvo);
        row.push_back(i.prich);
        row.push_back(i.rasch);
        row.push_back(cnt.zone);
        row.push_back(cnt.cell);
        row.push_back(cnt.name);
        row.push_back(i.id_part_kis);
        data.push_back(row);
    }
}

ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

bool ProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const
{
    return source_column!=0;
}

void ProxyModel::setNomFilret(bool el, bool wire)
{
    QString pattern;
    setFilterKeyColumn(0);
    if (el){
        pattern+="e";
    }
    if (wire){
        if (!pattern.isEmpty()){
            pattern+="|";
        }
        pattern+="w";
    }
    if (pattern.isEmpty()){
        setFilterFixedString("-");
    } else {
        setFilterRegExp(pattern);
    }
}
