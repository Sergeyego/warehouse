#ifndef DIALOGCMD_H
#define DIALOGCMD_H

#include <QDialog>
#include "tprinter.h"

namespace Ui {
class DialogCmd;
}

class DialogCmd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCmd(QString &cmd, QUrl printer, QWidget *parent = 0);
    ~DialogCmd();
    void setPrintCmd(QString cmd);
    void setImageCmd(const QByteArray &cmd);

private:
    Ui::DialogCmd *ui;
    QUrl printerUrl;
    QByteArray imageCmd;

private slots:
    void goCmd();
};

#endif // DIALOGCMD_H
