#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include "tablemodel.h"
#include <QSortFilterProxyModel>

namespace Ui {
class FormBalance;
}

class BalanceModel : public TableModel
{
        Q_OBJECT

public:
    explicit BalanceModel(QObject *parent=nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    void refresh(QDate dat, bool bypart=true);
    QList<partInfo> getPartList(QString key);
    contInfo getContInfo(QString contKey);

private:
    bool byp;
    QStringList headerPart;
    QStringList headerMark;

    QMultiHash<QString,partInfo> part;
    QHash<QString,contInfo> cont;
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
