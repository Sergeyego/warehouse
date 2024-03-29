#include "dialogreqload.h"
#include "ui_dialogreqload.h"

DialogReqLoad::DialogReqLoad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogReqLoad)
{
    ui->setupUi(this);

    createTmpTables();
    modelEl = new ModelEl(this);
    ui->tableViewEl->setModel(modelEl);
    ui->tableViewEl->setColumnWidth(0,50);
    ui->tableViewEl->setColumnWidth(1,200);
    ui->tableViewEl->setColumnWidth(2,100);
    ui->tableViewEl->setColumnWidth(3,50);
    ui->tableViewEl->setColumnWidth(4,120);
    ui->tableViewEl->setColumnWidth(5,100);
    ui->tableViewEl->setColumnWidth(6,80);
    ui->tableViewEl->setColumnWidth(7,80);

    modelWire = new ModelWire(this);
    ui->tableViewWire->setModel(modelWire);
    ui->tableViewWire->setColumnWidth(0,50);
    ui->tableViewWire->setColumnWidth(1,200);
    ui->tableViewWire->setColumnWidth(2,100);
    ui->tableViewWire->setColumnWidth(3,50);
    ui->tableViewWire->setColumnWidth(4,120);
    ui->tableViewWire->setColumnWidth(5,100);
    ui->tableViewWire->setColumnWidth(6,80);
    ui->tableViewWire->setColumnWidth(7,80);

    ui->comboBoxPol->setModel(Models::instance()->relPol->model());
    ui->comboBoxPol->setModelColumn(1);

    ui->comboBoxCat->setModel(Models::instance()->relKat->model());
    ui->comboBoxCat->setModelColumn(1);

    QStringList headerLabels;
    headerLabels<<tr("Имя файла")<<tr("Изменен");

    filesModel = new TableModel(this);
    filesModel->setHeader(headerLabels);
    ui->tableViewFiles->setModel(filesModel);

    ui->tableViewFiles->setColumnWidth(0,200);
    ui->tableViewFiles->setColumnWidth(1,115);

    ftpClient = new QFtp(this);
    updateFtpInfo();
    connect(ftpClient,SIGNAL(commandStarted(int)),this,SLOT(ftpCommandStart(int)));
    connect(ftpClient, SIGNAL(commandFinished(int,bool)),this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftpClient, SIGNAL(listInfo(QUrlInfo)),this, SLOT(addToList(QUrlInfo)));
    connect(ui->comboBoxPol,SIGNAL(currentIndexChanged(int)),this,SLOT(setHighPalette()));
    connect(ui->comboBoxCat,SIGNAL(currentIndexChanged(int)),this,SLOT(setHighPalette()));
    connect(ui->pushButtonCrePol,SIGNAL(clicked(bool)),this,SLOT(createPol()));
    connect(ui->pushButtonLoad,SIGNAL(clicked(bool)),this,SLOT(loadReq()));
    connect(ui->pushButtonCodes,SIGNAL(clicked(bool)),this,SLOT(codes1C()));

    connect(ui->tableViewFiles->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updData(QModelIndex)));

    clearData();
    updateList();
}

DialogReqLoad::~DialogReqLoad()
{
    delete ui;
}

bool DialogReqLoad::ftpGet(QString name)
{
    bool ok=(ftpClient->state()==QFtp::LoggedIn);
    if (!ok){
        ftpConnect();
    }
    int interval= ok ? 0 : delay;
    QTimer::singleShot(interval, [this, name]() {
        if (ftpClient->state()==QFtp::LoggedIn){
            QBuffer *buffer = new QBuffer();
            buffer->open(QIODevice::ReadWrite);
            int id=ftpClient->get(name,buffer);
            mapBuffer.insert(id,buffer);
            //qDebug()<<"get id="<<id;
        }
    } );
    return ok;
}

bool DialogReqLoad::ftpDel(QString name)
{
    bool ok=(ftpClient->state()==QFtp::LoggedIn);
    if (!ok){
        ftpConnect();
    }
    int interval= ok ? 0 : delay;
    QTimer::singleShot(interval, [this, name]() {
        if (ftpClient->state()==QFtp::LoggedIn){
            ftpClient->remove(name);
        }
    } );
    return ok;
}

