#include "sync1c.h"

Sync1C::Sync1C(QObject *parent): QObject(parent)
{
    updateBaseSettings(1);
    updateKeys();
}

void Sync1C::getBalance(QDate dat, QMultiHash<QString, partInfo> &info)
{
    QString obj=QString("AccumulationRegister_усОстаткиТоваров/Balance(Period=datetime'%1')?$expand=КлючАналитикиУчетаНоменклатуры/*").arg(dat.toString("yyyy-MM-dd"));
    QJsonObject o=getSync(obj);
    QJsonArray json=o.value("value").toArray();
    info.clear();
    for (QJsonValue v : json){
        QJsonObject keyAn=v.toObject().value("КлючАналитикиУчетаНоменклатуры").toObject();
        QJsonObject part=keyAn.value("ПартияНоменклатуры").toObject();
        QJsonObject nom=keyAn.value("Номенклатура").toObject();
        partInfo inf;
        inf.id_kis=nom.value("КодКИС").toString();
        inf.kvo=v.toObject().value("КоличествоBalance").toDouble();
        inf.prich=v.toObject().value("КоличествоПриходBalance").toDouble();
        inf.rasch=v.toObject().value("КоличествоРасходBalance").toDouble();
        inf.id_part_kis=part.value("КодКис").toString();
        inf.name=nom.value("Description").toString();
        inf.number=part.value("Description").toString();
        inf.rcp=part.value("РецептураПлавка").toString();
        inf.desc=part.value("Комментарий").toString();
        inf.ist=partIstNams.value(part.value("Источник_Key").toString());
        info.insert(inf.id_kis,inf);
        //qDebug()<<id_kis<<":"<<inf.id_part_kis<<" "<<inf.name<<""<<inf.packName<<" "<<inf.number<<" "<<inf.ist<<" "<<inf.rcp<<" "<<inf.desc<<" "<<inf.kvo;
    }
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
    checkEanEl(id_doc);
    syncCatalog(true,false);
    syncPartEl(id_doc);
    syncOpDocEl(id_doc);
}

void Sync1C::syncPriemWire(int id_doc)
{
    checkEanWire(id_doc);
    syncCatalog(false,true);
    syncPartWire(id_doc);
    syncOpDocWire(id_doc);
}

void Sync1C::syncShip(int id_ship)
{
    checkEan(id_ship);
    syncCatalog(true,true);
    syncPartShip(id_ship);
    syncShipDoc(id_ship);
}

void Sync1C::updateKeys()
{
    partIstKeys = updateKeys("Catalog_усИсточникиПартий","Code","Ref_Key");
    partIstNams = updateKeys("Catalog_усИсточникиПартий","Ref_Key","Description");
    catalogTypeKeys = updateKeys("Catalog_усВидыНоменклатуры","Description","Ref_Key");
    postIstKeys = updateKeys("Catalog_усИсточникиПоступления","Description","Ref_Key");
    counterKeys = updateKeys("Catalog_усКонтрагенты","Code","Ref_Key");
    shipTypeKeys = updateKeys("Catalog_усНаправлениеОтгрузки","Description","Ref_Key");

    constKeys.clear();
    constKeys.insert(namEl,getKey("Catalog_усНоменклатура",namEl,"Description"));
    constKeys.insert(namWire,getKey("Catalog_усНоменклатура",namWire,"Description"));
    constKeys.insert(namStages,getKey("Catalog_усСтадииПриемки",namStages,"Description"));
    constKeys.insert(namGates,getKey("Catalog_усЯчейки",namGates,"Code"));
    constKeys.insert(namUnit,getKey("Catalog_усЕдиницыИзмерения",namUnit,"Description"));
    constKeys.insert(namAccounting,getKey("Catalog_усМоделиУчетаНоменклатуры",namAccounting,"Description"));
    constKeys.insert(namStatus,getKey("Catalog_усСтатусыНоменклатуры",namStatus,"Description"));
    constKeys.insert(namContType,getKey("Catalog_усТипыКонтейнеров",namContType,"Description"));
    constKeys.insert(namCodOrg,getKey("Catalog_Организации",namCodOrg,"Code"));

    //qDebug()<<constKeys;
}

bool Sync1C::updateBaseSettings(int id_base)
{
    QSqlQuery query;
    query.prepare("select wd.url, wd.usr, wd.pass from warehouse_data as wd where wd.id = :id_base ");
    query.bindValue(":id_base",id_base);
    bool ok=false;
    if (query.exec()){
        if (query.next()){
            base1C.url=query.value(0).toString();
            base1C.user=query.value(1).toString();
            base1C.password=query.value(2).toString();
            ok=true;
        }

    } else {
        showErrMes(query.lastError().text());
    }
    return ok;
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
    QString authData = base1C.user+":"+base1C.password;
    QNetworkRequest request(QUrl(base1C.url+obj));
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Accept-Charset", "UTF-8");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("User-Agent", "Appszsm");
    request.setRawHeader("Authorization", "Basic "+authData.toLocal8Bit().toBase64());
    return request;
}

