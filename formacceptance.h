#ifndef FORMACCEPTANCE_H
#define FORMACCEPTANCE_H

#include <QWidget>
#include "sync1c.h"

namespace Ui {
class FormAcceptance;
}

class FormAcceptance : public QWidget
{
    Q_OBJECT

public:
    explicit FormAcceptance(QWidget *parent = 0);
    ~FormAcceptance();

private:
    Ui::FormAcceptance *ui;
    Sync1C *sync1C;
};

#endif // FORMACCEPTANCE_H
