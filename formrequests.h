#ifndef FORMREQUESTS_H
#define FORMREQUESTS_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormRequests;
}

class ModelReq : public DbTableModel
{
    Q_OBJECT

 public:
    explicit ModelReq(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end);
    void refresh(int moonth, int year);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
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

private slots:
    void loadSettings();
    void saveSettings();
    void updReq();
    void updData(int index);
    void switchFlt(bool b);
};

#endif // FORMREQUESTS_H