void DialogReqLoad::parceXml(QIODevice *dev)
{
    modelEl->clearData();
    modelWire->clearData();
    double sum=0.0;
    dev->seek(0);
    xmldata=dev->readAll();
    dev->seek(0);
    QXmlStreamReader xml(dev);    
    while (!xml.atEnd() && !xml.hasError()){
        xml.readNextStartElement();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name()==QString("ЗаявкаПроизводство")){
                QXmlStreamAttributes attr=xml.attributes();
                ui->lineEditNum->setText(attr.value(QString("НомерЗаявкиДляРегистрации")).toString());
                ui->lineEditChange->setText(attr.value(QString("НомерИзмененийЗаявки")).toString());
                QString dateChange=attr.value(QString("ДатаИзменения")).toString();
                if (!dateChange.isEmpty()){
                    ui->dateEditChange->setDate(QDateTime::fromString(dateChange,"yyyy-MM-ddThh:mm:ss").date());
                }
                ui->lineEditComment->setText(attr.value(QString("КомментарийИзменения")).toString());
            } else if (xml.name()==QString("Заявка")){
                QXmlStreamAttributes attr=xml.attributes();
                ui->lineEditNum1C->setText(attr.value(QString("Номер")).toString());
                QString date=attr.value(QString("Дата")).toString();
                if (!date.isEmpty()){
                    ui->dateEditReq->setDate(QDateTime::fromString(date,"yyyy-MM-ddThh:mm:ss").date());
                }
                QString ter=attr.value(QString("СрокОтгрузки")).toString();
                if (!ter.isEmpty()){
                    ui->dateEditTer->setDate(QDateTime::fromString(ter,"yyyy-MM-ddThh:mm:ss").date());
                }
                ui->lineEditCat->setText(attr.value(QString("Подразделение")).toString());
                setCurrentCat(attr.value(QString("Подразделение")).toString());
            } else if (xml.name()==QString("Грузополучатель")){
                QXmlStreamAttributes attr=xml.attributes();
                ui->lineEditPol->setText(attr.value(QString("ПолноеНаименование")).toString());
                ui->lineEditInn->setText(attr.value(QString("ИНН")).toString());
                ui->lineEditKpp->setText(attr.value(QString("КПП")).toString());
                ui->lineEditOkpo->setText(attr.value(QString("ОКПО")).toString());
                ui->lineEditAdr->setText(attr.value(QString("ФактическийАдрес")).toString());
                setCurrentPol(attr.value(QString("ИНН")).toString());
            } else if (xml.name()==QString("Строки")){
                QXmlStreamAttributes attr=xml.attributes();
                QString type = attr.value(QString("Тип")).toString();
                QString code = attr.value(QString("Код")).toString();
                QString nom = attr.value(QString("Номенклатура")).toString();
                QString comment = attr.value(QString("Комментарий")).toString();
                double kvo = attr.value(QString("Количество")).toDouble();
                sum+=kvo;
                if (type!=QString("ПРОВОЛОКА")){
                    modelEl->addData(code,nom,kvo,comment);
                } else {
                    modelWire->addData(code,nom,kvo,comment);
                }
            }
        }
    }
    ui->labelItogo->setText(tr("ИТОГО: ")+QLocale().toString(sum,'f',2)+tr(" кг"));
    modelEl->select();
    modelWire->select();
    ui->pushButtonLoad->setEnabled(true);
}

void DialogReqLoad::setCurrentPol(QString inn)
{
    colVal val;
    val.val=-1;
    if (!inn.isEmpty()){
        QSqlQuery query;
        query.prepare("select p.id, p.str from poluch p where substring(p.innkpp::text, '\\m\\d*'::text) = :inn order by p.id desc");
        query.bindValue(":inn",inn);
        if (query.exec()){
            if (query.next()){
                val.val=query.value(0).toInt();
                val.disp=query.value(1).toString();
            }
        } else {
            QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
        }
    }
    ui->comboBoxPol->setCurrentData(val);
    ui->pushButtonCrePol->setEnabled(val.val==-1 && !inn.isEmpty());
    HighlightComboBox(ui->comboBoxPol);
}

void DialogReqLoad::setCurrentCat(QString cat)
{
    colVal val;
    val.val=-1;
    if (!cat.isEmpty()){
        QSqlQuery query;
        query.prepare("select p.id, p.nam from pol_kat p where p.tdnam = :cat");
        query.bindValue(":cat",cat);
        if (query.exec()){
            if (query.next()){
                val.val=query.value(0).toInt();
                val.disp=query.value(1).toString();
            }
        } else {
            QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
        }
    }
    ui->comboBoxCat->setCurrentData(val);
}

void DialogReqLoad::HighlightComboBox(QComboBox *combo)
{
    QPalette pal=combo->palette();
    QColor col = !combo->currentText().isEmpty() ? QColor(255,255,255): QColor(255,170,170);
    pal.setColor(QPalette::Normal, QPalette::Base, col);
    combo->setPalette(pal);
}

