#ifndef FORMDATAEL_H
#define FORMDATAEL_H

#include <QWidget>
#include "modelro.h"
#include <QDataWidgetMapper>
#include <QSettings>
#include <math.h>

namespace Ui {
class FormDataEl;
}

class FormDataEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormDataEl(QWidget *parent = 0);
    ~FormDataEl();
    QString marka();
    QString diametr();
    QString part();
    QString datePart();
    QString datePack();
    QString packer();
    QString packEd();
    QString packGr();
    QString masEd();
    QString masGr();
    QString masPal();
    QString kvoPackPal();
    QString eanEd();
    QString eanGr();
    QString barCode();
    QString barCodePack();

private:
    Ui::FormDataEl *ui;
    ModelRo *modelPart;
    QDataWidgetMapper *mapper;
    QSqlQueryModel *modelPacker;
    bool selectPart();
    void loadSettings();
    void saveSettings();
    QVariant currentData(int row);

private slots:
    void refreshData(QModelIndex index);
    void genEan();
    void setKvoPack();

public slots:
    void updPart();
    void updPacker();
};

#endif // FORMDATAEL_H
