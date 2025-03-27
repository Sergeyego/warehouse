#ifndef FORMLABELSWIRE_H
#define FORMLABELSWIRE_H

#include <QWidget>
#include "formdatawire.h"
#include "labelswire.h"
#include "dialogwebview.h"

namespace Ui {
class FormLabelsWire;
}

class FormLabelsWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormLabelsWire(QWidget *parent = 0);
    ~FormLabelsWire();

private:
    Ui::FormLabelsWire *ui;
    FormDataWire *data;
    LabelE801016 *labelE801016;
    LabelG95110 *labelG95110;
    LabelE4570 *labelE4570;
    LabelG100100 *labelG100100;
    LabelG100100Pal *labelG100100Pal;
    LabelE7035 *labelE7035;

private slots:
    void printPackListA5();
};

#endif // FORMLABELSWIRE_H