bool DialogReqLoad::check()
{
    QString err;
    if (ui->lineEditNum->text().isEmpty()){
        err+=QString::fromUtf8("Отсутствует номер заявки.\n");
    }
    if (ui->lineEditNum1C->text().isEmpty()){
        err+=QString::fromUtf8("Отсутствует номер 1С заявки.\n");
    }
    if (ui->dateEditReq->date()==ui->dateEditReq->minimumDate()){
        err+=QString::fromUtf8("Отсутствует дата заявки.\n");
    }
    if (ui->dateEditTer->date()==ui->dateEditTer->minimumDate()){
        err+=QString::fromUtf8("Отсутствует срок отгрузки.\n");
    }
    if (ui->comboBoxPol->currentText().isEmpty() || ui->comboBoxPol->getCurrentData().val.toInt()<0){
        err+=QString::fromUtf8("Отсутствует получатель. Нажмите кнопку \"Создать получателя\" или выберите получателя из списка.\n");
    }
    if (ui->comboBoxCat->currentText().isEmpty() || ui->comboBoxCat->getCurrentData().val.toInt()<0){
        err+=QString::fromUtf8("Не выбрано представительство.\n");
    }
    if (!modelEl->check()){
        err+=QString::fromUtf8("Не все соответствия кодов электродов.\n");
    }
    if (!modelWire->check()){
        err+=QString::fromUtf8("Не все соответствия кодов проволоки.\n");
    }

    bool ok=err.isEmpty();

    if (!ok){
        QMessageBox::critical(this,QString::fromUtf8("Ошибка"),err,QMessageBox::Ok);
    }
    return ok;
}

bool DialogReqLoad::insertNewRequest()
{
    QSqlQuery query;
    query.prepare("insert into requests (num, dat, id_rec, id_cat, comment, tdnum, xmldata) values (:num, :dat, :id_rec, :id_cat, :comment, :tdnum, :xmldata) returning id");
    query.bindValue(":num",ui->lineEditNum->text());
    query.bindValue(":dat",ui->dateEditReq->date());
    query.bindValue(":id_rec",ui->comboBoxPol->getCurrentData().val);
    query.bindValue(":id_cat",ui->comboBoxCat->getCurrentData().val);
    query.bindValue(":comment",ui->lineEditComment->text());
    query.bindValue(":tdnum",ui->lineEditNum1C->text());
    query.bindValue(":xmldata",xmldata);
    bool ok = query.exec();
    if (ok){
        if (query.next()){
            int id = query.value(0).toInt();
            QSqlQuery queryEl;
            queryEl.prepare("insert into requests_el (id_req, id_el, id_diam, id_var, id_pack, plan, comment, dat_term) (select :id_req, id_el, id_diam, id_var, id_pack, kvo, comm, :dat from tmp_req_el)");
            queryEl.bindValue(":id_req",id);
            queryEl.bindValue(":dat",ui->dateEditTer->date());
            if (!queryEl.exec()){
                ok=false;
                QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryEl.lastError().text());
            }
            QSqlQuery queryWire;
            queryWire.prepare("insert into requests_wire (id_req, id_provol, id_diam, id_spool, id_pack, plan, comment, dat_term) (select :id_req, id_prov, id_diam, id_spool, id_pack, kvo, comm, :dat from tmp_req_wire)");
            queryWire.bindValue(":id_req",id);
            queryWire.bindValue(":dat",ui->dateEditTer->date());
            if (!queryWire.exec()){
                ok=false;
                QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryWire.lastError().text());
            }
            insertChange(id);
        }
    } else {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
    return ok;
}

bool DialogReqLoad::updateRequest(int id)
{
    insertChange(id);

    QSqlQuery query;
    query.prepare("update requests set xmldata = :xmldata where id = :id");
    query.bindValue(":id",id);
    query.bindValue(":xmldata",xmldata);
    bool ok1 = query.exec();
    if (!ok1){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }

    bool ok2 = updateRequestEl(id);
    bool ok3 = updateRequestWire(id);
    return ok1 && ok2 && ok3;
}

void DialogReqLoad::insertChange(int id)
{
    if (ui->dateEditChange->date()!=ui->dateEditChange->minimumDate()){
        QSqlQuery query;
        query.prepare("insert into requests_changes (id_req, nam, dat) values (:id_req, :nam, :dat ) ON CONFLICT (id_req, nam, dat) DO NOTHING");
        query.bindValue(":id_req",id);
        query.bindValue(":nam",ui->lineEditChange->text());
        query.bindValue(":dat",ui->dateEditChange->date());
        if (!query.exec()){
            QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
        }
    }
    QSqlQuery queryComm;
    queryComm.prepare("update requests set comment = comment || (CASE WHEN comment = '' THEN '' ELSE '; ' END) || :comm1 where id = :id and comment <> :comm2");
    queryComm.bindValue(":id",id);
    queryComm.bindValue(":comm1",ui->lineEditComment->text());
    queryComm.bindValue(":comm2",ui->lineEditComment->text());
    if (!queryComm.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryComm.lastError().text());
    }
}

