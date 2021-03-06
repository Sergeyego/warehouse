#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include "formacceptancewire.h"
#include "formacceptanceel.h"
#include "formship.h"
#include "formpoluch.h"
#include "formdrv.h"
#include "formbalance.h"
#include "formturnovers.h"
#include "olap/cubewidget.h"
#include "formaccnaklel.h"
#include "formaccnaklwire.h"
#include "dialogcfgprint.h"
#include "formlabelsel.h"
#include "formlabelswire.h"
#include "formcells.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
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
    void newFormShip();
    void newFormPoluch();
    void newFormDrv();
    void newFormBalance();
    void newFormTurnovers();
    void newAnShipEl();
    void newAnShipWire();
    void newAnAccEl();
    void newAnAccWire();
    void newFormAccNaklEl();
    void newFormAccNaklWire();
    void newFormLabelsEl();
    void newFormLabelsWire();
    void newFormCells();
    void cfgPrint();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H
