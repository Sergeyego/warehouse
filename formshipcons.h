#ifndef FORMSHIPCONS_H
#define FORMSHIPCONS_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormShipCons;
}

class ModelShipCons : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipCons(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_pol=-1);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
};

class FormShipCons : public QWidget
{
    Q_OBJECT

public:
    explicit FormShipCons(QWidget *parent = 0);
    ~FormShipCons();

private:
    Ui::FormShipCons *ui;
    void loadsettings();
    void savesettings();
    ModelShipCons *modelShip;
    DbMapper *mapper;

private slots:
    void updShip();
    void updPol();
    void setCurrentShip(int index);
};

#endif // FORMSHIPCONS_H