bool DialogReqLoad::updateRequestEl(int id)
{
    bool ok=true;

    if (!modelEl->isEmpty()){
        for (int i=0; i<modelEl->rowCount(); i++){
            int id_el = modelEl->data(modelEl->index(i,2),Qt::EditRole).toInt();
            int id_diam = modelEl->data(modelEl->index(i,3),Qt::EditRole).toInt();
            int id_pack = modelEl->data(modelEl->index(i,4),Qt::EditRole).toInt();
            int id_var = modelEl->data(modelEl->index(i,5),Qt::EditRole).toInt();
            double kvo = modelEl->data(modelEl->index(i,6),Qt::EditRole).toDouble();
            QString comm = modelEl->data(modelEl->index(i,7),Qt::EditRole).toString();
            QSqlQuery queryUpd;
            queryUpd.prepare("update requests_el set cor = :kvo-plan, comment = :comm where id_req=:id_req and id_el=:id_el and id_diam=:id_diam and id_var=:id_var and id_pack=:id_pack and dat_term=:dat_term");
            queryUpd.bindValue(":kvo",kvo);
            queryUpd.bindValue(":comm",comm);
            queryUpd.bindValue(":id_req",id);
            queryUpd.bindValue(":id_el",id_el);
            queryUpd.bindValue(":id_diam",id_diam);
            queryUpd.bindValue(":id_pack",id_pack);
            queryUpd.bindValue(":id_var",id_var);
            queryUpd.bindValue(":dat_term",ui->dateEditTer->date());
            ok=queryUpd.exec();
            if (!ok){
                QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryUpd.lastError().text());
            }
        }
    }

    QSqlQuery queryDel;
    queryDel.prepare("update requests_el set cor = -plan where id_req = :id_req and (id_el||'#'||id_diam||'#'||id_var||'#'||id_pack||'#'||to_char(dat_term,'YYYY-MM-DD')) not in "
                     "(select (id_el||'#'||id_diam||'#'||id_var||'#'||id_pack||'#'||to_char(:dat_term::date,'YYYY-MM-DD')) from tmp_req_el)");
    queryDel.bindValue(":id_req",id);
    queryDel.bindValue(":dat_term",ui->dateEditTer->date());
    bool ok2 = queryDel.exec();
    if (!ok2){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryDel.lastError().text());
    }

    QSqlQuery queryIns;
    queryIns.prepare("insert into requests_el (id_req, id_el, id_diam, id_var, id_pack, comment, cor, dat_term) "
                     "(select :id_req1, t.id_el, t.id_diam, t.id_var, t.id_pack, t.comm, t.kvo, :dat_term1 from tmp_req_el as t where "
                     "(t.id_el||'#'||t.id_diam||'#'||t.id_var||'#'||t.id_pack||'#'||to_char(:dat_term2::date,'YYYY-MM-DD')) not in "
                     "(select (r.id_el||'#'||r.id_diam||'#'||r.id_var||'#'||r.id_pack||'#'||to_char(r.dat_term,'YYYY-MM-DD')) from requests_el as r where r.id_req = :id_req2 ) )");
    queryIns.bindValue(":id_req1",id);
    queryIns.bindValue(":id_req2",id);
    queryIns.bindValue(":dat_term1",ui->dateEditTer->date());
    queryIns.bindValue(":dat_term2",ui->dateEditTer->date());
    bool ok3 = queryIns.exec();
    if (!ok3){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryIns.lastError().text());
    }

    ok = ok && ok2 && ok3;

    return ok;
}

