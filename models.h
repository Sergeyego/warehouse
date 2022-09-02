#ifndef MODELS_H
#define MODELS_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QtSql>
#include "db/dbtablemodel.h"
#include "sync1c.h"

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

class ModelElPart : public DbRelationalModel
{
    Q_OBJECT
public:
    ModelElPart(QObject *parent = 0);
public slots:
    void refresh(QDate date);
};

class ModelWirePart : public DbRelationalModel
{
    Q_OBJECT
public:
    ModelWirePart(QObject *parent = 0);
public slots:
    void refresh(QDate date);
};

class RelPart : public DbRelation
{
    Q_OBJECT
public:
    RelPart(DbRelationalModel * model, QObject *parent);
public slots:
    void setFilter(int index);

};

#endif // MODELS_H
