#ifndef DIALOGLBLSIZE_H
#define DIALOGLBLSIZE_H

#include <QDialog>

namespace Ui {
class DialogLblSize;
}

class DialogLblSize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLblSize(double w, double h, double g, bool rot=false, QWidget *parent = 0);
    ~DialogLblSize();
    double getWidth();
    double getHeight();
    double getGap();
    bool getRotate();

private:
    Ui::DialogLblSize *ui;
};

#endif // DIALOGLBLSIZE_H
