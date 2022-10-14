#ifndef FORMTURNOVERS_H
#define FORMTURNOVERS_H

#include <QWidget>
#include "db/tablemodel.h"
#include "models.h"

namespace Ui {
class FormTurnovers;
}

class TurnoversModel : public TableModel
{
    Q_OBJECT

public:
    TurnoversModel(QObject *parent = nullptr);
    void refresh(QDate beg, QDate end);
public slots:
    void setElEn(bool b);
    void setWireEn(bool b);
    void recalc();

private:
    QMultiHash<QString,accInfo> acc;
    QMultiHash<QString,accInfo> ship;
    QMultiHash<QString,turnInfo> bal;

    void getAcc(QString kis, double &ind, double &ret, double &oth);
    void getShip(QString kis, double &shp, double &ret, double &oth);
    void getBal(QString kis, double &beg, double &end);
    bool el_en;
    bool wire_en;
};

class FormTurnovers : public QWidget
{
    Q_OBJECT

public:
    explicit FormTurnovers(QWidget *parent = 0);
    ~FormTurnovers();

private:
    Ui::FormTurnovers *ui;
    TurnoversModel *turnoversModel;
private slots:
    void refresh();
    void save();
};

#endif // FORMTURNOVERS_H
