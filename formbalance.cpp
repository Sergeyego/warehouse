#include "formbalance.h"
#include "ui_formbalance.h"

FormBalance::FormBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBalance)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    modelBalance = new BalanceModel(this);
    ui->tableView->setModel(modelBalance);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refresh()));
}

FormBalance::~FormBalance()
{
    delete ui;
}

void FormBalance::refresh()
{
    modelBalance->refresh(ui->dateEdit->date(),ui->radioButtonPart->isChecked());
}

BalanceModel::BalanceModel(QObject *parent): QAbstractTableModel(parent)
{
    byp=true;
    headerPart<<"id_part_kis"<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг";
    headerMark<<"id_kis"<<"Номенклатура"<<"Количество, кг";
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
    if (role==Qt::EditRole || role == Qt::DisplayRole){
        return d[index.row()][index.column()];
    }
    return QVariant();
}

QVariant BalanceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole && section>=0 && section<columnCount()){
        return byp ? headerPart.at(section) : headerMark.at(section);
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void BalanceModel::refresh(QDate dat, bool bypart)
{
    byp=bypart;
    QMultiHash<markInfo,partInfo> h;
    Models::instance()->sync1C->getBalance(dat.addDays(1),h);
    if (byp){
        beginResetModel();
        d.clear();
        QMultiHash<markInfo, partInfo>::const_iterator i = h.constBegin();
        while (i != h.constEnd()) {
            QVector<QVariant> row;
            partInfo pinfo=i.value();
            row.push_back(pinfo.id_part_kis);
            row.push_back(pinfo.name);
            row.push_back(pinfo.packName);
            row.push_back(pinfo.number);
            row.push_back(pinfo.ist);
            row.push_back(pinfo.rcp);
            row.push_back(pinfo.desc);
            row.push_back(pinfo.kvo);
            d.push_back(row);
            ++i;
        }
        endResetModel();
    }
}
