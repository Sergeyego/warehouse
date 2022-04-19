#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include "tablemodel.h"
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
    QVariant data(const QModelIndex &index, int role) const;
    void refresh(QDate dat, bool bypart=true);
    QString getPackName(QString id_part_kis);
    QString getDesc(QString id_part_kis, QString defval=QString());
    void getPartData(QString kis, QVector<QVector<QVariant>> &data);
    QStringList getPartHeader();

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
    BalanceModel *balanceModel;
    ProxyModel *proxyModel;
    TableModel *partModel;
    ProxyModel *proxyPartModel;
    void loadSettings();
    void saveSettings();
private slots:
    void refresh();
    void setFilter();
    void save();
    void updPart(QModelIndex index);
    void createPackList();
};

#endif // FORMBALANCE_H
