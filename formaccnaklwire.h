#ifndef FORMACCNAKLWIRE_H
#define FORMACCNAKLWIRE_H

#include <QWidget>

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
};

#endif // FORMACCNAKLWIRE_H
