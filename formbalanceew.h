#ifndef FORMBALANCEEW_H
#define FORMBALANCEEW_H

#include <QWidget>
#include "tablemodel.h"
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QInputDialog>
#include "executor.h"

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
    Executor *sqlExecutor;

private slots:
    void startUpd();
    void upd();
    void setByPart();
    void selectMark(QModelIndex index);
    void save();
    void edtDesc(QModelIndex index);
};

#endif // FORMBALANCEEW_H