bool DialogReqLoad::updateRequestWire(int id)
{
    bool ok=true;

    if (!modelWire->isEmpty()){
        for (int i=0; i<modelWire->rowCount(); i++){
            int id_prov = modelWire->data(modelWire->index(i,2),Qt::EditRole).toInt();
            int id_diam = modelWire->data(modelWire->index(i,3),Qt::EditRole).toInt();
            int id_spool = modelWire->data(modelWire->index(i,4),Qt::EditRole).toInt();
            int id_pack = modelWire->data(modelWire->index(i,5),Qt::EditRole).toInt();
            double kvo = modelWire->data(modelWire->index(i,6),Qt::EditRole).toDouble();
            QString comm = modelWire->data(modelWire->index(i,7),Qt::EditRole).toString();
            QSqlQuery queryUpd;
            queryUpd.prepare("update requests_wire set cor = :kvo-plan, comment = :comm where id_req=:id_req and id_provol=:id_prov and id_diam=:id_diam and id_spool=:id_spool and id_pack=:id_pack and dat_term=:dat_term");
            queryUpd.bindValue(":kvo",kvo);
            queryUpd.bindValue(":comm",comm);
            queryUpd.bindValue(":id_req",id);
            queryUpd.bindValue(":id_prov",id_prov);
            queryUpd.bindValue(":id_diam",id_diam);
            queryUpd.bindValue(":id_pack",id_pack);
            queryUpd.bindValue(":id_spool",id_spool);
            queryUpd.bindValue(":dat_term",ui->dateEditTer->date());
            ok=queryUpd.exec();
            if (!ok){
                QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryUpd.lastError().text());
            }
        }
    }

    QSqlQuery queryDel;
    queryDel.prepare("update requests_wire set cor = -plan where id_req = :id_req and (id_provol||'#'||id_diam||'#'||id_spool||'#'||id_pack||'#'||to_char(dat_term,'YYYY-MM-DD')) not in "
                     "(select (id_prov||'#'||id_diam||'#'||id_spool||'#'||id_pack||'#'||to_char(:dat_term::date,'YYYY-MM-DD')) from tmp_req_wire)");
    queryDel.bindValue(":id_req",id);
    queryDel.bindValue(":dat_term",ui->dateEditTer->date());
    bool ok2 = queryDel.exec();
    if (!ok2){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryDel.lastError().text());
    }

    QSqlQuery queryIns;
    queryIns.prepare("insert into requests_wire (id_req, id_provol, id_diam, id_spool, id_pack, comment, cor, dat_term) "
                     "(select :id_req1, t.id_prov, t.id_diam, t.id_spool, t.id_pack, t.comm, t.kvo, :dat_term1 from tmp_req_wire as t where "
                     "(t.id_prov||'#'||t.id_diam||'#'||t.id_spool||'#'||t.id_pack||'#'||to_char(:dat_term2::date,'YYYY-MM-DD')) not in "
                     "(select (r.id_provol||'#'||r.id_diam||'#'||r.id_spool||'#'||r.id_pack||'#'||to_char(r.dat_term,'YYYY-MM-DD')) from requests_wire as r where r.id_req = :id_req2 ) )");
    queryIns.bindValue(":id_req1",id);
    queryIns.bindValue(":id_req2",id);
    queryIns.bindValue(":dat_term1",ui->dateEditTer->date());
    queryIns.bindValue(":dat_term2",ui->dateEditTer->date());
    bool ok3 = queryIns.exec();
    if (!ok3){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryIns.lastError().text());
    }

    ok = ok && ok2 && ok3;

    return ok;
}

void DialogReqLoad::syncPol()
{
    int id_pol=ui->comboBoxPol->getCurrentData().val.toInt();
    QSqlQuery query;
    QString naim, adres, innkpp, okpo, inn, kpp;
    query.prepare("select p.naim, p.adres, p.innkpp, p.okpo from poluch p where p.id = :id_pol ");
    query.bindValue(":id_pol",id_pol);
    if (query.exec()){
        if (query.next()){
            naim=query.value(0).toString();
            adres=query.value(1).toString();
            innkpp=query.value(2).toString();
            okpo=query.value(3).toString();
        }
    } else {
        QMessageBox::critical(this, tr("Ошибка"),query.lastError().text());
        return;
    }
    QStringList l = innkpp.split('/');
    if (l.size()){
        inn=l.at(0);
        if (l.size()>1){
            kpp=l.at(1);
        }
    }
    QString message;
    if (!ui->lineEditPol->text().isEmpty() && naim!=ui->lineEditPol->text()){
        message+=tr("Отличается полное наименование получателя: в справочнике \"")+naim+tr("\", в заявке: \"")+ui->lineEditPol->text()+tr("\";\n");
    }
    if (!ui->lineEditInn->text().isEmpty() && inn!=ui->lineEditInn->text()){
        message+=tr("Отличается ИНН получателя: в справочнике \"")+inn+tr("\", в заявке: \"")+ui->lineEditInn->text()+tr("\";\n");
    }
    if (!ui->lineEditKpp->text().isEmpty() &&  kpp!=ui->lineEditKpp->text()){
        message+=tr("Отличается КПП получателя: в справочнике \"")+kpp+tr("\", в заявке: \"")+ui->lineEditKpp->text()+tr("\";\n");
    }
    if (!ui->lineEditAdr->text().isEmpty() && adres!=ui->lineEditAdr->text()){
        message+=tr("Отличается адрес получателя: в справочнике \"")+adres+tr("\", в заявке: \"")+ui->lineEditAdr->text()+tr("\";\n");
    }
    if (!ui->lineEditOkpo->text().isEmpty() && okpo!=ui->lineEditOkpo->text()){
        message+=tr("Отличается ОКПО получателя: в справочнике \"")+okpo+tr("\", в заявке: \"")+ui->lineEditOkpo->text()+tr("\";\n");
    }

    if (!message.isEmpty()){
        message+=tr("Обновить данные в справочнике?");
        int n = QMessageBox::question(this,tr("Данные не совпадают"),message,QMessageBox::Yes, QMessageBox::No);
        if (n==QMessageBox::Yes){
            QString innkppreq=ui->lineEditInn->text();
            if (!innkppreq.isEmpty() && !ui->lineEditKpp->text().isEmpty()){
                innkppreq+="/"+ui->lineEditKpp->text();
            }
            QSqlQuery queryUpd;
            queryUpd.prepare("update poluch set naim = :naim, adres = :adres, innkpp = :innkpp, okpo = :okpo where id = :id_pol");
            queryUpd.bindValue(":id_pol",id_pol);
            queryUpd.bindValue(":naim",ui->lineEditPol->text().isEmpty() ? naim : ui->lineEditPol->text());
            queryUpd.bindValue(":adres",ui->lineEditAdr->text().isEmpty() ? adres : ui->lineEditAdr->text());
            queryUpd.bindValue(":innkpp",ui->lineEditInn->text().isEmpty() ? innkpp : innkppreq);
            queryUpd.bindValue(":okpo",ui->lineEditOkpo->text().isEmpty() ? okpo : ui->lineEditOkpo->text());
            if (!queryUpd.exec()){
                QMessageBox::critical(this, tr("Ошибка"),queryUpd.lastError().text());
            }
        }
    }
}

