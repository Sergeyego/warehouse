#ifndef SYNC1C_H
#define SYNC1C_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QBuffer>
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
    void syncPriemEl(int id_doc);
    void syncPriemWire(int id_doc);

private:
    const QString emptyKey="00000000-0000-0000-0000-000000000000";
    const QString namEl="Сварочные электроды";
    const QString namWire="Сварочная проволока";

    base1CSettigs base1C;
    QString elParentKey;
    QString wireParentKey;
    QHash <QString, QString> partIstKeys;
    QHash <QString, QString> catalogTypeKeys;
    QHash <QString, QString> catalogKeys;
    QMultiHash <QString, packVal> catalogPacks;
    QMultiHash <QString, QString> catalogEans;
    QMultiHash <QString, QString> catalogParts;
    QString syncCatalog(bool syncEl=true, bool syncWire=true);
    QNetworkRequest baseRequest(QString obj);
    bool patchSync(QString obj, QJsonObject &data, QJsonObject *respData = nullptr);
    bool postSync(QString obj, QJsonObject &data, QJsonObject *respData = nullptr);
    QJsonObject getSync(QString obj);
    bool deleteSync(QString obj);
    QJsonObject tmpCatalog(QString name);
    bool containsPack(QString ownerKey, QString nam);
    QString packKey(QString ownerKey, QString nam);
    QString partiKey(QString id);
    QHash <QString, QString> updateKeys(QString obj, QString key, QString val);
    int updatePartIstKeys();
    int updateCatologTypeKeys();
    int updateCatologKeys();
    int updateCatalogPacks();
    int updateCatalogEans();
    int catalogSync(QString queryStr, QString parentKey, QString typeKey);
    int packSync(QString queryStr);
    int eanSync(QString queryStr);
    int elCatalogSync();
    int elPackSync();
    int elEanSync();
    int wireCatalogSync();
    int wirePackSync();
    int wireEanSync();
    bool setPriemStatus(QString docKey);
    bool deletePriemStr(QString docKey);
    int syncPart(QString queryPart);
    int syncOpDoc(QString queryDoc, QString queryCont);
    int syncOpDocData(QString queryCont, QString docKey);
    int syncPartEl(int id_doc);
    int syncPartWire(int id_doc);
    int syncOpDocEl(int id_doc);
    int syncOpDocWire(int id_doc);
    bool checkEan(QString queryDoc, QString queryGen);
    bool genEan(QString queryGen, int id_part);
    bool checkEanEl(int id_doc);

private slots:
    void showErrMes(QString err);

};

#endif // SYNC1C_H
