#ifndef DIALOGPRINTLABEL_H
#define DIALOGPRINTLABEL_H

#include <QDialog>
#include "labelbase.h"
#include "modelprint.h"
#include "tprinter.h"
#include "dialogcmd.h"

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
    void setPrintCmdMode(bool b);
    void setCut(bool b);

private:
    Ui::DialogPrintLabel *ui;
    LabelBase *label;
    ModelPrint *modelPrint;
    bool printCmdMode;
    QUrl currentUrl();
    int currentDpi();
    QString currentCmd();

private slots:
    void print();
    void savePrinter();
    void changedPrinter();
    void calibrate();
    void showCmd();
};

#endif // DIALOGPRINTLABEL_H
