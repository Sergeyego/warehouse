#ifndef FORMCELLS_H
#define FORMCELLS_H

#include <QWidget>

namespace Ui {
class FormCells;
}

class FormCells : public QWidget
{
    Q_OBJECT

public:
    explicit FormCells(QWidget *parent = 0);
    ~FormCells();

private:
    Ui::FormCells *ui;
};

#endif // FORMCELLS_H
