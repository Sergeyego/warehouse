#include "formbalance.h"
#include "ui_formbalance.h"

FormBalance::FormBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBalance)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    modelBalance = new BalanceModel(this);
    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(modelBalance);
    proxyModel->sort(1);

    ui->tableView->setModel(proxyModel);
    ui->tableView->resizeToContents();
    setFilter();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(ui->checkBoxWire,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormBalance::~FormBalance()
{
    delete ui;
}

void FormBalance::refresh()
{
    modelBalance->refresh(ui->dateEdit->date(),ui->radioButtonPart->isChecked());
    ui->tableView->resizeToContents();
}

void FormBalance::setFilter()
{
    proxyModel->setNomFilret(ui->checkBoxEl->isChecked(),ui->checkBoxWire->isChecked());
}

void FormBalance::save()
{
    ui->tableView->save(tr("Остатки на ")+ui->dateEdit->date().toString("dd.MM.yy"),1,true);
}

BalanceModel::BalanceModel(QObject *parent): TableModel(parent)
{
    byp=true;
    headerPart<<"t"<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг"<<"Зона"<<"Ячейка"<<"Поддон";
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
    Models::instance()->sync1C->getBalance(dat.addDays(1),part);
    Models::instance()->sync1C->getContBalance(dat.addDays(1),cont);
    QVector<QVector<QVariant>> tmpd;
    if (byp){
        QMultiHash<QString, partInfo>::const_iterator i = part.constBegin();
        while (i != part.constEnd()) {
            QVector<QVariant> row;
            partInfo pinfo=i.value();
            QStringList idpl = pinfo.id_part_kis.split(":");
            QString pack;
            QString desc=pinfo.desc;
            if (idpl.size()>1){
                QString id_part=idpl.at(1);
                if (idpl.at(0)=="w"){
                    pack=Models::instance()->relWirePart->data(id_part,3).toString();
                } else if (idpl.at(0)=="e"){
                    pack=Models::instance()->relElPart->data(id_part,3).toString();
                    QString prim=Models::instance()->relElPart->data(id_part,4).toString();
                    if (!prim.isEmpty()){
                        desc=prim;
                    }
                }
            }
            contInfo cinfo = cont.value(pinfo.contKey);
            row.push_back(Models::instance()->relKis->data(pinfo.id_kis,2));
            row.push_back(pinfo.id_kis);
            row.push_back(pack);
            row.push_back(pinfo.number);
            row.push_back(pinfo.ist);
            row.push_back(pinfo.rcp);
            row.push_back(desc);
            row.push_back(pinfo.kvo);
            row.push_back(pinfo.prich);
            row.push_back(pinfo.rasch);
            row.push_back(cinfo.zone);
            row.push_back(cinfo.cell);
            row.push_back(cinfo.name);
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

QList<partInfo> BalanceModel::getPartList(QString kis)
{
    return part.values(kis);
}

contInfo BalanceModel::getContInfo(QString contKey)
{
    return cont.value(contKey);
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
