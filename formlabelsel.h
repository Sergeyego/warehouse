#ifndef FORMLABELSEL_H
#define FORMLABELSEL_H

#include <QWidget>
#include "formdatael.h"
#include "labelsel.h"
#include <dialogprintpacklist.h>

namespace Ui {
class FormLabelsEl;
}

class FormLabelsEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormLabelsEl(QWidget *parent = 0);
    ~FormLabelsEl();

private:
    Ui::FormLabelsEl *ui;
    FormDataEl *data;
    LabelE5040 *labelE5040;
    LabelG5040 *labelG5040;
    LabelE60150 *labelE60150;
    LabelG60150 *labelG60150;

private slots:
    void printPackList();
    void printE60150();
    void printG60150();
};

#endif // FORMLABELSEL_H
