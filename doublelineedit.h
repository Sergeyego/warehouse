#ifndef DOUBLELINEEDIT_H
#define DOUBLELINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QDoubleValidator>

class DoubleLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DoubleLineEdit(QWidget *parent);
    void keyPressEvent(QKeyEvent *e);
    void setRange(double min, double max, int decimal=1);
private:
    QDoubleValidator *dval;
};

#endif // DOUBLELINEEDIT_H
