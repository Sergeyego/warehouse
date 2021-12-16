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

class FormAcceptance : public QWidget
{
    Q_OBJECT

public:
    explicit FormAcceptance(QWidget *parent = 0);
    ~FormAcceptance();

private:
    Ui::FormAcceptance *ui;
    Sync1C *sync1C;
    DbTableModel *modelAcceeptance;
    DbTableModel *modelAcceeptanceData;
    DbMapper *mapper;

private slots:
    void updAcc();
    void updAccData(int index);
    void sync();
};

#endif // FORMACCEPTANCE_H
