#include "tablemodel.h"

TableModel::TableModel(QObject *parent): QAbstractTableModel(parent)
{

}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }
    QVariant origData=p_d[index.row()][index.column()];
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

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return p_d.size();
}

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return p_header.size();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole && section>=0 && section<columnCount()){
        return p_header.at(section);
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void TableModel::setModelData(const QVector<QVector<QVariant> > &data, const QStringList &hdata)
{
    beginResetModel();
    p_d=data;
    if(!hdata.isEmpty()){
        p_header=hdata;
    }
    endResetModel();
    emit sigUpd();
}

void TableModel::setHeader(const QStringList &hdata)
{
    beginResetModel();
    p_header=hdata;
    endResetModel();
}

void TableModel::clear()
{
    if (rowCount()){
        beginResetModel();
        p_d.clear();
        endResetModel();
    }
}