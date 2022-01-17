#ifndef FORMACCEPTANCE_H
#define FORMACCEPTANCE_H

#include <QWidget>
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
    ModelAcceptance(QString table, QObject *parent = 0);
    void refresh(QDate beg, QDate end);
    bool insertRow(int row, const QModelIndex &parent);
};

class ModelAcceptanceData: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptanceData(QString table, DbRelation *relPart, QObject *parent = 0);
    void refresh(int id_acc);
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
    ModelAcceptance *modelAcceeptance;
    ModelAcceptance *modelAcceptanceWire;
    ModelAcceptanceData *modelAcceeptanceData;
    ModelAcceptanceData *modelAcceeptanceDataWire;
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
