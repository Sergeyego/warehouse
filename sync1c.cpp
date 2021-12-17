#include "sync1c.h"

Sync1C::Sync1C(QObject *parent): QObject(parent)
{
    base1C.url=QString("http://192.168.1.9/wms/odata/standard.odata/");
    base1C.user=QString("Администратор");   

    updateCatologTypeKeys();
    updatePartIstKeys();
}

void Sync1C::syncCatalogEl()
{
    QString info=syncCatalog(true,false);
    QMessageBox::information(nullptr,tr("Информация"),info,QMessageBox::Ok);
}

void Sync1C::syncCatalogWire()
{
    QString info=syncCatalog(false,true);
    QMessageBox::information(nullptr,tr("Информация"),info,QMessageBox::Ok);
}

void Sync1C::syncPriemEl(int id_doc)
{
    syncCatalog(true,false);
    syncPartEl(id_doc);
    //QSqlQuery query;
    //query.prepare("");
}

QString Sync1C::syncCatalog(bool syncEl, bool syncWire)
{
    QProgressDialog d;
    d.setWindowTitle(tr("Синхронизация данных"));
    d.setLabelText(tr("Обновление номенклатуры"));
    d.setCancelButton(nullptr);
    d.setMinimum(0);
    d.setMaximum(3);
    d.show();

    int in=updateCatologKeys();
    int nc=0;
    if (syncEl){
        nc+=elCatalogSync();
    }
    if (syncWire){
        nc+=wireCatalogSync();
    }
    if (nc>0){
        updateCatologKeys();
    }
    QApplication::processEvents();
    d.setLabelText("Обновление типов упаковки");
    d.setValue(1);

    int ip=updateCatalogPacks();
    int np=0;
    if (syncEl){
        np+=elPackSync();
    }
    if (syncWire){
        np+=wirePackSync();
    }
    if (np>0){
        updateCatalogPacks();
    }

    QApplication::processEvents();
    d.setLabelText("Обновление штрихкодов");
    d.setValue(2);

    int ie=updateCatalogEans();
    int ne=0;
    if (syncEl){
        ne+=elEanSync();
    }
    if (syncWire){
        ne+=wireEanSync();
    }

    d.hide();
    QString info;
    info=QString("Всего наименований номенклатуры: %1; Загружено новых: %2; \n"
                 "Всего типов упаковки: %3; Загружено новых %4; \n"
                 "Всего штрихкодов: %5; Загружено новых: %6;").arg(in).arg(nc).arg(ip).arg(np).arg(ie).arg(ne);
    return info;
}

QNetworkRequest Sync1C::baseRequest(QString obj)
{
    QNetworkRequest request(QUrl(base1C.url+obj));
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("User-Agent", "Appszsm");
    request.setRawHeader("Authorization", "Basic "+base1C.user.toLocal8Bit().toBase64());
    return request;
}

bool Sync1C::patchSync(QString obj, QJsonObject &data, QJsonObject *respData)
{
    bool ok=false;
    return ok;
}

bool Sync1C::postSync(QString obj, QJsonObject &data, QJsonObject *respData)
{
    QJsonDocument doc;
    doc.setObject(data);
    QByteArray d = doc.toJson();
    QNetworkRequest request(baseRequest(obj));

    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply = man.post(request,d);
    if (!reply->isFinished()){
        loop.exec();
    }

    QJsonDocument respDoc;
    respDoc=QJsonDocument::fromJson(reply->readAll());

    if (respData){
        *respData = respDoc.object();
    }

    bool ok=(reply->error()==QNetworkReply::NoError);
    if (!ok){
        qDebug()<<QString::fromUtf8(respDoc.toJson());
        showErrMes(reply->errorString());
    }
    reply->deleteLater();
    return ok;
}

QJsonObject Sync1C::getSync(QString obj)
{
    QJsonDocument doc;
    QNetworkRequest request(baseRequest(obj));

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

QHash<QString, QString> Sync1C::updateKeys(QString obj, QString key, QString val)
{
    QJsonObject o=getSync(obj);
    QJsonArray json=o.value("value").toArray();
    QHash<QString, QString> hash;
    for (QJsonValue v : json){
        QString hkey=v.toObject().value(key).toString();
        QString hvalue=v.toObject().value(val).toString();
        if (!key.isEmpty()){
            hash.insert(hkey,hvalue);
        }
    }
    qDebug()<<obj<<" keys: "<<hash.size();
    return hash;
}

int Sync1C::updatePartIstKeys()
{
    partIstKeys = updateKeys("Catalog_усИсточникиПартий","Code","Ref_Key");
    return partIstKeys.size();
}

int Sync1C::updateCatologTypeKeys()
{
    catalogTypeKeys = updateKeys("Catalog_усВидыНоменклатуры","Description","Ref_Key");
    return catalogTypeKeys.size();
}

int Sync1C::updateCatologKeys()
{
    QJsonObject obj=getSync("Catalog_усНоменклатура");
    QJsonArray json=obj.value("value").toArray();
    catalogKeys.clear();
    for (QJsonValue val : json){
        QString key=val.toObject().value("КодКИС").toString();
        QString value=val.toObject().value("Ref_Key").toString();
        QString nam=val.toObject().value("Description").toString();
        if (nam==namEl){
            elParentKey=value;
        } else if (nam==namWire){
            wireParentKey=value;
        }
        if (!key.isEmpty()){
            catalogKeys.insert(key,value);
        }
    }
    //qDebug()<<"kvo noms: "<<json.size();
    return json.size();
}

int Sync1C::updateCatalogPacks()
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
    //qDebug()<<"kvo packs: "<<json.size();
    return json.size();
}

