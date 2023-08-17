#include "tprinter.h"

TPrinter::TPrinter(QObject *parent) : QObject(parent)
{

}

void TPrinter::printDataTcp(QString host, int port, const QByteArray &data)
{
    QTcpSocket tcpSocket;
    tcpSocket.connectToHost(host,port);
    bool ok=tcpSocket.waitForConnected();
    if (ok){
        qint64 x = 0;
        const qint64 size=data.size();
        int bsize=2048;
        while (x < size) {
            int b= ((size-x)< bsize) ? (size-x) : bsize;
            qint64 y = tcpSocket.write(data.right(size-x),b);
            tcpSocket.waitForBytesWritten();
            //qDebug()<<wrok<<QString::fromUtf8(" Отправлено %1 байт").arg(y);
            x += y;
        }
        tcpSocket.disconnectFromHost();
    } else {
        QMessageBox::critical(nullptr,QString::fromUtf8("Ошибка"),tcpSocket.errorString(),QMessageBox::Ok);
    }
}

void TPrinter::printDataIpp(QUrl url, const QByteArray &data)
{
    QByteArray d;

    d.append(char(0x02));
    d.append(char(0x00));

    d.append(char(0x00));
    d.append(char(0x02));

    d.append(char(0x00));
    d.append(char(0x00));
    d.append(char(0x00));
    d.append(char(0x01));

    d.append(char(0x01));
    d.append(char(0x47));
    d.append(char(0x00));
    d.append(char(0x12));
    d.append(QString("attributes-charset").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(0x05));
    d.append(QString("utf-8").toLocal8Bit());
    d.append(char(0x48));
    d.append(char(0x00));
    d.append(char(0x1B));
    d.append(QString("attributes-natural-language").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(0x05));
    d.append(QString("en-us").toLocal8Bit());
    d.append(char(0x45));
    d.append(char(0x00));
    d.append(char(0x0B));
    d.append(QString("printer-uri").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(url.toString().length()));
    d.append(url.toString().toLocal8Bit());
    d.append(char(0x42));
    d.append(char(0x00));
    d.append(char(0x08));
    d.append(QString("job-name").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(0x06));
    d.append(QString("testlb").toLocal8Bit());
    d.append(char(0x22));
    d.append(char(0x00));
    d.append(char(0x16));
    d.append(QString("ipp-attribute-fidelity").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(0x01));
    d.append(char(0x02));
    d.append(char(0x21));
    d.append(char(0x00));
    d.append(char(0x06));
    d.append(QString("copies").toLocal8Bit());
    d.append(char(0x00));
    d.append(char(0x04));
    d.append(char(0x00));
    d.append(char(0x00));
    d.append(char(0x00));
    d.append(char(0x01));

    d.append(char(0x03));

    d.append(data);

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/ipp");
    request.setRawHeader("User-Agent", "Appszsm");

    QEventLoop loop;
    QNetworkAccessManager man;
    connect(&man,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
    QNetworkReply *reply;

    reply=man.post(request,d);

    if (!reply->isFinished()){
        loop.exec();
    }

    bool ok=(reply->error()==QNetworkReply::NoError);
    if (!ok){
        //qDebug()<<"fail!"<<" "<<reply->readAll();
        QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString(),QMessageBox::Ok);
    }
    reply->deleteLater();
}

void TPrinter::printData(QUrl url, const QByteArray &data)
{
    QString scheme=url.scheme();
    if (!url.host().isEmpty() && url.isValid()){
        if (scheme == "http" || scheme == "ipp"){
            url.setScheme("http");
            printDataIpp(url,data);
        } else if (scheme == "socket"){
            printDataTcp(url.host(),url.port(9100),data);
        } else {
            QMessageBox::critical(nullptr,tr("Ошибка"),tr("Неподдерживаемый протокол: ")+scheme,QMessageBox::Ok);
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),tr("Неправильный url адрес принтера"),QMessageBox::Ok);
    }
}
