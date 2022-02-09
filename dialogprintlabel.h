#ifndef DIALOGPRINTLABEL_H
#define DIALOGPRINTLABEL_H

#include <QDialog>
#include "labelbase.h"
#include "modelprint.h"
#include "tprinter.h"

namespace Ui {
class DialogPrintLabel;
}

class LabelBase;

class DialogPrintLabel : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrintLabel(LabelBase *l, QWidget *parent = 0);
    ~DialogPrintLabel();

private:
    Ui::DialogPrintLabel *ui;
    LabelBase *label;
    ModelPrint *modelPrint;
    QUrl currentUrl();
    int currentDpi();

private slots:
    void print();
};

#endif // DIALOGPRINTLABEL_H
