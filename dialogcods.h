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
    DbRelation *relDiam;
    DbRelation *relWire;
    DbRelation *relSpool;
    DbRelation *relPack;
    DbRelation *relElPack;
};

#endif // DIALOGCODS_H
