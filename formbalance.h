#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

namespace Ui {
class FormBalance;
}

class BalanceModel : public QAbstractTableModel
{
        Q_OBJECT

public:
    explicit BalanceModel(QObject *parent=nullptr);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void refresh(QDate dat, bool bypart=true);
private:
    QVector<QVector<QVariant>> d;
    bool byp;
    QStringList headerPart;
    QStringList headerMark;
};

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ProxyModel(QObject *parent=nullptr);
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    void setNomFilret(bool el, bool wire);
};

class FormBalance : public QWidget
{
    Q_OBJECT

public:
    explicit FormBalance(QWidget *parent = 0);
    ~FormBalance();

private:
    Ui::FormBalance *ui;
    BalanceModel *modelBalance;
    ProxyModel *proxyModel;
private slots:
    void refresh();
    void setFilter();
    void save();
};

#endif // FORMBALANCE_H
