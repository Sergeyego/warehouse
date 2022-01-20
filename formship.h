#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "dialogcods.h"
#include <QDomDocument>
#include "models.h"

namespace Ui {
class FormShip;
}

class ModelShip : public DbTableModel
{
    Q_OBJECT
public:
    ModelShip(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_pol=-1);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());

private:

};

class ModelShipEl : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipEl(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
};

class ModelShipWire : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipWire(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
};

class FormShip : public QWidget
{
    Q_OBJECT

public:
    explicit FormShip(bool readonly, QWidget *parent = 0);
    ~FormShip();

private:
    Ui::FormShip *ui;
    ModelShip *modelShip;
    DbMapper *push;
    ModelShipEl *modelShipEl;
    ModelShipWire *modelShipWire;
    void loadsettings();
    void savesettings();
    QDomElement newElement(QString nam, QString val,  QDomDocument *doc);

public slots:
    void setPartFilter();

private slots:
    void updShip();
    void setCurrentShip(int index);
    void sync();
    void goXml();
    void edtCods();
    void updPol();
    void updBalance();
};

#endif // FORMSHIP_H
