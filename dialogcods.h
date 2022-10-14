#ifndef DIALOGCODS_H
#define DIALOGCODS_H

#include <QDialog>
#include "db/dbtablemodel.h"
#include "models.h"

namespace Ui {
class DialogCods;
}

class DialogCods : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCods(QWidget *parent = 0);
    ~DialogCods();

private:
    Ui::DialogCods *ui;
    DbTableModel *modelElCods;
    DbTableModel *modelWireCods;
    DbSqlRelation *relDiam;
    DbSqlRelation *relWire;
    DbSqlRelation *relSpool;
    DbSqlRelation *relPack;
    DbSqlRelation *relElPack;
    DbSqlRelation *relElVar;
};

#endif // DIALOGCODS_H
