#ifndef FORMDATAEL_H
#define FORMDATAEL_H

#include <QWidget>
#include "modelro.h"
#include <QDataWidgetMapper>

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
    QString packEd();
    QString packGr();
    QString masEd();
    QString masGr();
    QString eanEd();
    QString eanGr();
    QString qrCode();

private:
    Ui::FormDataEl *ui;
    ModelRo *modelPart;
    QDataWidgetMapper *mapper;
    bool selectPart();

private slots:
    void refreshData(QModelIndex index);
    void genEan();

public slots:
    void updPart();
};

#endif // FORMDATAEL_H
