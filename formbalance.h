#ifndef FORMBALANCE_H
#define FORMBALANCE_H

#include <QWidget>
#include "models.h"
#include "db/tablemodel.h"
#include <QSortFilterProxyModel>
#include "dialogwebview.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "progressreportdialog.h"

namespace Ui {
class FormBalance;
}

struct celSum
{
    double kvo;
    double prich;
    double rasch;
};

class FormBalance : public QWidget
{
    Q_OBJECT

public:
    explicit FormBalance(QWidget *parent = 0);
    ~FormBalance();

private:
    Ui::FormBalance *ui;
    QNetworkAccessManager *manager;
    TableModel *modelPart;
    QSortFilterProxyModel *proxyModelPart;
    TableModel *modelMark;
    QSortFilterProxyModel *proxyModelMark;
    QJsonDocument respDoc;
    ProgressReportDialog *progressDialog;
    void loadSettings();
    void saveSettings();
private slots:
    void startUpd();
    void upd();
    void setByPart();
    void save();
    void updPart(QModelIndex index);
    void createPackList();
    void createModelData();
};

#endif // FORMBALANCE_H
