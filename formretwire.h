#ifndef FORMRETWIRE_H
#define FORMRETWIRE_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"

namespace Ui {
class FormRetWire;
}

class ModelNaklRetWire : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelNaklRetWire(QObject *parent = 0);
    void refresh(int id_type, QDate begDate, QDate endDate);
    bool insertRow(int row, const QModelIndex &parent=QModelIndex());

private:
    int type;
};

class FormRetWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormRetWire(QWidget *parent = 0);
    ~FormRetWire();

private:
    Ui::FormRetWire *ui;
    ModelNaklRetWire *modelNakl;
    DbMapper *mapper;

private slots:
    void upd();
};

#endif // FORMRETWIRE_H
