#include "sync1c.h"

Sync1C::Sync1C(QObject *parent): QObject(parent)
{
    base1C.url=QString("http://192.168.1.9/wms/odata/standard.odata/");
    base1C.user=QString("Администратор");
}

void Sync1C::syncCatalogEl()
{
    updateCatologKeys();
    elCatalogSync();
    updateCatologKeys();
    updateCatalogPacks();
    elPackSync();
    updateCatalogPacks();
    updateCatalogEans();
    elEanSync();
}

QNetworkRequest Sync1C::getRequest(QString obj)
{
    QNetworkRequest request(QUrl(base1C.url+obj));
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Authorization", "Basic "+base1C.user.toLocal8Bit().toBase64());
    return request;
}

QNetworkRequest Sync1C::postRequest(QString obj, QByteArray contentLen)
{
    QNetworkRequest request(QUrl(base1C.url+obj));
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("User-Agent", "Appszsm");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", contentLen);
    request.setRawHeader("Authorization", "Basic "+base1C.user.toLocal8Bit().toBase64());
    return request;
}

bool Sync1C::postSync(QString obj, QJsonObject &data)
{
    bool ok=false;
    QJsonDocument doc;
    doc.setObject(data);
    QByteArray d= doc.toJson();
    QByteArray len;
    len.append(QString::number(d.length()));
    QNetworkRequest request(postRequest(obj,len));

    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply = man.post(request,d);
    if (!reply->isFinished()){
        loop.exec();
    }

    ok=(reply->error()==QNetworkReply::NoError);
    reply->deleteLater();
    return ok;
}

QJsonObject Sync1C::getSync(QString obj)
{
    QJsonDocument doc;
    QNetworkRequest request(getRequest(obj));

    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply = man.get(request);
    if (!reply->isFinished()){
        loop.exec();
    }

    if (reply->error()!=QNetworkReply::NoError){
        showErrMes(reply->errorString());
    } else {
        doc=QJsonDocument::fromJson(reply->readAll());
    }
    reply->deleteLater();
    return doc.object();
}

QJsonObject Sync1C::tmpCatalog(QString name)
{
    QJsonObject obj;
    QFile file(name);
    if (file.open(QIODevice::ReadOnly)){
        QByteArray cont=file.readAll();
        QJsonDocument doc=QJsonDocument::fromJson(cont);
        obj=doc.object();
        file.close();
    }
    return obj;
}

bool Sync1C::containsPack(QString ownerKey, QString nam)
{
    bool c=false;
    QList <packVal> list=catalogPacks.values(ownerKey);
    for(packVal n : list){
        if (n.nam==nam){
            c=true;
            break;
        }
    }
    return c;
}

void Sync1C::updateCatologKeys()
{
    QJsonObject obj=getSync("Catalog_усНоменклатура");
    QJsonArray json=obj.value("value").toArray();
    catalogKeys.clear();
    for (QJsonValue val : json){
        QString key=val.toObject().value("КодКИС").toString();
        QString value=val.toObject().value("Ref_Key").toString();
        QString nam=val.toObject().value("Description").toString();
        if (nam==QString("Сварочные электроды")){
            elParentKey=value;
        } else if (nam==QString("Сварочная проволока")){
            wireParentKey=value;
        }
        if (!key.isEmpty()){
            catalogKeys.insert(key,value);
        }
    }
    qDebug()<<"kvo noms: "<<json.size();
}

void Sync1C::updateCatalogPacks()
{
    QJsonObject obj=getSync("Catalog_усУпаковкиНоменклатуры");
    QJsonArray json=obj.value("value").toArray();
    catalogPacks.clear();
    for (QJsonValue val : json){
        QString key=val.toObject().value("Owner_Key").toString();
        packVal p;
        p.nam=val.toObject().value("Description").toString();
        p.id=val.toObject().value("Ref_Key").toString();
        if (!key.isEmpty()){
            catalogPacks.insert(key,p);
        }
    }
    qDebug()<<"kvo packs: "<<json.size();
}

void Sync1C::updateCatalogEans()
{
    QJsonObject obj=getSync("InformationRegister_усШтрихкоды");
    QJsonArray json=obj.value("value").toArray();
    catalogEans.clear();
    for (QJsonValue val : json){
        QString key=val.toObject().value("Номенклатура_Key").toString();
        QString ean=val.toObject().value("Штрихкод").toString();
        if (!key.isEmpty()){
            catalogEans.insert(key,ean);
        }
    }
    qDebug()<<"kvo eans: "<<json.size();
}


void Sync1C::showErrMes(QString err)
{
    QMessageBox::critical(nullptr,tr("Ошибка"),err,QMessageBox::Cancel);
}

void Sync1C::elCatalogSync()
{
    QJsonObject obj=tmpCatalog("tmp.json");
    QSqlQuery query;
    query.prepare("select distinct ee.id_el||':'||ee.id_diam, e.marka ||' ф '|| d.sdim from ean_el ee "
                  "inner join elrtr e on e.id = ee.id_el "
                  "inner  join diam d on d.id = ee.id_diam "
                  "order by e.marka ||' ф '|| d.sdim");
    if (query.exec()){
        while (query.next()){
            QString key=query.value(0).toString();
            if (!catalogKeys.contains(key)){
                obj.insert("Description",query.value(1).toString());
                obj.insert("КодКИС",key);
                obj.insert("Parent_Key",elParentKey);
                postSync("Catalog_усНоменклатура",obj);
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }
}

void Sync1C::elPackSync()
{
    QJsonObject obj=tmpCatalog("tmppack.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd()) {
        QSqlQuery query;
        query.prepare("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack from ean_el ee "
                      "inner join el_pack ep on ep.id = ee.id_pack "
                      "where ee.id_el||':'||ee.id_diam = :kis "
                      "order by npack");
        query.bindValue(":kis",i.key());
        if (query.exec()){
            while (query.next()){
                QString pack=query.value(1).toString();
                if (!containsPack(i.value(),pack)){
                    obj.insert("Description",pack);
                    obj.insert("Owner_Key",i.value());
                    postSync("Catalog_усУпаковкиНоменклатуры",obj);
                }
            }
        }
        ++i;
    }
}

void Sync1C::elEanSync()
{
    QJsonObject obj=tmpCatalog("tmpean.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd()) {
        QSqlQuery query;
        query.prepare("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack, "
                      "ee.ean_ed, ee.ean_group, ep.mass_ed, ep.mass_group from ean_el ee "
                      "inner join el_pack ep on ep.id = ee.id_pack "
                      "where ee.id_el||':'||ee.id_diam = :kis "
                      "order by npack");
        query.bindValue(":kis",i.key());
        if (query.exec()){
            while (query.next()){
                QString ean_ed=query.value(2).toString();
                QString ean_group=query.value(3).toString();
                QString mas_ed=query.value(4).toString();
                QString mas_group=query.value(5).toString();
                QList<packVal> packs=catalogPacks.values(i.value());
                for (packVal p: packs){
                    QString packKey=p.id;
                    QString nomKey=i.value();
                    obj.insert("Номенклатура_Key",nomKey);
                    obj.insert("УпаковкаНоменклатуры_Key",packKey);
                    obj.insert("Количество",mas_ed);
                    obj.insert("Штрихкод",ean_ed);
                    postSync("InformationRegister_усШтрихкоды",obj);

                    obj.insert("Количество",mas_group);
                    obj.insert("Штрихкод",ean_group);
                    postSync("InformationRegister_усШтрихкоды",obj);
                }
                //qDebug()<<i.value()<<" "<<packs.size();
            }
        }
        ++i;
    }
}
