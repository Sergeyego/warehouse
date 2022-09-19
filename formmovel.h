#ifndef FORMMOVEL_H
#define FORMMOVEL_H

#include <QWidget>
#include "tablemodel.h"
#include "executor.h"
#include <QSqlQueryModel>
#include <QDataWidgetMapper>
#include "modelro.h"
#include "dialogrest.h"

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

class ModelPressEl : public ModelRo
{
    Q_OBJECT
public:
    ModelPressEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelPackEl : public ModelRo
{
    Q_OBJECT
public:
    ModelPackEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelPerePackEl : public ModelRo
{
    Q_OBJECT
public:
    ModelPerePackEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelBreakEl : public ModelRo
{
    Q_OBJECT
public:
    ModelBreakEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelSelfEl : public ModelRo
{
    Q_OBJECT
public:
    ModelSelfEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelStockEl : public ModelRo
{
    Q_OBJECT
public:
    ModelStockEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelShipEl : public ModelRo
{
    Q_OBJECT
public:
    ModelShipEl(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelOstEl : public ModelRo
{
    Q_OBJECT
public:
    ModelOstEl(QObject *parent=0);
    void refresh(int id_part);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    int id_p;
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
    Executor *executorPart;
    Executor *executorOst;
    ModelPartElInfo *modelPartElInfo;
    QDataWidgetMapper *mapperInfo;
    ModelPressEl *modelPressEl;
    ModelPackEl *modelPackEl;
    ModelPerePackEl *modelPerePackEl;
    ModelBreakEl *modelBreakEl;
    ModelSelfEl *modelSelfEl;
    ModelStockEl *modelStockEl;
    ModelShipEl *modelShipEl;
    ModelOstEl *modelOstEl;
    void loadSettings();
    void saveSettings();

private slots:
    void startUpd();
    void upd();
    void updInfo(QModelIndex index);
    void calcOst();
};

#endif // FORMMOVEL_H
