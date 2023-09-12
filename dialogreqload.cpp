#include "dialogreqload.h"
#include "ui_dialogreqload.h"

DialogReqLoad::DialogReqLoad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogReqLoad)
{
    ui->setupUi(this);

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
            if (buffer.isOpen()){
                buffer.close();
            }
            buffer.open(QIODevice::ReadWrite);
            ftpClient->get(name,&buffer);
        }
    } );
    return ok;
}

void DialogReqLoad::parceXml(QIODevice *dev)
{
    clearData();
    QXmlStreamReader xml(dev);
    while (!xml.atEnd() && !xml.hasError()){
        xml.readNextStartElement();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name()==QString("ЗаявкаПроизводство")){
                QXmlStreamAttributes attr=xml.attributes();
                for (QXmlStreamAttribute a : attr){
                    //qDebug()<<a.name()<<a.value();
                    if (a.name()==QString("НомерЗаявкиДляРегистрации")){
                        ui->lineEditNum->setText(a.value().toString());
                    } else if (a.name()==QString("НомерИзмененийЗаявки")){
                        ui->lineEditChange->setText(a.value().toString());
                    } else if (a.name()==QString("ДатаИзменения")){
                        ui->dateEditChange->setDate(QDateTime::fromString(a.value().toString(),"yyyy-MM-ddThh:mm:ss").date());
                    } else if (a.name()==QString("КомментарийИзменения")){
                        ui->lineEditComment->setText(a.value().toString());
                    }
                }
            } else if (xml.name()==QString("Заявка")){
                QXmlStreamAttributes attr=xml.attributes();
                for (QXmlStreamAttribute a : attr){
                    //qDebug()<<a.name()<<a.value();
                    if (a.name()==QString("Номер")){
                        ui->lineEditNum1C->setText(a.value().toString());
                    } else if (a.name()==QString("Дата")){
                        ui->dateEditReq->setDate(QDateTime::fromString(a.value().toString(),"yyyy-MM-ddThh:mm:ss").date());
                    } else if (a.name()==QString("СрокОтгрузки")){
                        ui->dateEditTer->setDate(QDateTime::fromString(a.value().toString(),"yyyy-MM-ddThh:mm:ss").date());
                    }
                }
            } else if (xml.name()==QString("Грузополучатель")){
                QXmlStreamAttributes attr=xml.attributes();
                for (QXmlStreamAttribute a : attr){
                    //qDebug()<<a.name()<<a.value();
                    if (a.name()==QString("ПолноеНаименование")){
                        ui->lineEditPol->setText(a.value().toString());
                    } else if (a.name()==QString("ИНН")){
                        ui->lineEditInn->setText(a.value().toString());
                    } else if (a.name()==QString("КПП")){
                        ui->lineEditKpp->setText(a.value().toString());
                    } else if (a.name()==QString("ОКПО")){
                        ui->lineEditOkpo->setText(a.value().toString());
                    } else if (a.name()==QString("ФактическийАдрес")){
                        ui->lineEditAdr->setText(a.value().toString());
                    }
                }
            } else if (xml.name()==QString("Строки")){
                QXmlStreamAttributes attr=xml.attributes();
                for (QXmlStreamAttribute a : attr){
                    qDebug()<<a.name()<<a.value();
                }
            }
            //qDebug()<<xml.name();
        }
    }
}

void DialogReqLoad::updData(QModelIndex index)
{
    if (index.isValid()){
        QString name = ui->tableViewFiles->model()->data(ui->tableViewFiles->model()->index(index.row(),0),Qt::EditRole).toString();
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

void DialogReqLoad::ftpCommandFinished(int /*commandId*/, bool error)
{
    if (ftpClient->currentCommand() == QFtp::Get){
        //getFile->close();
        if (!error) {
            buffer.seek(0);
            parceXml(&buffer);
            //qDebug()<<buffer.readAll();
        }
        //getFile->remove();
        //delete getFile;
    }
    if (error) {
        QMessageBox::critical(NULL, tr("FTP"),ftpClient->errorString());
    } else {
        if (ftpClient->currentCommand() == QFtp::ConnectToHost) {
            ftpClient->login(QString(ftpuser),QString(ftppassword));
        } else if (ftpClient->currentCommand() == QFtp::Login){
            ftpClient->cd(ftppath);
        } else if (ftpClient->currentCommand() == QFtp::List) {
            //emit dataChanged(this->index(0,0),this->index(rowCount()-1,columnCount()-1));
            //emit sigList();
            qDebug()<<"list finished";
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
        qDebug()<<"Start list";
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
