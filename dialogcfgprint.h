#ifndef DIALOGCFGPRINT_H
#define DIALOGCFGPRINT_H

#include <QDialog>
#include "modelprint.h"

namespace Ui {
class DialogCfgPrint;
}

class DialogCfgPrint : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCfgPrint(QWidget *parent = 0);
    ~DialogCfgPrint();

private:
    Ui::DialogCfgPrint *ui;
    ModelPrint *modelPrint;
    QAction *actionAddIpp;
    QAction *actionAddSocket;

private slots:
    void removePrint(int row);
    void addPrint();
};

#endif // DIALOGCFGPRINT_H
