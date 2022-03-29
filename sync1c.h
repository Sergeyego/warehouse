#ifndef SYNC1C_H
#define SYNC1C_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QMessageBox>
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

struct partInfo
{
    QString id_part_kis;
    QString id_kis;
    QString name;
    QString number;
    QString rcp;
    QString desc;
    QString ist;
    QString contKey;
    double kvo;
    double prich;
    double rasch;
};

struct contInfo
{
    QString name;
    QString cell;
    QString zone;
    double kvo;
    double prich;
    double rasch;
};

struct turnInfo
{
    double beg_kvo;
    double prich;
    double rasch;
    double end_kvo;
    QString id_part_kis;
    QString id_kis;
    QString contKey;
};

struct accInfo
{
    double kvo;
    QString source;
    QString id_part_kis;
    QString part_number;
    QString id_kis;
    QString name;
};

struct naklInfo
{
    QString number;
    QDateTime date;
    QString from;
    QString to;
};

struct naklDataInfo
{
    QString name;
    QString parti;
    QString rcp;
    QString barcodecont;
    double kvo;
};

    class Sync1C: public QObject
    {
        Q_OBJECT
    public:
        Sync1C(QObject *parent);
        void getBalance(QDate dat, QMultiHash<QString, partInfo> &info);
        void getContBalance(QDate dat, QHash<QString, contInfo> &info);
        void getBalanceAndTurnovers(QDate beg, QDate end, QMultiHash<QString, turnInfo> &info);
        void getAcceptanceTurnovers(QDate beg, QDate end, QMultiHash<QString, accInfo> &info);
        void getShipTurnovers(QDate beg, QDate end, QMultiHash<QString, accInfo> &info);
        void getCells(QVector<QVector<QVariant>> &info);
        void getConts(QVector<QVector<QVariant>> &info);
        void getNakl(QString kis, naklInfo &info, QVector<naklDataInfo> &datainfo);

    public slots:
        void syncCatalogEl();
        void syncCatalogWire();
        void syncPriemEl(int id_doc);
        void syncPriemWire(int id_doc);
        void syncShip(int id_ship);
        void updateKeys();

    private:
        const QString emptyKey="00000000-0000-0000-0000-000000000000";

        const QString namEl="Сварочные электроды";
        const QString namWire="Сварочная проволока";
        const QString namStages="Базовая настройка";
        const QString namGates="Ворота склада";
        const QString namUnit="кг";
        const QString namAccounting="Учет партий товара";
        const QString namStatus="Кондиция";
        const QString namContType="Европаллета";
        const QString namCodOrg="000000001";
        const QString namZoneOt="Хранение";

        const QString contPrefix="EUR";

        base1CSettigs base1C;
        QHash <QString, QString> constKeys;
        QHash <QString, QString> partIstKeys;
        QHash <QString, QString> partIstNams;
        QHash <QString, QString> catalogTypeKeys;
        QHash <QString, QString> catalogKeys;
        QHash <QString, QString> postIstKeys;
        QHash <QString, QString> counterKeys;
        QHash <QString, QString> shipTypeKeys;
        QHash <QString, QString> zoneKeys;
        QMultiHash <QString, packVal> catalogPacks;
        QMultiHash <QString, QString> catalogEans;
        QMultiHash <QString, QString> catalogParts;
        QMultiHash <QString, QString> catalogZoneOt;
        bool updateBaseSettings(int id_base);
        QString syncCatalog(bool syncEl=true, bool syncWire=true);
        QNetworkRequest baseRequest(QString obj);
        bool sendRequest(QString obj, QString req, QJsonObject *data = nullptr, QJsonObject *respData = nullptr);
        bool patchSync(QString obj, QJsonObject &data, QJsonObject *respData = nullptr);
        bool postSync(QString obj, QJsonObject &data, QJsonObject *respData = nullptr);
        QJsonObject getSync(QString obj);
        bool deleteSync(QString obj);
        QJsonObject tmpCatalog(QString name);
        bool containsPack(QString ownerKey, QString nam);
        QString packKey(QString ownerKey, QString nam);
        QString partiKey(QString id);
        QString partiKey(QString ownerKey, QString nam);
        QHash <QString, QString> updateKeys(QString obj, QString key, QString val);
        QString getKey(QString obj, QString nam, QString param);
        QString getCounterCategoryKey(QString nam);
        QString getCounterKey(int id);
        int updateCatologKeys();
        int updateCatalogPacks();
        int updateCatalogEans();
        int updateCatalogZoneOt();
        int catalogSync(QString queryStr, QString parentKey, QString typeKey);
        int packSync(QString queryStr);
        int eanSync(QString queryStr);
        int elCatalogSync();
        int elPackSync();
        int elEanSync();
        int wireCatalogSync();
        int wirePackSync();
        int wireEanSync();
        int zoneOtSync();
        bool setPriemStatus(QString docKey);
        bool setShipStatus(QString docKey);
        bool deleteDocStr(QString obj, QString docKey);
        int syncPart(QString queryPart);
        bool postDoc(QString obj, QString docKey);
        int syncOpDoc(QString queryDoc, QString queryCont);
        int syncOpDocData(QString queryCont, QString docKey);
        int syncShipDocData(int id_ship, QString docKey);
        int syncPartEl(int id_doc);
        int syncPartWire(int id_doc);
        int syncPartShip(int id_ship);
        int syncOpDocEl(int id_doc);
        int syncOpDocWire(int id_doc);
        int syncShipDoc(int id_ship);
        bool checkEan(QString queryDoc, QString queryGen);
        bool genEan(QString queryGen, int id_part);
        bool checkEanEl(int id_doc);
        bool checkEanWire(int id_doc);
        bool checkEan(int id_ship);

    private slots:
        void showErrMes(QString err);

    };

    #endif // SYNC1C_H
