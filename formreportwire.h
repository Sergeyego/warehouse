#ifndef FORMREPORTWIRE_H
#define FORMREPORTWIRE_H

#include <QWidget>
#include "progressexecutor.h"
#include "db/tablemodel.h"

namespace Ui {
class FormReportWire;
}

class ModelRepWire : public TableModel
{
    Q_OBJECT

public:
    explicit ModelRepWire(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
};

class FormReportWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportWire(QWidget *parent = 0);
    ~FormReportWire();

private:
    Ui::FormReportWire *ui;
    ProgressExecutor *sqlExecutor;
    ModelRepWire *modelReport;

private slots:
    void startUpd();
    void upd();
    void save();
};

#endif // FORMREPORTWIRE_H
