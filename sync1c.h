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
    QMultiHash <QString, QString> catalogPacks;
    QNetworkRequest getRequest(QString obj);
    QNetworkRequest postRequest(QString obj, QByteArray contentLen);
    bool postSync(QString obj, QJsonObject &data);
    QJsonObject getSync(QString obj);
    QJsonObject tmpCatalog(QString name);

private slots:
    void updateCatologKeys();
    void updateCatalogPacks();
    void showErrMes(QString err);
    void elCatalogSync();
    void elPackSync();

};

#endif // SYNC1C_H
