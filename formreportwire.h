#ifndef FORMREPORTWIRE_H
#define FORMREPORTWIRE_H

#include <QWidget>
#include "db/executor.h"
#include "db/tablemodel.h"

namespace Ui {
class FormReportWire;
}

class FormReportWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportWire(QWidget *parent = 0);
    ~FormReportWire();

private:
    Ui::FormReportWire *ui;
    Executor *sqlExecutor;
    TableModel *modelReport;

private slots:
    void startUpd();
    void upd();
    void save();
};

#endif // FORMREPORTWIRE_H