void DialogReqLoad::codes1C()
{
    DialogCods d;
    d.exec();
    updData(ui->tableViewFiles->currentIndex());
}

void DialogReqLoad::updData(QModelIndex index)
{
    if (index.isValid()){
        QString name = ui->tableViewFiles->model()->data(ui->tableViewFiles->model()->index(index.row(),0),Qt::EditRole).toString();
        clearData();
        ftpGet(name);
    }

}

void DialogReqLoad::ftpConnect()
{
    ftpClient->connectToHost(ftphost);
}

void DialogReqLoad::updateList()
{
    if (ftpClient->state()==QFtp::LoggedIn){
        ftpClient->list();
    } else {
        ftpConnect();
    }
}

void DialogReqLoad::ftpCommandFinished(int commandId, bool error)
{
    if (ftpClient->currentCommand() == QFtp::Get){
        if (!error) {
            //qDebug()<<"get finished id="<<commandId;
            QBuffer *buffer = mapBuffer.value(commandId);
            if (buffer){
                if (buffer->isOpen()){
                    parceXml(buffer);
                    buffer->close();
                }
                buffer->deleteLater();
            }
            mapBuffer.remove(commandId);
        }
    }
    if (error) {
        QMessageBox::critical(NULL, tr("FTP"),ftpClient->errorString());
    } else {
        if (ftpClient->currentCommand() == QFtp::ConnectToHost) {
            ftpClient->login(ftpuser,ftppassword);
        } else if (ftpClient->currentCommand() == QFtp::Login){
            ftpClient->cd(ftppath);
        } /*else if (ftpClient->currentCommand() == QFtp::List) {
            //qDebug()<<"list finished";
        }*/ else if (ftpClient->currentCommand()==QFtp::Cd){
            ftpClient->list();
        } else if (ftpClient->currentCommand()==QFtp::Remove){
            //ftpClient->list();
            accept();
        }
    }
}

void DialogReqLoad::ftpCommandStart(int /*commandId*/)
{
    if (ftpClient->currentCommand()==QFtp::List){
        filesModel->clear();
        //qDebug()<<"Start list";
    }
}

void DialogReqLoad::addToList(const QUrlInfo &urlInfo)
{
    if (urlInfo.isFile()){
        QString name=urlInfo.name();
        if (name.right(4).toLower()==QString(".xml")){
            int row=filesModel->rowCount();
            filesModel->insertRow(row);
            filesModel->setData(filesModel->index(row,0),name,Qt::EditRole);
            filesModel->setData(filesModel->index(row,1),urlInfo.lastModified(),Qt::EditRole);
        }
    }
}

void DialogReqLoad::clearData()
{
    ui->lineEditNum->clear();
    ui->lineEditNum1C->clear();
    ui->dateEditReq->setDate(ui->dateEditReq->minimumDate());
    ui->dateEditChange->setDate(ui->dateEditChange->minimumDate());
    ui->dateEditTer->setDate(ui->dateEditTer->minimumDate());
    ui->lineEditChange->clear();
    ui->lineEditComment->clear();
    ui->lineEditPol->clear();
    ui->lineEditInn->clear();
    ui->lineEditKpp->clear();
    ui->lineEditOkpo->clear();
    ui->lineEditAdr->clear();
    ui->comboBoxPol->setCurrentIndex(-1);
    ui->lineEditCat->clear();
    ui->comboBoxCat->setCurrentIndex(-1);
    modelEl->clearData();
    modelWire->clearData();
    ui->labelItogo->setText(tr("ИТОГО: "));
    ui->pushButtonLoad->setEnabled(false);
    xmldata.clear();
}

