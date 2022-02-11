#ifndef FORMACCEPTANCEEL_H
#define FORMACCEPTANCEEL_H

#include <QWidget>
#include <QInputDialog>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "labelbase.h"

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

class LabelElPal : public LabelBase
{
public:
    LabelElPal(int id_acc, int cont=-1, QObject *parent=0);
protected:
    virtual QString getCod();
private:
    struct accInfo {
        QString nameNom;
        QString namePart;
        double kvo;
    };
    QMultiMap<QString,accInfo> hash;
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
    QAction *actionPrintLblAll;
    QAction *actionPrintLblOne;
    void loadsettings();
    void savesettings();

private slots:
    void updAcc();
    void updAccData(int index);
    void setPartFilter();
    void sync();
    void printPalAll();
    void printPalOne();
};

#endif // FORMACCEPTANCEEL_H
