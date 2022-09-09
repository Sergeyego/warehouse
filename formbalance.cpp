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

    calcSum();

    if (ui->tableView->model()->rowCount()){
        ui->tableView->selectRow(0);
    }
}

void FormBalance::calcSum()
{
    bool byp = ui->radioButtonPart->isChecked();
    double sum=0;
    int col = byp? 6 : 1;
    for (int i=0; i<proxyModel->rowCount();i++){
        sum+=proxyModel->data(proxyModel->index(i,col),Qt::EditRole).toDouble();
    }
    ui->labelSum->setText("Итого: "+QLocale().toString(sum,'f',2)+" кг");
}

void FormBalance::setFilter()
{
    proxyModel->setNomFilret(ui->checkBoxEl->isChecked(),ui->checkBoxWire->isChecked());
    calcSum();
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
    double kvo=0;
    QString cont;
    QTableView *tableView = ui->radioButtonPart->isChecked() ? ui->tableView : ui->tableViewPart;
    QModelIndex ind=tableView->model()->index(tableView->currentIndex().row(),12);
    if (ind.isValid()){
        kis=tableView->model()->data(ind,Qt::EditRole).toString();
        kvo=tableView->model()->data(tableView->model()->index(ind.row(),6),Qt::EditRole).toDouble();
        cont=tableView->model()->data(tableView->model()->index(ind.row(),11),Qt::EditRole).toString();
    }
    if (!kis.isEmpty()){
        QStringList lk=kis.split(":");
        if (lk.size()==2){
            int id_part=QString(lk.at(1)).toInt();
            if (lk.at(0)=="e"){
                PackElDoc doc(id_part,kvo,cont);
                DialogPrintPackList d(&doc);
                d.exec();
            } else {
                PackWireDoc doc(id_part,kvo,cont);
                DialogPrintPackList d(&doc);
                d.exec();
            }
        }
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
    updData(dat);
    QVector<QVector<QVariant>> tmpd;
    if (byp){
        QMultiHash<QString, partInfo>::const_iterator i = part.constBegin();
        while (i != part.constEnd()) {
            QVector<QVariant> row;
            partInfo pinfo=i.value();
            contInfo cinfo = cont.value(pinfo.contKey);
            BalanceModel::pData pd=partData.value(pinfo.id_part_kis);
            row.push_back(Models::instance()->relKis->data(pinfo.id_kis,2));
            row.push_back(pinfo.id_kis);
            row.push_back(pd.pack);
            row.push_back(pinfo.number);
            row.push_back(pinfo.ist);
            row.push_back(pinfo.rcp);
            row.push_back(pd.prim);
            row.push_back(pinfo.kvo);
            row.push_back(pinfo.prich);
            if (cinfo.rasch>0){
                row.push_back(pinfo.kvo);
            } else {
                row.push_back(pinfo.rasch);
            }
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
                contInfo cnt = cont.value(pinfo.contKey);
                if (cnt.rasch>0){
                    rasch+=pinfo.kvo;
                }

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

QStringList BalanceModel::getPartHeader()
{
    return headerPart;
}

void BalanceModel::updData(QDate dat)
{
    Models::instance()->sync1C->getBalance(dat,part);
    Models::instance()->sync1C->getContBalance(dat,cont);
    QList<partInfo> list = part.values();
    int maxide=0;
    int maxidw=0;
    int minide=10000000000;
    int minidw=10000000000;
    for (partInfo i : list){
        QStringList idl=i.id_part_kis.split(":");
        if (idl.size()>1){
            if (idl.at(0)=="e"){
                int ide=idl.at(1).toInt();
                if (ide>maxide){
                    maxide=ide;
                }
                if (ide<minide){
                    minide=ide;
                }
            } else if (idl.at(0)=="w"){
                int idw=idl.at(1).toInt();
                if (idw>maxidw){
                    maxidw=idw;
                }
                if (idw<minidw){
                    minidw=idw;
                }
            }
        }
    }
    QSqlQuery query;
    query.prepare("(select 'e:'||p.id, ep.pack_ed, "
                  "CASE WHEN p.id_var<>1 THEN '/'||ev.nam ||'/ ' ELSE '' END || p.prim_prod from parti p "
                  "inner join el_pack ep on ep.id = p.id_pack "
                  "inner join elrtr_vars ev on ev.id = p.id_var "
                  "where p.id between :minide and :maxide "
                  ") union ("
                  "select 'w:'||wp.id, CASE WHEN (COALESCE(wp2.mas_ed,0)<>0) THEN (' (' || COALESCE(wp2.mas_ed,0) || ' кг)') ELSE '' end, wp.prim_prod "
                  "from wire_parti wp "
                  "inner join wire_pack wp2 on wp2.id = wp.id_pack_type "
                  "where wp.id between :minidw and :maxidw )");
    query.bindValue(":minide",minide);
    query.bindValue(":maxide",maxide);
    query.bindValue(":minidw",minidw);
    query.bindValue(":maxidw",maxidw);
    if (query.exec()){
        partData.clear();
        while (query.next()){
            BalanceModel::pData pd;
            pd.pack=query.value(1).toString();
            pd.prim=query.value(2).toString();
            partData.insert(query.value(0).toString(),pd);
        }
    } else {
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
}

void BalanceModel::getPartData(QString kis, QVector<QVector<QVariant> > &data)
{
    QList<partInfo> list = part.values(kis);
    data.clear();
    for (partInfo i : list){
        BalanceModel::pData pd=partData.value(i.id_part_kis);
        QVector<QVariant> row;
        contInfo cnt = cont.value(i.contKey);
        row.push_back(Models::instance()->relKis->data(i.id_kis,2));
        row.push_back(i.name);
        row.push_back(pd.pack);
        row.push_back(i.number);
        row.push_back(i.ist);
        row.push_back(i.rcp);
        row.push_back(pd.prim);
        row.push_back(i.kvo);
        row.push_back(i.prich);
        if (cnt.rasch>0){
            row.push_back(i.kvo);
        } else {
            row.push_back(i.rasch);
        }
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