int Sync1C::updateCatalogEans()
{
    QJsonObject obj=getSync("InformationRegister_усШтрихкоды?$filter=like(Штрихкод,'4627120______')");
    QJsonArray json=obj.value("value").toArray();
    catalogEans.clear();
    for (QJsonValue val : json){
        QString key=val.toObject().value("Номенклатура_Key").toString();
        QString ean=val.toObject().value("Штрихкод").toString();
        if (!key.isEmpty()){
            catalogEans.insert(key,ean);
        }
    }
    //qDebug()<<"kvo eans: "<<json.size();
    return json.size();
}


void Sync1C::showErrMes(QString err)
{
    QMessageBox::critical(nullptr,tr("Ошибка"),err,QMessageBox::Cancel);
}

int Sync1C::catalogSync(QString queryStr, QString parentKey, QString typeKey)
{
    int n=0;
    bool ok=true;
    QJsonObject obj=tmpCatalog("tmp.json");
    QSqlQuery query;
    query.prepare(queryStr);
    if (query.exec()){
        while (query.next()){
            QString key=query.value(0).toString();
            if (!catalogKeys.contains(key)){
                obj.insert("Description",query.value(1).toString());
                obj.insert("КодКИС",key);
                obj.insert("Parent_Key",parentKey);
                obj.insert("ВидНоменклатуры_Key",typeKey);
                ok=postSync("Catalog_усНоменклатура",obj);
                if (ok){
                    n++;
                } else {
                    break;
                }
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return n;
}

int Sync1C::packSync(QString queryStr)
{
    int n=0;
    bool ok=true;
    QJsonObject obj=tmpCatalog("tmppack.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd() && ok) {
        QSqlQuery query;
        query.prepare(queryStr);
        query.bindValue(":kis",i.key());
        if (query.exec()){
            while (query.next() && ok){
                QString pack=query.value(1).toString();
                if (!containsPack(i.value(),pack)){
                    obj.insert("Description",pack);
                    obj.insert("Owner_Key",i.value());
                    obj.insert("Коэффициент",query.value(2).toDouble());
                    obj.insert("Масса",query.value(2).toDouble());
                    ok=postSync("Catalog_усУпаковкиНоменклатуры",obj);
                    if (ok){
                        n++;
                    } else {
                        break;
                    }
                }
            }
        }
        ++i;
    }
    return n;
}

int Sync1C::eanSync(QString queryStr)
{
    int n=0;
    bool ok=true;
    QJsonObject obj=tmpCatalog("tmpean.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd() && ok) {
        QSqlQuery query;
        query.prepare(queryStr);
        query.bindValue(":kis",i.key());
        if (query.exec()){
            while (query.next() && ok){
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
                    if (!ean_ed.isEmpty() && !catalogEans.values(nomKey).contains(ean_ed)){
                        obj.insert("Количество",mas_ed);
                        obj.insert("Штрихкод",ean_ed);
                        ok=postSync("InformationRegister_усШтрихкоды",obj);
                        if (ok){
                            n++;
                        } else {
                            break;
                        }
                    }
                    if (!ean_group.isEmpty()&& !catalogEans.values(nomKey).contains(ean_group)){
                        obj.insert("Количество",mas_group);
                        obj.insert("Штрихкод",ean_group);
                        ok=postSync("InformationRegister_усШтрихкоды",obj);
                        if (ok){
                            n++;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
        ++i;
    }
    return n;
}

int Sync1C::elCatalogSync()
{
    QString query("select distinct ee.id_el||':'||ee.id_diam, e.marka ||' ф '|| d.sdim "
                  "from ean_el ee "
                  "inner join elrtr e on e.id = ee.id_el "
                  "inner  join diam d on d.id = ee.id_diam "
                  "order by e.marka ||' ф '|| d.sdim");
    return catalogSync(query,elParentKey,catalogTypeKeys.value("Сварочные электроды"));
}

int Sync1C::elPackSync()
{
    QString query("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack, ep.mass_ed "
                  "from ean_el ee "
                  "inner join el_pack ep on ep.id = ee.id_pack "
                  "where ee.id_el||':'||ee.id_diam = :kis "
                  "order by npack");
    return packSync(query);
}

int Sync1C::elEanSync()
{
    QString query("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack, "
                  "ee.ean_ed, ee.ean_group, ep.mass_ed, ep.mass_group "
                  "from ean_el ee "
                  "inner join el_pack ep on ep.id = ee.id_pack "
                  "where ee.id_el||':'||ee.id_diam = :kis "
                  "order by npack");
    return eanSync(query);
}

int Sync1C::wireCatalogSync()
{
    QString query("select distinct we.id_prov ||':'||we.id_diam ||':'||we.id_spool, p.nam ||' ф '|| d.sdim||' '||wpk.short as nam "
                  "from wire_ean we "
                  "inner join provol p on p.id=we.id_prov "
                  "inner  join diam d on d.id = we.id_diam "
                  "inner join wire_pack_kind wpk on wpk.id = we.id_spool "
                  "order by nam");
    return catalogSync(query,wireParentKey,catalogTypeKeys.value("Сварочная проволока"));
}

int Sync1C::wirePackSync()
{
    QString query("select distinct we.id_prov ||':'||we.id_diam||':'||we.id_spool as kis, "
                  "CASE WHEN wp.pack_group<>'-' THEN wp.pack_ed||'/'||wp.pack_group ELSE wp.pack_ed end as npack, "
                  "wp.mas_ed "
                  "from wire_ean we "
                  "inner join wire_pack wp on wp.id = we.id_pack "
                  "where we.id_prov ||':'||we.id_diam||':'||we.id_spool = :kis "
                  "order by npack");
    return packSync(query);
}

int Sync1C::wireEanSync()
{
    QString query("select distinct we.id_prov ||':'||we.id_diam||':'||we.id_spool as kis, "
                  "CASE WHEN wp.pack_group<>'-' THEN wp.pack_ed||'/'||wp.pack_group ELSE wp.pack_ed end as npack, "
                  "we.ean_ed, we.ean_group, wp.mas_ed, wp.mas_group "
                  "from wire_ean we "
                  "inner join wire_pack wp on wp.id = we.id_pack "
                  "where we.id_prov ||':'||we.id_diam||':'||we.id_spool = :kis "
                  "order by npack");
    return eanSync(query);
}

int Sync1C::syncPartEl(int id_doc)
{
    int n=0;
    bool ok=true;
    QJsonObject obj=tmpCatalog("tmppart.json");
    QSqlQuery query;
    query.prepare("select ad.id_part, p.id_el ||':'||(select id from diam as d where d.diam=p.diam) as kis, "
                  "p.n_s, p.dat_part, i.key1c, p.prim_prod, rn.nam "
                  "from acceptance_data ad "
                  "inner join parti p on p.id = ad.id_part "
                  "inner join istoch i on i.id = p.id_ist "
                  "left join rcp_nam rn on rn.id = p.id_rcp "
                  "where ad.id_acceptance = :id_doc "
                  "order by p.n_s, p.dat_part");
    query.bindValue(":id_doc",id_doc);
    if (query.exec()){
        while (query.next()){
            QString desc=query.value(2).toString()+"-"+QString::number(query.value(3).toDate().year());
            QString ownerKey=catalogKeys.value(query.value(1).toString(),emptyKey);

            QString filter=QString("?$filter=Description eq '%1' and Owner_Key eq guid'%2'").arg(desc).arg(ownerKey);

            QJsonObject partObj=getSync("Catalog_усПартииНоменклатуры"+filter);
            QJsonArray json=partObj.value("value").toArray();
            qDebug()<<json.size();

            if (!json.size()){
                //qDebug()<<query.value(0).toString();
                obj.insert("КодКис","e:"+query.value(0).toString());
                obj.insert("Description",desc);
                obj.insert("Code",query.value(2).toString());
                obj.insert("Owner_Key",ownerKey);
                obj.insert("ДатаПартии",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
                obj.insert("ДатаПроизводства",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
                obj.insert("СрокГодности",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
                obj.insert("Источник_Key",partIstKeys.value(query.value(4).toString()));
                obj.insert("Комментарий",query.value(5).toString());
                obj.insert("РецептураПлавка",query.value(6).toString());

                ok=postSync("Catalog_усПартииНоменклатуры",obj);
            } else {
                //QJsonDocument doc;
                //doc.setObject(partObj);
                qDebug()<<"Партия существует! "+desc;
            }
            if (ok){
                n++;
            } else {
                break;
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return n;
}
