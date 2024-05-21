#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "dialogcods.h"
#include "models.h"
#include "db/tablemodel.h"
#include "dialogwebview.h"

namespace Ui {
class FormShip;
}

class ModelBalance;

struct shipContInfo {
    QString tablename;
    QString namId;
    QString namIdDoc;
    QString namKis;
    QString namIdPart;
    QString namKvo;
    QString prefix;
    ModelBalance *modelBalence;
    DbSqlRelation *relPart;
};

class ModelBalance : public TableModel
{
    Q_OBJECT
public:
    ModelBalance(QObject *parent=0);
    void refresh(QString kis);
    void updData(QDate dat);
    double getStock(QString ide);
    void clear();
private:
    struct pData {
        QString pack;
        QString prim;
    };
    QHash<QString,pData> partData;
    QMultiHash<QString,partInfo> part;
    QHash<QString,contInfo> cont;
};

class ModelShip : public DbTableModel
{
    Q_OBJECT
public:
    ModelShip(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_pol=-1);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
};

class ModelShipData : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipData(shipContInfo c, QObject *parent=0);
    void refresh(int id_ship);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
    void setFlt(QString kis);
    bool insertRow(int row, const QModelIndex &parent);

public slots:
    double getStock(QModelIndex index);
    void setPartFlt(int ind);
    void setOstControl(bool b);

private:
    shipContInfo info;
    int currentIdShip;
    int fltind;
    bool ostControl;

private slots:
    void calcSum();

signals:
    void sigStock(QString mes);
    void sigSum(QString s);
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
    ModelShipData *modelShipEl;
    ModelShipData *modelShipWire;
    ModelBalance *modelBalance;
    TableModel *modelElStat;
    TableModel *modelWireStat;
    QSortFilterProxyModel *proxyModelBalance;
    void loadsettings();
    void savesettings();
    void calcStat(ModelShipData *modelShipData, TableModel *modelStat);

public slots:
    void setPartFilter();

private slots:
    void updShip();
    void setCurrentShip(int index);
    void sync();
    void printNakl();
    void updPol();
    void updBalance();
    void updKisBalance(QModelIndex ind);
    void updShipStatisticEl();
    void updShipStatisticWire();
};

#endif // FORMSHIP_H
