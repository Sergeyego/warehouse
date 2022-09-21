#ifndef DIALOGSHIPLOAD_H
#define DIALOGSHIPLOAD_H

#include <QDialog>
#include "modelro.h"

namespace Ui {
class DialogShipLoad;
}

class DialogShipLoad : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShipLoad(int id_ship, QWidget *parent = 0);
    ~DialogShipLoad();
public slots:
    void accept();

private:
    Ui::DialogShipLoad *ui;
    int id_s;
    ModelRo *modelShip;
    ModelRo *modelEl;
    ModelRo *modelWire;
    double getStockEl(int id_part);
    double getStockWire(int id_part);
    bool insertEl(int id_part, double kvo);
    bool insertWire(int id_part, double kvo);

private slots:
    void updShipData(QModelIndex index);
};

#endif // DIALOGSHIPLOAD_H