bool Sync1C::sendRequest(QString obj, QString req, QJsonObject *data, QJsonObject *respData)
{
    QNetworkRequest request(baseRequest(obj));
    QByteArray d;
    if (data){
        QJsonDocument doc;
        doc.setObject(*data);
        d = doc.toJson();
    }
    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply;
    if (req=="GET"){
        reply=man.get(request);
    } else if (req=="POST"){
        reply=man.post(request,d);
    } else if (req=="DELETE"){
        reply=man.deleteResource(request);
    } else {
        reply=man.sendCustomRequest(request,req.toUtf8(),d);
    }

    if (!reply->isFinished()){
        loop.exec();
    }

    QJsonDocument respDoc;
    respDoc=QJsonDocument::fromJson(reply->readAll());

    bool ok=(reply->error()==QNetworkReply::NoError);
    if (!ok){
        QString mes=respDoc.object().value("odata.error").toObject().value("message").toObject().value("value").toString();
        showErrMes(reply->errorString()+":\n"+mes);
    } else if (respData){
        *respData = respDoc.object();
    }
    reply->deleteLater();
    return ok;
}

bool Sync1C::patchSync(QString obj, QJsonObject &data, QJsonObject *respData)
{
    return sendRequest(obj,"PATCH",&data,respData);
}

bool Sync1C::postSync(QString obj, QJsonObject &data, QJsonObject *respData)
{
    return sendRequest(obj,"POST",&data,respData);
}

QJsonObject Sync1C::getSync(QString obj)
{
    QJsonObject d;
    sendRequest(obj,"GET",nullptr,&d);
    return d;
}

bool Sync1C::deleteSync(QString obj)
{
    return sendRequest(obj,"DELETE");
}

