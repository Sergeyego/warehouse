#ifndef MODELS_H
#define MODELS_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QtSql>
#include "db/dbtablemodel.h"
#include "sync1c.h"
#include "invoicemanager.h"

class Models : public QObject
{
    Q_OBJECT
public:
    static Models *instance();
    Sync1C *sync1C;
    InvoiceManager *invoiceManager;
    DbSqlRelation *relPol;
    DbSqlRelation *relKat;
    DbSqlRelation *relDrv;
    DbSqlRelation *relAccTypeEl;
    DbSqlRelation *relAccTypeWire;
    DbSqlRelation *relKis;
    DbSqlRelation *relWirePart;
    DbSqlRelation *relElPart;
    DbSqlRelation *relElrtr;
    DbSqlRelation *relElPack;
    DbSqlRelation *relVars;
    DbSqlRelation *relDiam;
    DbSqlRelation *relProvol;
    DbSqlRelation *relSpool;
    DbSqlRelation *relWirePack;
    DbSqlRelation *relReqCause;
    QString createPalBarcode(QString prefix);

protected:
    Models(QObject *parent = 0);

private:
    static Models* models_instance;
};

#endif // MODELS_H
