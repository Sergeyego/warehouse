#ifndef FORMREQREPORT_H
#define FORMREQREPORT_H

#include <QWidget>
#include <QListWidgetItem>
#include "progressexecutor.h"
#include "db/tablemodel.h"

namespace Ui {
class FormReqReport;
}

class FormReqReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormReqReport(QWidget *parent = nullptr);
    ~FormReqReport();

private:
    Ui::FormReqReport *ui;
    ProgressExecutor *executor;
    TableModel *modelReport;
    void addColumn(QString title, QString colName, bool checked=false);
    QStringList currentTitle;

private slots:
    void upd();
    void execFinished();
    void colUp();
    void colDown();
    void saveXlsx();
};

#endif // FORMREQREPORT_H
