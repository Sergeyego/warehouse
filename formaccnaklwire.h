#ifndef FORMACCNAKLWIRE_H
#define FORMACCNAKLWIRE_H

#include <QWidget>
#include "models.h"
#include "modelro.h"

namespace Ui {
class FormAccNaklWire;
}

class FormAccNaklWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormAccNaklWire(QWidget *parent = 0);
    ~FormAccNaklWire();

private:
    Ui::FormAccNaklWire *ui;
    ModelRo *modelNakl;
    ModelRo *modelNaklCont;
    void loadsettings();
    void savesettings();

private slots:
    void refreshNakl();
    void refreshNaklCont(QModelIndex index);
    void printNakl();
};

#endif // FORMACCNAKLWIRE_H
