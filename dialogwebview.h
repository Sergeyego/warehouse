#ifndef DIALOGWEBVIEW_H
#define DIALOGWEBVIEW_H

#include <QDialog>
#include <QPrinter>
#include <QPrintDialog>
#include "printhandler.h"

namespace Ui {
class DialogWebView;
}

class DialogWebView : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWebView(QWidget *parent = nullptr);
    ~DialogWebView();
    void setUrl(QString url);

private:
    Ui::DialogWebView *ui;

private slots:
    void print();
};

#endif // DIALOGWEBVIEW_H
