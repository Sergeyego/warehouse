#ifndef SYNC1C_H
#define SYNC1C_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QEventLoop>
#include <QProgressDialog>
#include <QApplication>

struct base1CSettigs
{
    QString url;
    QString user;
    QString password;
};

struct packVal
{
    QString id;
    QString nam;
};

class Sync1C: public QObject
{
    Q_OBJECT
public:
    Sync1C(QObject *parent);

public slots:
    void syncCatalogEl();
    void syncCatalogWire();

private:
    base1CSettigs base1C;
    QString elParentKey;
    QString wireParentKey;
    QHash <QString, QString> catalogKeys;
    QMultiHash <QString, packVal> catalogPacks;
    QMultiHash <QString, QString> catalogEans;
    QString syncCatalog(bool syncEl=true, bool syncWire=true);
    QNetworkRequest baseRequest(QString obj);
    bool postSync(QString obj, QJsonObject &data);
    QJsonObject getSync(QString obj);
    QJsonObject tmpCatalog(QString name);
    bool containsPack(QString ownerKey, QString nam);
    int updateCatologKeys();
    int updateCatalogPacks();
    int updateCatalogEans();
    int catalogSync(QString queryStr, QString parentKey);
    int packSync(QString queryStr);
    int eanSync(QString queryStr);
    int elCatalogSync();
    int elPackSync();
    int elEanSync();
    int wireCatalogSync();
    int wirePackSync();
    int wireEanSync();

private slots:
    void showErrMes(QString err);

};

#endif // SYNC1C_H