QJsonObject Sync1C::tmpCatalog(QString name)
{
    QJsonObject obj;
    QSqlQuery query;
    query.prepare("select tmp from warehouse_tmp where nam = :nam ");
    query.bindValue(":nam",name);
    if (query.exec()){
        if (query.next()){
            QJsonDocument doc=QJsonDocument::fromJson(query.value(0).toByteArray());
            obj=doc.object();
        }
    } else {
        showErrMes(query.lastError().text());
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

QString Sync1C::packKey(QString ownerKey, QString nam)
{
    QString key = emptyKey;
    QList <packVal> list=catalogPacks.values(ownerKey);
    for(packVal n : list){
        if (n.nam==nam){
            key=n.id;
            break;
        }
    }
    return key;
}

QString Sync1C::partiKey(QString id)
{
    return getKey("Catalog_усПартииНоменклатуры",id,"КодКис");
}

QString Sync1C::partiKey(QString ownerKey, QString nam)
{
    QString key = emptyKey;
    QString filter=QString("?$select=Ref_Key&$filter=Description eq '%1' and Owner_Key eq guid'%2'").arg(nam).arg(ownerKey);
    QJsonObject partObj=getSync("Catalog_усПартииНоменклатуры"+filter);
    QJsonArray json=partObj.value("value").toArray();
    if (json.size()){
        QJsonObject o = json.at(0).toObject();
        key = o.value("Ref_Key").toString();
    }
    return key;
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

QString Sync1C::getKey(QString obj, QString nam, QString param)
{
    QString filter=QString("?$select=Ref_Key&$filter=%1 eq '%2'").arg(param).arg(nam);
    QString key=emptyKey;
    QJsonObject o=getSync(obj+filter);
    QJsonArray json=o.value("value").toArray();
    if (json.size()){
        key=json.at(0).toObject().value("Ref_Key").toString();
    }
    return key;
}

QString Sync1C::getCounterCategoryKey(QString nam)
{
    QString key=getKey("Catalog_усКонтрагенты",nam,"Description");
    if (key==emptyKey){
        QJsonObject obj=tmpCatalog("tmpcounter.json");
        obj.insert("Description",nam);
        obj.insert("IsFolder",true);
        QJsonObject ret;
        bool ok= postSync("Catalog_усКонтрагенты",obj, &ret);
        if (ok){
            key = ret.value("Ref_Key").toString();
        }
    }
    return key;
}

QString Sync1C::getCounterKey(int id)
{
    QString key=emptyKey;
    QSqlQuery query;
    query.prepare("select p.short, p.naim, coalesce(p.adres_egrul, p.adres ), p.telef, p.innkpp, p.okpo, "
                  "pk.nam, p.bank, p.city, p.bik, p.rs, p.ks from poluch p "
                  "inner join pol_kat pk on pk.id = p.id_kat "
                  "where p.id = :id");
    query.bindValue(":id",id);
    if (query.exec()){
        if (query.next()){
            QJsonObject obj=tmpCatalog("tmpcounter.json");

            QString snam = query.value(0).toString();
            QString nam = query.value(1).toString();
            //QString adr = query.value(2).toString();
            //QString telef = query.value(3).toString();
            QString innkpp = query.value(4).toString();
            QString okpo = query.value(5).toString();
            QString kat = query.value(6).toString();
            QString bank = query.value(7).toString();
            QString city = query.value(8).toString();
            QString bik = query.value(9).toString();
            QString rs = query.value(10).toString();
            QString ks = query.value(11).toString();

            QString inn, kpp;
            QStringList innkpplist = innkpp.split("/");
            if (innkpplist.size()>1){
                inn=innkpplist.at(0);
                kpp=innkpplist.at(1);
            } else if (innkpplist.size()){
                inn=innkpplist.at(0);
            }
            inn=inn.simplified();
            kpp=kpp.simplified();

            QString rekv=bank;
            if (!city.isEmpty()){
                if (!rekv.isEmpty()){
                    rekv+=" ";
                }
                rekv+=city;
            }
            if (!rs.isEmpty()){
                if (!rekv.isEmpty()){
                    rekv+=", ";
                }
                rekv+="p/c "+rs;
            }
            if (!ks.isEmpty()){
                if (!rekv.isEmpty()){
                    rekv+=", ";
                }
                rekv+="к/c "+ks;
            }
            if (!bik.isEmpty()){
                if (!rekv.isEmpty()){
                    rekv+=", ";
                }
                rekv+="БИК "+bik;
            }

            obj.insert("Description",snam);
            obj.insert("НаименованиеПолное",nam);
            obj.insert("ИНН",inn);
            obj.insert("КПП",kpp);
            obj.insert("ОКПО",okpo);
            obj.insert("БанковскиеРеквизиты",rekv);
            obj.insert("Parent_Key",getCounterCategoryKey(kat));

            key = getKey("Catalog_усКонтрагенты",snam,"Description");

            if (key==emptyKey){
                QJsonObject ret;
                bool ok= postSync("Catalog_усКонтрагенты",obj, &ret);
                if (ok){
                    key = ret.value("Ref_Key").toString();
                }
            } else {
                patchSync(QString("Catalog_усКонтрагенты(guid'%1')").arg(key),obj);
            }

        }
    } else {
        showErrMes(query.lastError().text());
    }
    return key;
}

int Sync1C::updateCatologKeys()
{
    catalogKeys=updateKeys("Catalog_усНоменклатура","КодКИС","Ref_Key");
    return catalogKeys.size();
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
    QMessageBox::critical(nullptr,tr("Ошибка"),err,QMessageBox::Ok);
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
                obj.insert("МодельУчета_Key",constKeys.value(namAccounting,emptyKey));
                obj.insert("ЕдиницаИзмерения_Key",constKeys.value(namUnit,emptyKey));
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
    struct spack
    {
        QString npack;
        QVariant mass_ed;
    };
    spack packstr;
    QMultiHash<QString,spack> phash;
    QSqlQuery query;
    query.prepare(queryStr);
    bool ok=query.exec();
    if (ok){
        while (query.next()){
            packstr.npack=query.value(1).toString();
            packstr.mass_ed=query.value(2);
            phash.insert(query.value(0).toString(),packstr);
        }
    } else {
        showErrMes(query.lastError().text());
    }

    QJsonObject obj=tmpCatalog("tmppack.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd() && ok) {
        QList<spack> packs = phash.values(i.key());
        for (spack p : packs){
            QString pack=p.npack;
            double mas_ed=p.mass_ed.toDouble();
            if (!containsPack(i.value(),pack)){
                obj.insert("Description",pack);
                obj.insert("Owner_Key",i.value());
                obj.insert("Коэффициент",/*mas_ed*/1);
                obj.insert("Масса",mas_ed);
                obj.insert("ЕдиницаИзмерения_Key",constKeys.value(namUnit,emptyKey));
                ok=postSync("Catalog_усУпаковкиНоменклатуры",obj);
                if (ok){
                    n++;
                } else {
                    break;
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
    struct sean
    {
        QString npack;
        QString ean_ed;
        QString ean_group;
        QVariant mass_ed;
        QVariant mass_group;
    };
    sean eanstr;
    QMultiHash<QString,sean> ehash;
    QSqlQuery query;
    query.prepare(queryStr);
    bool ok=query.exec();
    if (ok){
        while (query.next()){
            eanstr.npack=query.value(1).toString();
            eanstr.ean_ed=query.value(2).toString();
            eanstr.ean_group=query.value(3).toString();
            eanstr.mass_ed=query.value(4);
            eanstr.mass_group=query.value(5).toString();
            ehash.insert(query.value(0).toString(),eanstr);
        }
    } else {
        showErrMes(query.lastError().text());
    }

    QJsonObject obj=tmpCatalog("tmpean.json");
    QHash<QString, QString>::const_iterator i = catalogKeys.constBegin();
    while (i != catalogKeys.constEnd() && ok) {
        QList<sean> eans = ehash.values(i.key());
        for (sean e: eans){
            QString ean_ed=e.ean_ed;
            QString ean_group=e.ean_group;
            QString mas_ed=e.mass_ed.toString();
            QString mas_group=e.mass_group.toString();
            QString nomKey=i.value();
            QString packK = packKey(nomKey,e.npack);
            obj.insert("Номенклатура_Key",nomKey);
            obj.insert("УпаковкаНоменклатуры_Key",packK);
            if (!ean_ed.isEmpty() && !catalogEans.values(nomKey).contains(ean_ed)){
                obj.insert("Количество",mas_ed);
                obj.insert("Штрихкод",ean_ed);
                ok=postSync("InformationRegister_усШтрихкоды",obj);
                if (ok){
                    catalogEans.insert(nomKey,ean_ed);
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
                    catalogEans.insert(nomKey,ean_group);
                    n++;
                } else {
                    break;
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
    return catalogSync(query,constKeys.value(namEl,emptyKey),catalogTypeKeys.value(namEl,emptyKey));
}

int Sync1C::elPackSync()
{
    QString query("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack, ep.mass_ed "
                  "from ean_el ee "
                  "inner join el_pack ep on ep.id = ee.id_pack "
                  "order by npack");
    return packSync(query);
}

int Sync1C::elEanSync()
{
    QString query("select distinct ee.id_el||':'||ee.id_diam as kis, ep.pack_ed||'/'||ep.pack_group as npack, "
                  "ee.ean_ed, ee.ean_group, ep.mass_ed, ep.mass_group "
                  "from ean_el ee "
                  "inner join el_pack ep on ep.id = ee.id_pack "
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
    return catalogSync(query,constKeys.value(namWire,emptyKey),catalogTypeKeys.value(namWire,emptyKey));
}

int Sync1C::wirePackSync()
{
    QString query("select distinct we.id_prov ||':'||we.id_diam||':'||we.id_spool as kis, "
                  "CASE WHEN wp.pack_group<>'-' THEN wp.pack_ed||'/'||wp.pack_group ELSE wp.pack_ed end as npack, "
                  "wp.mas_ed "
                  "from wire_ean we "
                  "inner join wire_pack wp on wp.id = we.id_pack "
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
                  "order by npack");
    return eanSync(query);
}

bool Sync1C::setPriemStatus(QString docKey)
{
    QJsonObject status=tmpCatalog("tmpstatus.json");
    status.insert("ОжидаемаяПриемка_Key",docKey);
    status.insert("Статус","Новая");
    return postSync("InformationRegister_усСтатусыОжидаемыхПриемок",status);
}

bool Sync1C::setShipStatus(QString docKey)
{
    QJsonObject status=tmpCatalog("tmpshipstatus.json");
    status.insert("ЗаказНаОтгрузку_Key",docKey);
    status.insert("Статус","Новый");
    return postSync("InformationRegister_усСтатусыЗаказовНаОтгрузку",status);
}

bool Sync1C::deleteDocStr(QString obj, QString docKey)
{
    bool ok=true;
    QString filter=obj+QString("?$filter=Владелец_Key eq guid'%1'").arg(docKey);
    QJsonObject o=getSync(filter);
    QJsonArray json=o.value("value").toArray();
    for(QJsonValue v: json){
        qDebug()<<"delete "+obj+":"<<v.toObject().value("Ref_Key").toString();
        bool b = deleteSync(obj+QString("(guid'%1')").arg(v.toObject().value("Ref_Key").toString()));
        ok = ok && b;
    }
    return ok;
}

int Sync1C::syncPart(QString queryPart)
{
    int n=0;
    bool ok=true;
    QJsonObject obj=tmpCatalog("tmppart.json");
    QSqlQuery query;
    query.prepare(queryPart);
    if (query.exec()){
        QProgressDialog d;
        d.setWindowTitle(tr("Синхронизация данных"));
        d.setLabelText(tr("Обновление партий"));
        d.setCancelButton(nullptr);
        d.setMinimum(0);
        d.setMaximum(query.size());
        d.show();
        while (query.next()){
            QString desc=query.value(2).toString()+"-"+QString::number(query.value(3).toDate().year());
            QString ownerKey=catalogKeys.value(query.value(1).toString(),emptyKey);

            QString key = partiKey(ownerKey,desc);

            obj.insert("КодКис",query.value(0).toString());
            obj.insert("Description",desc);
            obj.insert("Code",query.value(2).toString());
            obj.insert("Owner_Key",ownerKey);
            obj.insert("ДатаПартии",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
            obj.insert("ДатаПроизводства",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
            obj.insert("СрокГодности",query.value(3).toDate().toString("yyyy-MM-dd")+"T00:00:00");
            obj.insert("Источник_Key",partIstKeys.value(query.value(4).toString(),emptyKey));
            obj.insert("Комментарий",query.value(5).toString());
            obj.insert("РецептураПлавка",query.value(6).toString());

            if (key==emptyKey){
                ok=postSync("Catalog_усПартииНоменклатуры",obj);
            } else {
                ok=patchSync(QString("Catalog_усПартииНоменклатуры(guid'%1')").arg(key),obj);
            }
            if (ok){
                n++;
            } else {
                break;
            }
            QApplication::processEvents();
            d.setValue(n);
            d.setLabelText("Обновление партии: "+desc);
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return n;
}

bool Sync1C::postDoc(QString obj, QString docKey)
{
    QJsonObject d;
    return postSync(obj+QString("(guid'%1')/Post?PostingModeOperational=false").arg(docKey),d);
}

int Sync1C::syncOpDoc(QString queryDoc, QString queryCont)
{
    int n=0;
    QJsonObject obj=tmpCatalog("tmpop.json");
    const QString objname = "Document_усОжидаемаяПриемка";

    QSqlQuery query;
    query.prepare(queryDoc);
    if (query.exec()){
        if (query.next()){
            QString num=query.value(1).toString();
            QString strDat=QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
            obj.insert("Number",num);
            obj.insert("Date", strDat);
            obj.insert("НомерКИС",num);
            obj.insert("ДатаКИС",query.value(2).toDate().toString("yyyy-MM-dd")+"T00:00:00");
            obj.insert("ИсточникПоступления_Key",postIstKeys.value(query.value(3).toString(),emptyKey));
            obj.insert("ДатаПоступления",strDat);
            obj.insert("Поклажедатель_Key",counterKeys.value(query.value(4).toString(),emptyKey));
            obj.insert("Контрагент_Key",counterKeys.value(query.value(5).toString(),emptyKey));
            obj.insert("СтадииПриемки_Key",constKeys.value(namStages,emptyKey));
            obj.insert("Организация_Key",constKeys.value(namCodOrg,emptyKey));
            obj.insert("ВоротаПриемки_Key",constKeys.value(namGates,emptyKey));

            QString docKey=getKey(objname,num,"Number");

            if (docKey==emptyKey){
                QJsonObject ret;
                bool ok= postSync(objname,obj, &ret);
                if (ok){
                    docKey=ret.value("Ref_Key").toString();
                    syncOpDocData(queryCont,docKey);
                    setPriemStatus(docKey);
                    postDoc(objname,docKey);
                }
            } else {
                int b = QMessageBox::question(nullptr,tr("Предупреждение"),QString("Документ с номером %1 уже существует. Перезаписать документ?").arg(num),QMessageBox::Yes,QMessageBox::No);
                if (b==QMessageBox::Yes){
                    QString stFilter = QString("InformationRegister_усСтатусыОжидаемыхПриемок?$filter=ОжидаемаяПриемка_Key eq guid'%1'").arg(docKey);
                    QJsonObject st= getSync(stFilter);
                    QJsonArray stAr=st.value("value").toArray();
                    if (stAr.size()){
                        if (stAr.at(0).toObject().value("Статус").toString()!=QString("Новая")){
                            showErrMes(tr("Можно перезаписать документ Ожидаемая приемка только со статусом 'Новая'"));
                        } else {
                            patchSync(objname+QString("(guid'%1')").arg(docKey),obj);
                            syncOpDocData(queryCont,docKey);
                            postDoc(objname,docKey);
                        }
                    } else {
                        patchSync(objname+QString("(guid'%1')").arg(docKey),obj);
                        syncOpDocData(queryCont,docKey);
                        setPriemStatus(docKey);
                        postDoc(objname,docKey);
                    }

                }
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }

    return n;
}

int Sync1C::syncOpDocData(QString queryCont, QString docKey)
{
    int i=1;
    QJsonObject obj=tmpCatalog("tmpopst.json");
    QSqlQuery query;
    query.prepare(queryCont);
    if (query.exec()){
        bool ok = deleteDocStr("Document_усСтрокаОжидаемойПриемки",docKey);
        while (query.next() && ok){
            //double masEd=query.value(5).toDouble();
            //int kvo = masEd!=0 ? query.value(6).toDouble()/masEd : 0;
            QString nomKey=catalogKeys.value(query.value(1).toString(),emptyKey);
            obj.insert("Number",QString::number(i));
            obj.insert("Date",QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));
            obj.insert("Владелец_Key",docKey);
            obj.insert("Номенклатура_Key",nomKey);
            obj.insert("УпаковкаНоменклатуры_Key",packKey(nomKey,query.value(4).toString()));
            obj.insert("КоличествоУпаковок",/*kvo*/query.value(6).toDouble());
            obj.insert("ПартияНоменклатуры_Key",partiKey(query.value(0).toString()));
            obj.insert("Количество",query.value(6).toDouble());
            obj.insert("ТипКонтейнера_Key",constKeys.value(namContType,emptyKey));
            obj.insert("НомерКонтейнера",contPrefix+"-"+query.value(7).toString());
            obj.insert("СтатусНоменклатуры_Key",constKeys.value(namStatus,emptyKey));

            postSync("Document_усСтрокаОжидаемойПриемки",obj);
            i++;
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return i;
}

int Sync1C::syncShipDocData(int id_ship, QString docKey)
{
    int i=1;
    QJsonObject obj=tmpCatalog("tmpshipst.json");
    QSqlQuery query;
    query.prepare("(select 'e:'||o.id_part as idp, p.id_el ||':'||(select id from diam as d where d.diam=p.diam) as kis, "
                  "p.n_s, p.dat_part, ep.pack_ed||'/'||ep.pack_group as npack, ep.mass_ed, o.massa, 'e:'||o.id as idship "
                  "from otpusk o "
                  "inner join parti p on p.id = o.id_part "
                  "inner join el_pack ep on ep.id = p.id_pack "
                  "where o.id_sert = :id_ship "
                  ") "
                  "union "
                  "(select 'w:'||wsc.id_wparti, wpm.id_provol ||':'||wpm.id_diam||':'||p.id_pack, "
                  "wpm.n_s, wpm.dat, "
                  "CASE WHEN wp.pack_group<>'-' THEN wp.pack_ed||'/'||wp.pack_group ELSE wp.pack_ed end as npack, "
                  "wp.mas_ed, wsc.m_netto, 'w:'||wsc.id "
                  "from wire_shipment_consist wsc "
                  "inner join wire_parti p on p.id = wsc.id_wparti "
                  "inner join wire_parti_m wpm on wpm.id = p.id_m "
                  "inner join wire_pack wp on wp.id = p.id_pack_type "
                  "where wsc.id_ship = :id_ship "
                  ") order by idship");
    query.bindValue(":id_ship",id_ship);
    if (query.exec()){
        bool ok = deleteDocStr("Document_усСтрокаЗаказаНаОтгрузку",docKey);
        while (query.next() && ok){
            //double masEd=query.value(5).toDouble();
            //int kvo = masEd!=0 ? query.value(6).toDouble()/masEd : 0;
            QString nomKey=catalogKeys.value(query.value(1).toString(),emptyKey);
            obj.insert("Number",QString::number(i));
            obj.insert("Date",QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));
            obj.insert("Владелец_Key",docKey);
            obj.insert("Номенклатура_Key",nomKey);
            obj.insert("УпаковкаНоменклатуры_Key",packKey(nomKey,query.value(4).toString()));
            obj.insert("КоличествоУпаковок",/*kvo*/query.value(6).toDouble());
            obj.insert("ПартияНоменклатуры_Key",partiKey(query.value(0).toString()));
            obj.insert("Количество",query.value(6).toDouble());
            obj.insert("СтатусНоменклатуры_Key",constKeys.value(namStatus,emptyKey));

            postSync("Document_усСтрокаЗаказаНаОтгрузку",obj);
            i++;
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return i;
}

int Sync1C::syncPartEl(int id_doc)
{    
    QString query = QString("select 'e:'||ad.id_part, p.id_el ||':'||(select id from diam as d where d.diam=p.diam) as kis, "
                           "p.n_s, p.dat_part, i.key1c, p.prim_prod, rn.nam "
                           "from acceptance_data ad "
                           "inner join parti p on p.id = ad.id_part "
                           "inner join istoch i on i.id = p.id_ist "
                           "left join rcp_nam rn on rn.id = p.id_rcp "
                           "where ad.id_acceptance = %1 "
                           "order by p.n_s, p.dat_part").arg(id_doc);
    return syncPart(query);
}

int Sync1C::syncPartWire(int id_doc)
{
    QString query = QString("select 'w:'||wp.id as id_part, wpm.id_provol||':'||wpm.id_diam||':'||wp.id_pack as kis, "
                            "wpm.n_s, wpm.dat, ws.key1c, null, pb.n_plav "
                            "from wire_acceptance_data wad "
                            "inner join wire_parti wp on wp.id = wad.id_part "
                            "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                            "inner join wire_source ws on ws.id = wpm.id_source "
                            "inner join  prov_buht pb on pb.id = wpm.id_buht "
                            "where wad.id_acceptance = %1 "
                            "order by wpm.n_s, wpm.dat").arg(id_doc);
    return syncPart(query);
}

int Sync1C::syncPartShip(int id_ship)
{
    int n=0;
    QString queryEl = QString("select 'e:'||o.id_part, p.id_el ||':'||(select id from diam as d where d.diam=p.diam) as kis, "
                              "p.n_s, p.dat_part, i.key1c, p.prim_prod, rn.nam "
                              "from otpusk o "
                              "inner join parti p on p.id = o.id_part "
                              "inner join istoch i on i.id = p.id_ist "
                              "left join rcp_nam rn on rn.id = p.id_rcp "
                              "where o.id_sert = %1 "
                              "order by p.n_s, p.dat_part").arg(id_ship);
    QString queryWire = QString("select 'w:'||wp.id as id_part, wpm.id_provol||':'||wpm.id_diam||':'||wp.id_pack as kis, "
                                "wpm.n_s, wpm.dat, ws.key1c, null, pb.n_plav "
                                "from wire_shipment_consist wsc "
                                "inner join wire_parti wp on wp.id = wsc.id_wparti "
                                "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                                "inner join wire_source ws on ws.id = wpm.id_source "
                                "inner join  prov_buht pb on pb.id = wpm.id_buht "
                                "where wsc.id_ship = %1 "
                                "order by wpm.n_s, wpm.dat").arg(id_ship);
    n+=syncPart(queryEl);
    n+=syncPart(queryWire);
    return n;
}

int Sync1C::syncOpDocEl(int id_doc)
{
    QString queryDoc = QString("select a.id, at2.prefix_el||date_part('year',a.dat)||'-'||a.num, a.dat, at2.nam, at2.codfromel, at2.codto  "
                              "from acceptance a "
                              "inner join acceptance_type at2 on at2.id = a.id_type "
                              "where a.id = %1").arg(id_doc);
    QString queryCont = QString("select 'e:'||ad.id_part, p.id_el ||':'||(select id from diam as d where d.diam=p.diam) as kis, "
                                "p.n_s, p.dat_part, ep.pack_ed||'/'||ep.pack_group, ep.mass_ed, ad.kvo, "
                                "at2.prefix_el ||date_part('year',a.dat) ||'-'||a.num ||'-'||ad.numcont as numcont "
                                "from acceptance_data ad "
                                "inner join acceptance a on a.id = ad.id_acceptance "
                                "inner join acceptance_type at2 on at2.id = a.id_type "
                                "inner join parti p on p.id = ad.id_part "
                                "inner join el_pack ep on ep.id = p.id_pack "
                                "where ad.id_acceptance = %1 "
                                "order by ad.id").arg(id_doc);
    return syncOpDoc(queryDoc,queryCont);
}

int Sync1C::syncOpDocWire(int id_doc)
{
    QString queryDoc = QString("select a.id, at2.prefix_wire||date_part('year',a.dat)||'-'||a.num, a.dat, at2.nam, at2.codfromwire, at2.codto  "
                               "from wire_acceptance a "
                               "inner join acceptance_type at2 on at2.id = a.id_type "
                               "where a.id = %1").arg(id_doc);
    QString queryCont = QString("select 'w:'||ad.id_part, wpm.id_provol ||':'||wpm.id_diam||':'||p.id_pack as kis, "
                                "wpm.n_s, wpm.dat, "
                                "CASE WHEN wp.pack_group<>'-' THEN wp.pack_ed||'/'||wp.pack_group ELSE wp.pack_ed end as npack, "
                                "wp.mas_ed, ad.kvo, "
                                "at2.prefix_wire ||date_part('year',wa.dat) ||'-'||wa.num ||'-'||ad.numcont as numcont "
                                "from wire_acceptance_data ad "
                                "inner join wire_acceptance wa on wa.id = ad.id_acceptance "
                                "inner join acceptance_type at2 on at2.id = wa.id_type "
                                "inner join wire_parti p on p.id = ad.id_part "
                                "inner join wire_parti_m wpm on wpm.id = p.id_m "
                                "inner join wire_pack wp on wp.id = p.id_pack_type "
                                "where ad.id_acceptance = %1 "
                                "order by ad.id").arg(id_doc);
    return syncOpDoc(queryDoc,queryCont);
}

int Sync1C::syncShipDoc(int id_ship)
{
    int n=0;
    const QString objname = "Document_усЗаказНаОтгрузку";
    QJsonObject obj=tmpCatalog("tmpship.json");
    QSqlQuery query;
    query.prepare("select s.nom_s, st.prefix||date_part('year',s.dat_vid)||'-'|| s.nom_s, s.dat_vid, s.id_pol, st.nam from sertifikat s "
                  "inner join sert_type st on st.id = s.id_type "
                  "where s.id = :id_ship");
    query.bindValue(":id_ship",id_ship);
    if (query.exec()){
        if (query.next()){
            QString num=query.value(1).toString();
            QString strDat=QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
            obj.insert("Number",query.value(0).toString());
            obj.insert("Date", strDat);
            obj.insert("НомерКИС",num);
            obj.insert("ДатаКИС",query.value(2).toDate().toString("yyyy-MM-dd")+"T00:00:00");
            obj.insert("ДатаОтгрузки",strDat);
            obj.insert("Организация_Key",constKeys.value(namCodOrg,emptyKey));
            obj.insert("НаправлениеОтгрузки_Key",shipTypeKeys.value(query.value(4).toString(),emptyKey));
            obj.insert("СтадииОтгрузки_Key",getKey("Catalog_усСтадииОтгрузки",namStages,"Description"));
            obj.insert("Контрагент_Key",getCounterKey(query.value(3).toInt()));

            QString docKey=getKey(objname,num,"НомерКИС");

            if (docKey==emptyKey){
                QJsonObject ret;
                bool ok= postSync(objname,obj, &ret);
                if (ok){
                    docKey=ret.value("Ref_Key").toString();
                    syncShipDocData(id_ship,docKey);
                    setShipStatus(docKey);
                    postDoc(objname,docKey);
                }
            } else {
                int b = QMessageBox::question(nullptr,tr("Предупреждение"),QString("Документ с номером %1 уже существует. Перезаписать документ?").arg(num),QMessageBox::Yes,QMessageBox::No);
                if (b==QMessageBox::Yes){
                    QString stFilter = QString("InformationRegister_усСтатусыЗаказовНаОтгрузку?$filter=ЗаказНаОтгрузку_Key eq guid'%1'").arg(docKey);
                    QJsonObject st= getSync(stFilter);
                    QJsonArray stAr=st.value("value").toArray();
                    if (stAr.size()){
                        if (stAr.at(0).toObject().value("Статус").toString()!=QString("Новый")){
                            showErrMes(tr("Можно перезаписать документ Заказ на отгрузку только со статусом 'Новый'"));
                        } else {
                            patchSync(objname+QString("(guid'%1')").arg(docKey),obj);
                            syncShipDocData(id_ship,docKey);
                            postDoc(objname,docKey);
                        }
                    } else {
                        patchSync(objname+QString("(guid'%1')").arg(docKey),obj);
                        syncShipDocData(id_ship,docKey);
                        setShipStatus(docKey);
                        postDoc(objname,docKey);
                    }

                }
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }

    return n;
}

bool Sync1C::checkEan(QString queryDoc, QString queryGen)
{
    QSqlQuery query;
    query.prepare(queryDoc);
    bool ok=query.exec();
    if (ok){
        while (query.next()){
            if (query.value(1).toString().isEmpty()){
                bool b=genEan(queryGen,query.value(0).toInt());
                ok=ok && b;
            }
        }
    } else {
        showErrMes(query.lastError().text());
    }
    return ok;
}

bool Sync1C::genEan(QString queryGen, int id_part)
{
    QSqlQuery query;
    query.prepare(queryGen);
    query.bindValue(":id_part",id_part);
    bool ok=query.exec();
    if (!ok){
        showErrMes(query.lastError().text());
    }
    return ok;
}

bool Sync1C::checkEanEl(int id_doc)
{
    QString queryDoc = QString("select ad.id_part, ee.ean_ed from acceptance_data ad "
                               "inner join parti p on p.id = ad.id_part "
                               "left join ean_el ee on ee.id_el = p.id_el and ee.id_diam = (select d.id from diam d where d.diam = p.diam) and ee.id_pack = p.id_pack "
                               "where ad.id_acceptance = %1").arg(id_doc);
    QString queryGen = QString("select * from add_ean_el( :id_part )");

    return checkEan(queryDoc, queryGen);
}

bool Sync1C::checkEanWire(int id_doc)
{
    QString queryDoc = QString("select ad.id_part, we.ean_ed from wire_acceptance_data ad "
                               "inner join wire_parti wp on wp.id = ad.id_part "
                               "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                               "left join wire_ean we on we.id_prov=wpm.id_provol and we.id_diam=wpm.id_diam and we.id_spool=wp.id_pack and we.id_pack=wp.id_pack_type "
                               "where ad.id_acceptance = %1").arg(id_doc);
    QString queryGen = QString("select * from add_ean_wire(:id_part)");

    return checkEan(queryDoc, queryGen);
}

bool Sync1C::checkEan(int id_ship)
{
    QString queryDocEl = QString("select o.id_part, ee.ean_ed from otpusk o "
                                 "inner join parti p on p.id = o.id_part "
                                 "left join ean_el ee on ee.id_el = p.id_el and ee.id_diam = (select d.id from diam d where d.diam = p.diam) and ee.id_pack = p.id_pack "
                                 "where o.id_sert = %1").arg(id_ship);
    QString queryGenEl = QString("select * from add_ean_el( :id_part )");
    QString queryDocWire = QString("select wsc.id_wparti, we.ean_ed from wire_shipment_consist wsc "
                                   "inner join wire_parti wp on wp.id = wsc.id_wparti "
                                   "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                                   "left join wire_ean we on we.id_prov=wpm.id_provol and we.id_diam=wpm.id_diam and we.id_spool=wp.id_pack and we.id_pack=wp.id_pack_type "
                                   "where wsc.id_ship = %1").arg(id_ship);
    QString queryGenWire = QString("select * from add_ean_wire(:id_part)");
    bool okEl = checkEan(queryDocEl,queryGenEl);
    bool okWire = checkEan(queryDocWire,queryGenWire);
    return okEl && okWire;
}
