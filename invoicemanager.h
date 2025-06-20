#ifndef INVOICEMANAGER_H
#define INVOICEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QSqlDatabase>
#include <QRegularExpression>

class InvoiceManager : public QObject
{
    Q_OBJECT
public:
    explicit InvoiceManager(QObject *parent = nullptr);
    void getInvoice(QString path, QString vid, QString type, QString filename, int year);
private:
    QNetworkAccessManager *manager;

private slots:
    void onResult(QNetworkReply *reply);

signals:

public slots:
};

#endif // INVOICEMANAGER_H
