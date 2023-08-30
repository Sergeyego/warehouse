#include "invoicemanager.h"

InvoiceManager::InvoiceManager(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onResult(QNetworkReply*)));
}

void InvoiceManager::getInvoice(QString path, QString vid, QString type, QString filename, int year)
{
    QSqlDatabase db=QSqlDatabase::database();
    const QString host=db.isValid()? db.hostName() : "127.0.0.1";
    int port=7000;
    QUrl url("http://"+host+":"+QString::number(port)+"/"+path);
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply *reply = manager->get(request);
    reply->setProperty("invoice_vid",vid);
    reply->setProperty("invoice_type",type);
    reply->setProperty("filename",filename);
    reply->setProperty("year",year);
}

void InvoiceManager::onResult(QNetworkReply *reply)
{
    if (reply->error()){
        QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
    } else {
        QString vid=reply->property("invoice_vid").toString();
        QString type=reply->property("invoice_type").toString();
        QString filename=reply->property("filename").toString();
        QString year=reply->property("year").isNull() ? QString::number(QDate::currentDate().year()) : reply->property("year").toString();
        if (vid.isEmpty()){
            vid=tr("unknown");
        }
        if (type.isEmpty()){
            type=tr("unknown");
        }
        if (filename.isEmpty()){
            filename=tr("temp.docx");
        }

        QDir dir(QDir::homePath()+tr("/Накладные"));
        if (!dir.exists()) dir.mkdir(dir.path());
        dir.setPath(dir.path()+"/"+vid);
        if (!dir.exists()) dir.mkdir(dir.path());
        dir.setPath(dir.path()+"/"+type);
        if (!dir.exists()) dir.mkdir(dir.path());
        dir.setPath(dir.path()+"/"+year);
        if (!dir.exists()) dir.mkdir(dir.path());
        filename=filename.replace(QRegExp("[^\\w]"), "_");
        QString totalName=dir.path()+"/"+filename+".docx";
        QFile file(totalName);
        if (file.open(QIODevice::WriteOnly)){
            file.write(reply->readAll());
            file.close();
            QFileInfo fileInfo(file);
            QDesktopServices::openUrl((QUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()))));
        }
    }
    reply->deleteLater();
}
