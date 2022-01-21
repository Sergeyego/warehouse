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

BalanceModel::BalanceModel(QObject *parent): QAbstractTableModel(parent)
{
    byp=true;
    headerPart<<"t"<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг";
    headerMark<<"t"<<"Номенклатура"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг";;
}

int BalanceModel::rowCount(const QModelIndex &/*parent*/) const
{
    return d.size();
}

int BalanceModel::columnCount(const QModelIndex &/*parent*/) const
{
    return byp ? headerPart.size() : headerMark.size();
}

QVariant BalanceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }
    QVariant origData=d[index.row()][index.column()];
    QVariant::Type type=origData.type();
    if (role==Qt::DisplayRole){
        if (type==QMetaType::Double){
            return (origData.isNull() || origData==0) ? QString("") : QLocale().toString(origData.toDouble(),'f',1);
        } else if (type==QMetaType::QDate){
            return (origData.isNull()) ? QString("") : origData.toDate().toString("dd.MM.yy");
        }
    } else if (role==Qt::TextAlignmentRole){
        return (type==QMetaType::Int || type==QMetaType::Double || type==QMetaType::Float || type==QMetaType::LongLong ) ?
                    int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role==Qt::EditRole || role == Qt::DisplayRole){
        return origData;
    }
    return QVariant();
}

QVariant BalanceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole && section>=0 && section<columnCount()){
        return byp ? headerPart.at(section) : headerMark.at(section);
    } else if (orientation==Qt::Vertical && role==Qt::DisplayRole){
        return QString("  ");
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void BalanceModel::refresh(QDate dat, bool bypart)
{
    byp=bypart;
    QMultiHash<QString,partInfo> h;
    Models::instance()->sync1C->getBalance(dat.addDays(1),h);
    beginResetModel();
    d.clear();
    if (byp){
        QMultiHash<QString, partInfo>::const_iterator i = h.constBegin();
        while (i != h.constEnd()) {
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
            row.push_back(Models::instance()->relKis->data(pinfo.id_kis,2));
            row.push_back(pinfo.name);
            row.push_back(pack);
            row.push_back(pinfo.number);
            row.push_back(pinfo.ist);
            row.push_back(pinfo.rcp);
            row.push_back(desc);
            row.push_back(pinfo.kvo);
            row.push_back(pinfo.prich);
            row.push_back(pinfo.rasch);
            d.push_back(row);
            ++i;
        }
    } else {
        QList<QString> mlist = h.uniqueKeys();
        for (QString kis : mlist){
            QVector<QVariant> row;
            row.push_back(Models::instance()->relKis->data(kis,2));
            row.push_back(Models::instance()->relKis->data(kis));
            double kvo=0;
            double prich=0;
            double rasch=0;
            QList <partInfo> plist = h.values(kis);
            for (partInfo pinfo : plist){
                kvo+=pinfo.kvo;
                prich+=pinfo.prich;
                rasch+=pinfo.rasch;
            }
            row.push_back(kvo);
            row.push_back(prich);
            row.push_back(rasch);
            d.push_back(row);
        }
    }
    endResetModel();
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
