#ifndef FORMREPORTEL_H
#define FORMREPORTEL_H

#include <QWidget>
#include "executor.h"
#include "tablemodel.h"

namespace Ui {
class FormReportEl;
}

class FormReportEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormReportEl(QWidget *parent = 0);
    ~FormReportEl();

private:
    Ui::FormReportEl *ui;
    Executor *sqlExecutor;
    TableModel *modelReport;

private slots:
    void startUpd();
    void upd();
    void save();
};

#endif // FORMREPORTEL_H
