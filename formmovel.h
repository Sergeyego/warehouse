#ifndef FORMMOVEL_H
#define FORMMOVEL_H

#include <QWidget>
#include "tablemodel.h"
#include "executor.h"
#include <QSqlQueryModel>
#include <QDataWidgetMapper>

namespace Ui {
class FormMovEl;
}

class ModelPartElInfo : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartElInfo(QObject *parent=0);
    void refresh(int id_part);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

class FormMovEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormMovEl(QWidget *parent = 0);
    ~FormMovEl();

private:
    Ui::FormMovEl *ui;
    TableModel *modelPart;
    Executor *executor;
    ModelPartElInfo *modelPartElInfo;
    QDataWidgetMapper *mapperInfo;

private slots:
    void startUpd();
    void upd();
    void updInfo(QModelIndex index);
};

#endif // FORMMOVEL_H
