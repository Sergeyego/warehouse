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

    QStringList headerLabels;
    headerLabels<<tr("Имя файла")<<tr("Изменен");

    filesModel = new TableModel(this);
    filesModel->setHeader(headerLabels);
    ui->tableViewFiles->setModel(filesModel);

    ui->tableViewFiles->setColumnWidth(0,200);
    ui->tableViewFiles->setColumnWidth(1,110);

    ftpClient = new QFtp(this);
    updateFtpInfo();
    connect(ftpClient,SIGNAL(commandStarted(int)),this,SLOT(ftpCommandStart(int)));
    connect(ftpClient, SIGNAL(commandFinished(int,bool)),this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftpClient, SIGNAL(listInfo(QUrlInfo)),this, SLOT(addToList(QUrlInfo)));

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

void DialogReqLoad::parceXml(QIODevice *dev)
{
    modelEl->clearData();
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
            } else if (xml.name()==QString("Грузополучатель")){
                QXmlStreamAttributes attr=xml.attributes();
                ui->lineEditPol->setText(attr.value(QString("ПолноеНаименование")).toString());
                ui->lineEditInn->setText(attr.value(QString("ИНН")).toString());
                ui->lineEditKpp->setText(attr.value(QString("КПП")).toString());
                ui->lineEditOkpo->setText(attr.value(QString("ОКПО")).toString());
                ui->lineEditAdr->setText(attr.value(QString("ФактическийАдрес")).toString());
            } else if (xml.name()==QString("Строки")){
                QXmlStreamAttributes attr=xml.attributes();
                QString type = attr.value(QString("Тип")).toString();
                QString code = attr.value(QString("Код")).toString();
                QString nom = attr.value(QString("Номенклатура")).toString();
                QString comment = attr.value(QString("Комментарий")).toString();
                double kvo = attr.value(QString("Количество")).toDouble();
                //qDebug()<<type<<code<<nom<<kvo<<comment;
                if (type!=QString("ПРОВОЛОКА")){
                    modelEl->addData(code,nom,kvo,comment);
                }
            }
            //qDebug()<<xml.name();
        }
    }
    modelEl->select();
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
                    buffer->seek(0);
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
        } else if (ftpClient->currentCommand() == QFtp::List) {
            //qDebug()<<"list finished";
        } else if (ftpClient->currentCommand()==QFtp::Cd){
            ftpClient->list();
        } else if (ftpClient->currentCommand()==QFtp::Remove){
            ftpClient->list();
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

    select();
}

QVariant ModelEl::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole){
        bool ok=true;
        for (int i=2; i<=5; i++){
            ok = ok && (!this->data(this->index(index.row(),i),Qt::EditRole).isNull());
        }
        if (!ok){
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
