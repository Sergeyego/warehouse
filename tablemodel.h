#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QLocale>
#include <QDate>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:    
    TableModel(QObject *parent=nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setModelData(const QVector<QVector<QVariant>> &data, const QStringList &hdata = QStringList());
    void setHeader(const QStringList &hdata);
    void clear();

private:
    QVector<QVector<QVariant>> p_d;
    QStringList p_header;
};

#endif // TABLEMODEL_H
