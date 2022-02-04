#ifndef FORMACCEPTANCEEL_H
#define FORMACCEPTANCEEL_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"

namespace Ui {
class FormAcceptanceEl;
}

class ModelAcceptanceEl : public DbTableModel{
    Q_OBJECT

public:
    ModelAcceptanceEl(QWidget *parent = 0);
    void refresh(QDate beg, QDate end);
    bool insertRow(int row, const QModelIndex &parent);
};

class ModelAcceptanceElData: public DbTableModel
{
    Q_OBJECT
public:
    ModelAcceptanceElData(QObject *parent = 0);
    void refresh(int id_acc);
    bool insertRow(int row, const QModelIndex &parent);

private slots:
    void caclSum();

signals:
    void sigSum(QString s);
};

class FormAcceptanceEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormAcceptanceEl(QWidget *parent = 0);
    ~FormAcceptanceEl();

private:
    Ui::FormAcceptanceEl *ui;
    ModelAcceptanceEl *modelAcceptanceEl;
    ModelAcceptanceElData *modelAcceptanceElData;
    DbMapper *mapper;
    void loadsettings();
    void savesettings();

private slots:
    void updAcc();
    void updAccData(int index);
    void setPartFilter();
    void sync();
};

#endif // FORMACCEPTANCEEL_H
