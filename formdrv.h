#ifndef FORMDRV_H
#define FORMDRV_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"

namespace Ui {
class FormDrv;
}

class FormDrv : public QWidget
{
    Q_OBJECT

public:
    explicit FormDrv(QWidget *parent = 0);
    ~FormDrv();

private:
    Ui::FormDrv *ui;
    DbTableModel *modelDrv;
    DbMapper *mapper;
};

#endif // FORMDRV_H
