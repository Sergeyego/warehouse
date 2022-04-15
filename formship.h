#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "dialogcods.h"
#include <QDomDocument>
#include "models.h"
#include "tablemodel.h"

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
    QString queryState;
    QString prefix;
    ModelBalance *modelBalence;
    DbRelation *relPart;
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
    QMultiHash<QString,partInfo> part;
    QHash<QString,contInfo> cont;
    QString getPackName(QString id_part_kis);
    QString getDesc(QString id_part_kis, QString defval=QString());
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
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submit();
    void revert();
    void setFlt(QString kis);
    bool insertRow(int row, const QModelIndex &parent);

public slots:
    double getStock(QModelIndex index);
    void refreshState();
    void setPartFlt(int ind);
    void setOstControl(bool b);

private:
    shipContInfo info;
    QMap <int,int> colorState;
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
    QSortFilterProxyModel *proxyModelBalance;
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
    void updKisBalance(QModelIndex ind);
    void updShipStatistic();
};

#endif // FORMSHIP_H
