#ifndef DIALOGPRINTPACKLIST_H
#define DIALOGPRINTPACKLIST_H

#include <QDialog>
#include "formdatael.h"
#include "formdatawire.h"
#include <QTextDocument>
#include <QTextTable>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include "qr/qrencode.h"
#include "code128/code128.h"
#include "sync1c.h"
#include "models.h"

namespace Ui {
class DialogPrintPackList;
}

class QRImg{
public:
    static bool createQr(QImage &image, QString str);
};

class Code128Img{
public:
    static bool createCode128(QImage &image, QString str);
};

class DialogPrintPackList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrintPackList(QTextDocument *d, QWidget *parent = 0);
    ~DialogPrintPackList();
    void setSingle(bool b);


private:
    Ui::DialogPrintPackList *ui;
    QTextDocument *doc;
    bool single;

private slots:
    void drawDoc(QPainter *painter);
    void print();

};

class PackElDoc : public QTextDocument
{
public:
    PackElDoc (FormDataEl *data, QObject *parent=0);
    PackElDoc (int id_part, double kvo, QString cont, QObject *parent=0);
private:
    struct packInfo {
        QString cont;
        QString marka;
        QString diam;
        QString part;
        QString datePart;
        QString kvoM;
        QString kvo;
        QString datePack;
        QString packer;
        QString barcode;
        QString eanGr;
    };

    void createDoc(packInfo &inf);
};

class PackWireDoc : public QTextDocument
{
public:
    PackWireDoc (FormDataWire *data, QObject *parent=0);
    PackWireDoc (int id_part, double kvo, QString cont, QObject *parent=0);

private:
    struct packInfo {
        QString cont;
        QString marka;
        QString diam;
        QString plavka;
        QString part;
        QString spool;
        QString kvoM;
        QString kvo;
        QString datePack;
        QString packer;
        QString barcode;
        QString eanGr;
    };

    void createDoc(packInfo &inf);
};

class PackNaklDoc : public QTextDocument
{
public:
    PackNaklDoc (QString kis, QObject *parent=0);
};
#endif // DIALOGPRINTPACKLIST_H
