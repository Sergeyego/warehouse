#ifndef FORMBALANCEEW_H
#define FORMBALANCEEW_H

#include <QWidget>
#include "tablemodel.h"
#include <QSqlQuery>
#include <QSortFilterProxyModel>

namespace Ui {
class FormBalanceEW;
}

class FormBalanceEW : public QWidget
{
    Q_OBJECT

public:
    explicit FormBalanceEW(bool e, bool w, QWidget *parent = 0);
    ~FormBalanceEW();

private:
    Ui::FormBalanceEW *ui;
    TableModel *modelMark;
    TableModel *modelPart;
    QSortFilterProxyModel *proxyModelPart;
    bool en_el;
    bool en_wire;
    double loadData(QVector<QVector<QVariant>> &data, QMultiMap<QString,double> &map, QSqlQuery &query);

private slots:
    void upd();
    void setByPart();
    void selectMark(QModelIndex index);
    void save();
};

#endif // FORMBALANCEEW_H