void DialogReqLoad::setHighPalette()
{
    DbComboBox *combo = qobject_cast<DbComboBox *>(sender());
    if (combo){
        HighlightComboBox(combo);
    }
}

void DialogReqLoad::createPol()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Добавить получателя"),
                                         tr("Введите краткое наименование получателя:"), QLineEdit::Normal,
                                         ui->lineEditPol->text(), &ok);
    if (ok && !text.isEmpty()){
        QString innkpp=ui->lineEditInn->text();
        QString kpp=ui->lineEditKpp->text();
        if (!innkpp.isEmpty() && !kpp.isEmpty()){
            innkpp+="/"+kpp;
        }
        QSqlQuery query;
        query.prepare("insert into poluch (naim, short, adres, id_kat, innkpp, okpo) values (:naim, :short, :adres, :id_kat, :innkpp, :okpo)");
        query.bindValue(":naim",ui->lineEditPol->text());
        query.bindValue(":short",text);
        query.bindValue(":adres",ui->lineEditAdr->text());
        query.bindValue(":id_kat",ui->comboBoxCat->getCurrentData().val);
        query.bindValue(":innkpp",innkpp);
        query.bindValue(":okpo",ui->lineEditOkpo->text());
        if (query.exec()){
            Models::instance()->relPol->refreshModel();
            setCurrentPol(ui->lineEditInn->text());
        } else {
            QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
        }
    }
}

