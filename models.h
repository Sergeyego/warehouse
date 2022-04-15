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
    DbRelation *relWirePart;
    DbRelation *relElPart;
    DbRelation *relEl;
    DbRelation *relPol;
    DbRelation *relShipType;
    DbRelation *relAccTypeEl;
    DbRelation *relAccTypeWire;
    DbRelation *relKis;
    DbRelationalModel *modelElPart;
    DbRelationalModel *modelWirePart;
    QString createPalBarcode(QString prefix);

public slots:
    void refresh();
    void setFilter(int index);

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
