#ifndef DIALOGPRINTPACKLIST_H
#define DIALOGPRINTPACKLIST_H

#include <QDialog>
#include "formdatael.h"
#include <QTextDocument>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include "qr/qrencode.h"

namespace Ui {
class DialogPrintPackList;
}

class DialogPrintPackList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrintPackList(FormDataEl *d, QWidget *parent = 0);
    ~DialogPrintPackList();

private:
    Ui::DialogPrintPackList *ui;
    FormDataEl *data;
    QTextDocument *doc;
    void createDoc();
    bool createQr(QImage &image, QString s);

private slots:
    void drawDoc(QPainter *painter);
    void print();

};

#endif // DIALOGPRINTPACKLIST_H
