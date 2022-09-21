#ifndef FORMSHIPCONS_H
#define FORMSHIPCONS_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"
#include "dialogcods.h"
#include "dialogshipload.h"
#include <QDomDocument>

namespace Ui {
class FormShipCons;
}

class ModelShipCons : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipCons(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_pol=-1);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
};

class ModelShipConsEl : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipConsEl(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private slots:
    void calcSum();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
    void sigSum(QString s);
};

class ModelShipConsWire : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipConsWire(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private slots:
    void calcSum();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
    void sigSum(QString s);
};

class FormShipCons : public QWidget
{
    Q_OBJECT

public:
    explicit FormShipCons(QWidget *parent = 0);
    ~FormShipCons();

private:
    Ui::FormShipCons *ui;
    void loadsettings();
    void savesettings();
    ModelShipCons *modelShip;
    DbMapper *mapper;
    ModelShipConsEl *modelEl;
    ModelShipConsWire *modelWire;
    QDomElement newElement(QString nam, QString val, QDomDocument *doc);

private slots:
    void updShip();
    void updPol();
    void setCurrentShip(int index);
    void edtCods();
    void goXml();
    void loadData();
};

#endif // FORMSHIPCONS_H
