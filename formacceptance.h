#ifndef FORMACCEPTANCE_H
#define FORMACCEPTANCE_H

#include <QWidget>
#include "sync1c.h"
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormAcceptance;
}

class ModelAcceptance: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptance(QObject *parent = 0);
    void refresh(QDate beg, QDate end);
    bool insertRow(int row, const QModelIndex &parent);
};

class ModelAcceptanceWire: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptanceWire(QObject *parent = 0);
    void refresh(QDate beg, QDate end);
    bool insertRow(int row, const QModelIndex &parent);
};

class FormAcceptance : public QWidget
{
    Q_OBJECT

public:
    explicit FormAcceptance(QWidget *parent = 0);
    ~FormAcceptance();

private:
    Ui::FormAcceptance *ui;
    Sync1C *sync1C;
    ModelAcceptance *modelAcceeptance;
    ModelAcceptanceWire *modelAcceptanceWire;
    DbTableModel *modelAcceeptanceData;
    DbTableModel *modelAcceeptanceDataWire;
    DbMapper *mapper;
    DbMapper *mapperWire;

private slots:
    void updAcc();
    void updAccData(int index);
    void updAccDataWire(int index);
    void sync();
    void setCurrentWidget();
    void setPartFilter();
};

#endif // FORMACCEPTANCE_H