void DialogReqLoad::loadReq()
{
    if (!check()){
        return;
    }
    syncPol();
    QString fileName=ui->tableViewFiles->model()->data(ui->tableViewFiles->model()->index(ui->tableViewFiles->currentIndex().row(),0),Qt::EditRole).toString();
    QSqlQuery query;
    query.prepare("select id from requests where num = :num and tdnum = :tdnum and date_part('year',dat) = :year ");
    query.bindValue(":num",ui->lineEditNum->text());
    query.bindValue(":tdnum",ui->lineEditNum1C->text());
    query.bindValue(":year",ui->dateEditReq->date().year());
    if (query.exec()){
        bool exist = query.size() > 0;
        if (!exist){
            if (insertNewRequest()){
                ftpDel(fileName);
            }
        } else {
            int n = QMessageBox::question(this,tr("Подтвердите корректировку"),tr("Заявка с таким номером уже существует. Заявка будет скорректирована. Продолжить?"),QMessageBox::Yes,QMessageBox::No);
            if (n==QMessageBox::Yes && query.next()){
                int id=query.value(0).toInt();
                if (updateRequest(id)){
                    ftpDel(fileName);
                }
            }
        }
    } else {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
}

void DialogReqLoad::updateFtpInfo()
{
    QSqlQuery query;
    query.prepare("select host_int, user_rw, pass_rw, path_req, conn_delay from ftp_info where id = 1");
    if (query.exec()){
        while (query.next()){
            ftphost=query.value(0).toString();
            ftpuser=query.value(1).toString();
            ftppassword=query.value(2).toString();
            ftppath=query.value(3).toString();
            delay=query.value(4).toInt();
        }
    } else {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
}

void DialogReqLoad::createTmpTables()
{
    QSqlQuery query;
    query.prepare("select * from create_tmp_req_tables()");
    if (!query.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
}

ModelEl::ModelEl(QWidget *parent) : DbTableModel("tmp_req_el",parent)
{
    addColumn("cod",tr("Код"));
    addColumn("nam",tr("Номенклатура"));
    addColumn("id_el",tr("Марка"),Models::instance()->relElrtr);
    addColumn("id_diam",tr("Диам."),Models::instance()->relDiam);
    addColumn("id_pack",tr("Упаковка"),Models::instance()->relElPack);
    addColumn("id_var",tr("Вариант"),Models::instance()->relVars);
    addColumn("kvo",tr("Кол-во"));
    addColumn("comm",tr("Коммент."));
    setSort("tmp_req_el.nam");

    setColumnFlags(0,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setColumnFlags(1,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setColumnFlags(6,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    select();
}

QVariant ModelEl::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        bool ok=true;
        for (int i=2; i<=5; i++){
            ok = ok && (!this->data(this->index(index.row(),i),Qt::EditRole).isNull());
        }
        if (!ok && !this->data(this->index(index.row(),0),Qt::EditRole).isNull()){
            return QColor(255,170,170);
        }
    }
    return DbTableModel::data(index,role);
}

bool ModelEl::insertRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}

void ModelEl::addData(QString code, QString nom, double kvo, QString comment)
{
    QVariant id_el, id_diam, id_pack, id_var;
    QSqlQuery queryVar;
    queryVar.prepare("select id_el, id_diam, id_pack, id_var from td_keys_el where ltrim(cod,'0') = ltrim(:cod,'0') ");
    queryVar.bindValue(":cod",code);
    if (queryVar.exec()){
        if (queryVar.next()){
            id_el=queryVar.value(0);
            id_diam=queryVar.value(1);
            id_pack=queryVar.value(2);
            id_var=queryVar.value(3);
        }
    } else {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryVar.lastError().text());
    }

    QSqlQuery query;
    query.prepare("insert into tmp_req_el (cod, nam, kvo, comm, id_el, id_diam, id_pack, id_var) values (:cod, :nam, :kvo, :comm, :id_el, :id_diam, :id_pack, :id_var )");
    query.bindValue(":cod",code);
    query.bindValue(":nam",nom);
    query.bindValue(":kvo",kvo);
    query.bindValue(":comm",comment);
    query.bindValue(":id_el",id_el);
    query.bindValue(":id_diam",id_diam);
    query.bindValue(":id_pack",id_pack);
    query.bindValue(":id_var",id_var);
    if (!query.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
}

void ModelEl::clearData()
{
    QSqlQuery query;
    query.prepare("delete from tmp_req_el");
    if (!query.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    } else {
        select();
    }
}

bool ModelEl::check()
{
    bool ok=true;
    for (int i=0; i<this->rowCount(); i++){
        for (int j=2; j<=5; j++){
            ok = ok && (!this->data(this->index(i,j),Qt::EditRole).isNull());
        }
    }
    return ok || this->isEmpty();
}

ModelWire::ModelWire(QWidget *parent) : DbTableModel("tmp_req_wire",parent)
{
    addColumn("cod",tr("Код"));
    addColumn("nam",tr("Номенклатура"));
    addColumn("id_prov",tr("Марка"),Models::instance()->relProvol);
    addColumn("id_diam",tr("Диам."),Models::instance()->relDiam);
    addColumn("id_spool",tr("Носитель"),Models::instance()->relSpool);
    addColumn("id_pack",tr("Упаковка"),Models::instance()->relWirePack);
    addColumn("kvo",tr("Кол-во"));
    addColumn("comm",tr("Коммент."));
    setSort("tmp_req_wire.nam");

    setColumnFlags(0,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setColumnFlags(1,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setColumnFlags(6,Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    select();
}

QVariant ModelWire::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        bool ok=true;
        for (int i=2; i<=5; i++){
            ok = ok && (!this->data(this->index(index.row(),i),Qt::EditRole).isNull());
        }
        if (!ok && !this->data(this->index(index.row(),0),Qt::EditRole).isNull()){
            return QColor(255,170,170);
        }
    }
    return DbTableModel::data(index,role);
}

bool ModelWire::insertRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}

void ModelWire::addData(QString code, QString nom, double kvo, QString comment)
{
    QVariant id_prov, id_diam, id_spool, id_pack;
    QSqlQuery queryVar;
    queryVar.prepare("select id_prov, id_diam, id_spool, id_pack from td_keys_wire where ltrim(cod,'0') = ltrim(:cod,'0') ");
    queryVar.bindValue(":cod",code);
    if (queryVar.exec()){
        if (queryVar.next()){
            id_prov=queryVar.value(0);
            id_diam=queryVar.value(1);
            id_spool=queryVar.value(2);
            id_pack=queryVar.value(3);
        }
    } else {
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),queryVar.lastError().text());
    }

    QSqlQuery query;
    query.prepare("insert into tmp_req_wire (cod, nam, kvo, comm, id_prov, id_diam, id_spool, id_pack) values (:cod, :nam, :kvo, :comm, :id_prov, :id_diam, :id_spool, :id_pack )");
    query.bindValue(":cod",code);
    query.bindValue(":nam",nom);
    query.bindValue(":kvo",kvo);
    query.bindValue(":comm",comment);
    query.bindValue(":id_prov",id_prov);
    query.bindValue(":id_diam",id_diam);
    query.bindValue(":id_spool",id_spool);
    query.bindValue(":id_pack",id_pack);
    if (!query.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    }
}

void ModelWire::clearData()
{
    QSqlQuery query;
    query.prepare("delete from tmp_req_wire");
    if (!query.exec()){
        QMessageBox::critical(NULL, QString::fromUtf8("Ошибка"),query.lastError().text());
    } else {
        select();
    }
}

bool ModelWire::check()
{
    bool ok=true;
    for (int i=0; i<this->rowCount(); i++){
        for (int j=2; j<=5; j++){
            ok = ok && (!this->data(this->index(i,j),Qt::EditRole).isNull());
        }
    }
    return ok || this->isEmpty();
}
