#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include "formacceptancewire.h"
#include "formacceptanceel.h"
#include "formretwire.h"
#include "formretel.h"
#include "formship.h"
#include "formshipcons.h"
#include "formpoluch.h"
#include "formdrv.h"
#include "formbalance.h"
#include "formreportel.h"
#include "formreportwire.h"
#include "olap/cubewidget.h"
#include "formaccnaklel.h"
#include "formaccnaklwire.h"
#include "dialogcfgprint.h"
#include "formlabelsel.h"
#include "formlabelswire.h"
#include "formcells.h"
#include "formbalanceew.h"
#include "formmovel.h"
#include "formmovwire.h"
#include "formrequests.h"
#include "formreqreport.h"
#include "formpack.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool master = false, QWidget *parent = 0);
    ~MainWindow();
    bool exist(QObject *a);
    void actAction(QAction *a, void (MainWindow::*sl)());
    void addSubWindow(QWidget *w, QObject *a);
    bool setActiveSubWindow(QString t);
    QMap <QString,QAction*> actions;

private slots:
    void closeTab(int index);
    void newFormAcceptanceWire();
    void newFormAcceptanceEl();
    void newFormRetEl();
    void newFormRetWire();
    void newFormShip();
    void newFormShipCons();
    void newFormPoluch();
    void newFormDrv();
    void newFormBalance();
    void newFormBalanceEl();
    void newFormBalanceWire();
    void newFormReportEl();
    void newFormReportWire();
    void newAnShipEl();
    void newAnShipWire();
    void newAnAccEl();
    void newAnAccWire();
    void newAnReq();
    void newFormAccNaklEl();
    void newFormAccNaklWire();
    void newFormLabelsEl();
    void newFormLabelsWire();
    void newFormCells();
    void newFormMovEl();
    void newFormMovWire();
    void newFormRequests();
    void newFormReqReport();
    void newFormPack();
    void cfgPrint();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H
