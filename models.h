#ifndef MODELS_H
#define MODELS_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QtSql>
#include "db/dbtablemodel.h"
#include "sync1c.h"

class ModelElPart : public DbRelationalModel
{
    Q_OBJECT
public:
    ModelElPart(QObject *parent = 0);
    void setMinDate(QDate d);
public slots:
    void refresh();
private:
    QDate minDate;
};

class ModelWirePart : public DbRelationalModel
{
    Q_OBJECT
public:
    ModelWirePart(QObject *parent = 0);
    void setMinDate(QDate d);
public slots:
    void refresh();
private:
    QDate minDate;
};

class RelPart : public DbRelation
{
    Q_OBJECT
public:
    RelPart(DbRelationalModel * model, QObject *parent);
    int currentFilter();
private:
    int fltIndex;
public slots:
    void setFilter(int index);
signals:
    void filterChanged(int index);

};

class Models : public QObject
{
    Q_OBJECT
public:
    static Models *instance();
    Sync1C *sync1C;
    DbRelation *relEl;
    DbRelation *relPol;
    DbRelation *relDrv;
    DbRelation *relShipType;
    DbRelation *relAccTypeEl;
    DbRelation *relAccTypeWire;
    DbRelation *relKis;
    DbRelation *relDocType;
    ModelWirePart *modelWirePart;
    RelPart *relWirePart;
    ModelElPart *modelElPart;
    RelPart *relElPart;
    QString createPalBarcode(QString prefix);

public slots:
    void refresh();

signals:
    void sigRefresh();

protected:
    Models(QObject *parent = 0);

private:
    static Models* models_instance;
    QVector<DbRelation*> rels;
    DbRelation *newDbRelation(QAbstractItemModel *queryModel, int key, int disp);
};

#endif // MODELS_H
