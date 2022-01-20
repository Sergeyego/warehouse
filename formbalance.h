#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include <QAbstractTableModel>

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

class FormBalance : public QWidget
{
    Q_OBJECT

public:
    explicit FormBalance(QWidget *parent = 0);
    ~FormBalance();

private:
    Ui::FormBalance *ui;
    BalanceModel *modelBalance;
private slots:
    void refresh();
};

#endif // FORMBALANCE_H
