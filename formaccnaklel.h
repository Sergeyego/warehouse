#ifndef FORMACCNAKLEL_H
#define FORMACCNAKLEL_H

#include <QWidget>
#include "models.h"
#include "modelro.h"

namespace Ui {
class FormAccNaklEl;
}

class FormAccNaklEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormAccNaklEl(QWidget *parent = 0);
    ~FormAccNaklEl();

private:
    Ui::FormAccNaklEl *ui;
    ModelRo *modelNakl;
    ModelRo *modelNaklCont;
    void loadsettings();
    void savesettings();

private slots:
    void refreshNakl();
    void refreshNaklCont(QModelIndex index);
    void printNakl();
};

#endif // FORMACCNAKLEL_H
