#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include "db/tablemodel.h"
#include <QSortFilterProxyModel>
#include "dialogprintpacklist.h"

namespace Ui {
class FormBalance;
}

class BalanceModel : public TableModel
{
        Q_OBJECT

public:
    explicit BalanceModel(QObject *parent=nullptr);
    void refresh(QDate dat, bool bypart=true, bool otOnly=false);
    void getPartData(QString kis, QVector<QVector<QVariant>> &data, bool otOnly=false);
    QStringList getPartHeader();

private:
    void updData(QDate dat);
    bool byp;
    QStringList headerPart;
    QStringList headerMark;
    QStringList zoneOt;
    struct pData {
        QString pack;
        QString prim;
    };
    QHash<QString,pData> partData;
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
    BalanceModel *balanceModel;
    ProxyModel *proxyModel;
    TableModel *partModel;
    ProxyModel *proxyPartModel;
    void loadSettings();
    void saveSettings();
private slots:
    void refresh();
    void calcSum();
    void setFilter();
    void save();
    void updPart(QModelIndex index);
    void createPackList();
};

#endif // FORMBALANCE_H
