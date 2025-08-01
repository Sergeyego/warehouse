#ifndef FORMREPORTEL_H
#define FORMREPORTEL_H

#include <QWidget>
#include "progressexecutor.h"
#include "db/tablemodel.h"

namespace Ui {
class FormReportEl;
}

class ModelRepEl : public TableModel
{
    Q_OBJECT

public:
    explicit ModelRepEl(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
};

class FormReportEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportEl(QWidget *parent = 0);
    ~FormReportEl();

private:
    Ui::FormReportEl *ui;
    ProgressExecutor *sqlExecutor;
    ModelRepEl *modelReport;

private slots:
    void startUpd();
    void upd();
    void save();
};

#endif // FORMREPORTEL_H
