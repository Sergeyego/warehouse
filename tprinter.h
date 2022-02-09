#ifndef TPRINTER_H
#define TPRINTER_H

#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TPrinter : public QObject
{
public:
    TPrinter(QObject *parent=0);
    static void printDataTcp(QString host, int port, const QByteArray &data);
    static void printDataIpp(QUrl url, const QByteArray &data);
    static void printData(QUrl url, const QByteArray &data);
};

#endif // TPRINTER_H
