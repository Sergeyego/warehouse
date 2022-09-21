#ifndef FORMRETEL_H
#define FORMRETEL_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormRetEl;
}

class ModelNaklRetEl : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelNaklRetEl(QObject *parent = 0);
    void refresh(int id_type, QDate begDate, QDate endDate);
    bool insertRow(int row, const QModelIndex &parent=QModelIndex());

private:
    int type;
};

class ModelNaklRetElData : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelNaklRetElData(QObject *parent = 0);
    void refresh(int id_nakl);
    bool submit();
    void revert();
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    double getStock(QModelIndex index);
signals:
    void sigStock(QString mes);
};

class FormRetEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormRetEl(QWidget *parent = 0);
    ~FormRetEl();

private:
    Ui::FormRetEl *ui;
    ModelNaklRetEl *modelNakl;
    ModelNaklRetElData *modelNaklData;
    DbMapper *mapper;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void updData(int index);
    void setCurrentFilter(int num);
    void printNakl();
};

#endif // FORMRETEL_H
