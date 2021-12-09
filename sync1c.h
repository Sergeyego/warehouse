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

private:
    base1CSettigs base1C;
    QString elParentKey;
    QString wireParentKey;
    QHash <QString, QString> catalogKeys;
    QMultiHash <QString, packVal> catalogPacks;
    QMultiHash <QString, QString> catalogEans;
    QNetworkRequest baseRequest(QString obj);
    bool postSync(QString obj, QJsonObject &data);
    QJsonObject getSync(QString obj);
    QJsonObject tmpCatalog(QString name);
    bool containsPack(QString ownerKey, QString nam);

private slots:
    void updateCatologKeys();
    void updateCatalogPacks();
    void updateCatalogEans();
    void showErrMes(QString err);
    int elCatalogSync();
    int elPackSync();
    int elEanSync();
    int wireCatalogSync();

};

#endif // SYNC1C_H
