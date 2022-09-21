#ifndef FORMRETWIRE_H
#define FORMRETWIRE_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

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

class ModelNaklRetWireData : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelNaklRetWireData(QObject *parent = 0);
    void refresh(int id_nakl);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool submit();
    void revert();
private:
    double getStock(QModelIndex index);
signals:
    void sigStock(QString mes);
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
    ModelNaklRetWireData *modelNaklData;
    DbMapper *mapper;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void updData(int index);
    void setCurrentFilter(int num);
    void printNakl();
};

#endif // FORMRETWIRE_H
