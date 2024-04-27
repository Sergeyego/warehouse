#ifndef DIALOGWEBVIEW_H
#define DIALOGWEBVIEW_H

#include <QDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBlock>
#include "httpsyncmanager.h"

namespace Ui {
class DialogWebView;
}

class DialogWebView : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWebView(QWidget *parent = nullptr);
    ~DialogWebView();
    void sendGetReq(QString path);
    void setSingle(bool s);
    void loadDoc(const QString &html);

private:
    Ui::DialogWebView *ui;
    bool single;

private slots:
    void print();
    void drawDoc(QPainter *painter);
};

#endif // DIALOGWEBVIEW_H
