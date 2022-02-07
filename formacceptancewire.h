#ifndef FORMACCEPTANCEWIRE_H
#define FORMACCEPTANCEWIRE_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormAcceptanceWire;
}

class ModelAcceptanceWire: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptanceWire(QObject *parent = 0);
    void refresh(QDate beg, QDate end);
    bool insertRow(int row, const QModelIndex &parent);
};

class ModelAcceptanceWireData: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptanceWireData(QObject *parent = 0);
    void refresh(int id_acc);
    bool insertRow(int row, const QModelIndex &parent);

private slots:
    void caclSum();

signals:
    void sigSum(QString s);
};

class FormAcceptanceWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormAcceptanceWire(QWidget *parent = 0);
    ~FormAcceptanceWire();

private:
    Ui::FormAcceptanceWire *ui;
    ModelAcceptanceWire *modelAcceptanceWire;
    ModelAcceptanceWireData *modelAcceptanceWireData;
    DbMapper *mapper;
    void loadsettings();
    void savesettings();

private slots:
    void updAcc();
    void updAccData(int index);
    void sync();
    void setPartFilter();
};

#endif // FORMACCEPTANCEWIRE_H
