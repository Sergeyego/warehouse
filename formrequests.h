#ifndef FORMREQUESTS_H
#define FORMREQUESTS_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"
#include "modelro.h"
#include "db/tablemodel.h"
#include "dialogreqload.h"

namespace Ui {
class FormRequests;
}

class ModelStat : public TableModel {
    Q_OBJECT

 public:
    explicit ModelStat(QWidget *parent = nullptr);
    void refresh(int id_req);
    QVariant data(const QModelIndex &item, int role) const;
    void setQuery(QString q);

private:
    int currentIdReq;
    QString strQuery;

public slots:
    void select();
};

class ModelReq : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelReq(QWidget *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    void refresh(QDate beg, QDate end);
    void refresh(int moonth, int year);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
public slots:
    void refreshState();
private:
    QMap<int,QColor> status;
    QString flt;
};

class ModelReqEl : public DbTableModel
{
    Q_OBJECT

 public:
    explicit ModelReqEl(QWidget *parent = nullptr);
    void refresh(int id_req);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    QVariant getIdPack(int id_el, int id_diam);

private slots:
    void calcSum();

signals:
    void sigSum(QString s);
};

class ModelReqWire : public DbTableModel
{
    Q_OBJECT

 public:
    explicit ModelReqWire(QWidget *parent = nullptr);
    void refresh(int id_req);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private slots:
    void calcSum();

signals:
    void sigSum(QString s);
};

class FormRequests : public QWidget
{
    Q_OBJECT

public:
    explicit FormRequests(QWidget *parent = nullptr);
    ~FormRequests();

private:
    Ui::FormRequests *ui;
    DbMapper *mapper;
    ModelReq *modelReq;
    ModelReqEl *modelReqEl;
    ModelReqWire *modelReqWire;
    DbTableModel *modelChanges;
    ModelStat *modelStatEl;
    ModelStat *modelStatWire;
    ModelRo *modelStatElData;
    ModelRo *modelStatWireData;

private slots:
    void loadSettings();
    void saveSettings();
    void updReq();
    void updData(int index);
    void updStatDataEl(QModelIndex index);
    void updStatDataWire(QModelIndex index);
    void switchFlt(bool b);
    void loadReq();
};

#endif // FORMREQUESTS_H
