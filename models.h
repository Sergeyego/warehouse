#ifndef MODELS_H
#define MODELS_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QtSql>
#include "db/dbtablemodel.h"

class Models : public QObject
{
    Q_OBJECT
public:
    static Models *instance();
    DbRelation *relWirePart;
    DbRelation *relElPart;
    DbRelation *relEl;
    DbRelation *relPol;
    DbRelation *relShipType;
    DbRelation *relAccType;

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
};

#endif // MODELS_H
