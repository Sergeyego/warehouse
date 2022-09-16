#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    actAction(ui->actionLblEl,&MainWindow::newFormLabelsEl);
    actAction(ui->actionLblWire,&MainWindow::newFormLabelsWire);
    actAction(ui->actionAcceptanceWire,&MainWindow::newFormAcceptanceWire);
    actAction(ui->actionAcceptanceEl,&MainWindow::newFormAcceptanceEl);
    actAction(ui->actionRetEl,&MainWindow::newFormRetEl);
    actAction(ui->actionRetWire,&MainWindow::newFormRetWire);
    actAction(ui->actionShip,&MainWindow::newFormShip);
    actAction(ui->actionPoluch,&MainWindow::newFormPoluch);
    actAction(ui->actionDrv,&MainWindow::newFormDrv);
    actAction(ui->actionBalance,&MainWindow::newFormBalance);
    actAction(ui->actionBalanceEl,&MainWindow::newFormBalanceEl);
    actAction(ui->actionBalanceWire,&MainWindow::newFormBalanceWire);
    actAction(ui->actionTurnovers,&MainWindow::newFormTurnovers);
    actAction(ui->actionReportEl,&MainWindow::newFormReportEl);
    actAction(ui->actionReportWire,&MainWindow::newFormReportWire);
    actAction(ui->actionAnShipEl,&MainWindow::newAnShipEl);
    actAction(ui->actionAnShipWire,&MainWindow::newAnShipWire);
    actAction(ui->actionAnAccEl,&MainWindow::newAnAccEl);
    actAction(ui->actionAnAccWire,&MainWindow::newAnAccWire);
    actAction(ui->actionAccNaklEl,&MainWindow::newFormAccNaklEl);
    actAction(ui->actionAccNaklWire,&MainWindow::newFormAccNaklWire);
    actAction(ui->actionCells,&MainWindow::newFormCells);
    actAction(ui->actionMovEl,&MainWindow::newFormMovEl);

    connect(ui->actionCfgPrint,SIGNAL(triggered(bool)),this,SLOT(cfgPrint()));
    connect(ui->actionAllRefresh,SIGNAL(triggered(bool)),Models::instance(),SLOT(refresh()));

    loadSettings();
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

bool MainWindow::exist(QObject *a)
{
    bool b=false;
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        b=setActiveSubWindow(action->text());
    }
    return b;
}

void MainWindow::actAction(QAction *a, void (MainWindow::*sl)())
{
    connect(a, &QAction::triggered, this, sl);
    actions.insert(a->text(),a);
}

void MainWindow::addSubWindow(QWidget *w, QObject *a)
{
    w->setAttribute(Qt::WA_DeleteOnClose);
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        w->setWindowTitle(action->text());
    }
    ui->tabWidget->addTab(w,w->windowTitle());
    ui->tabWidget->setCurrentWidget(w);
}

bool MainWindow::setActiveSubWindow(QString t)
{
    bool b=false;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (ui->tabWidget->tabText(i)==t){
            ui->tabWidget->setCurrentIndex(i);
            b=true;
            break;
        }
    }
    return b;
}

void MainWindow::closeTab(int index)
{
    ui->tabWidget->widget(index)->close();
}

void MainWindow::newFormAcceptanceWire()
{
    if (!exist(sender())){
        addSubWindow(new FormAcceptanceWire(),sender());
    }
}

void MainWindow::newFormAcceptanceEl()
{
    if (!exist(sender())){
        addSubWindow(new FormAcceptanceEl(),sender());
    }
}

void MainWindow::newFormRetEl()
{
    if (!exist(sender())){
        addSubWindow(new FormRetEl(),sender());
    }
}

void MainWindow::newFormRetWire()
{
    if (!exist(sender())){
        addSubWindow(new FormRetWire(),sender());
    }
}

void MainWindow::newFormShip()
{
    if (!exist(sender())){
        addSubWindow(new FormShip(false),sender());
    }
}

void MainWindow::newFormPoluch()
{
    if (!exist(sender())){
        addSubWindow(new FormPoluch(),sender());
    }
}

void MainWindow::newFormDrv()
{
    if (!exist(sender())){
        addSubWindow(new FormDrv(),sender());
    }
}

void MainWindow::newFormBalance()
{
    if (!exist(sender())){
        addSubWindow(new FormBalance(),sender());
    }
}

void MainWindow::newFormBalanceEl()
{
    if (!exist(sender())){
        addSubWindow(new FormBalanceEW(true,false),sender());
    }
}

void MainWindow::newFormBalanceWire()
{
    if (!exist(sender())){
        addSubWindow(new FormBalanceEW(false,true),sender());
    }
}

void MainWindow::newFormTurnovers()
{
    if (!exist(sender())){
        addSubWindow(new FormTurnovers(),sender());
    }
}

void MainWindow::newFormReportEl()
{
    if (!exist(sender())){
        addSubWindow(new FormReportEl(),sender());
    }
}

void MainWindow::newFormReportWire()
{
    if (!exist(sender())){
        addSubWindow(new FormReportWire(),sender());
    }
}

void MainWindow::newAnShipEl()
{
    if (!exist(sender())){
        addSubWindow(new CubeWidget(23),sender());
    }
}

void MainWindow::newAnShipWire()
{
    if (!exist(sender())){
        addSubWindow(new CubeWidget(22),sender());
    }
}

void MainWindow::newAnAccEl()
{
    if (!exist(sender())){
        addSubWindow(new CubeWidget(19),sender());
    }
}

void MainWindow::newAnAccWire()
{
    if (!exist(sender())){
        addSubWindow(new CubeWidget(21),sender());
    }
}

void MainWindow::newFormAccNaklEl()
{
    if (!exist(sender())){
        addSubWindow(new FormAccNaklEl(),sender());
    }
}

void MainWindow::newFormAccNaklWire()
{
    if (!exist(sender())){
        addSubWindow(new FormAccNaklWire(),sender());
    }
}

void MainWindow::newFormLabelsEl()
{
    if (!exist(sender())){
        addSubWindow(new FormLabelsEl(),sender());
    }
}

void MainWindow::newFormLabelsWire()
{
    if (!exist(sender())){
        addSubWindow(new FormLabelsWire(),sender());
    }
}

void MainWindow::newFormCells()
{
    if (!exist(sender())){
        addSubWindow(new FormCells(),sender());
    }
}

void MainWindow::newFormMovEl()
{
    if (!exist(sender())){
        addSubWindow(new FormMovEl(),sender());
    }
}

void MainWindow::cfgPrint()
{
    DialogCfgPrint d;
    d.exec();
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
    QString opentab=settings.value("main_opentab").toString();
    QString current=settings.value("main_currenttab").toString();

    if (!opentab.isEmpty()){
        QStringList l=opentab.split("|");
        foreach (QString a, l) {
            if (actions.contains(a)){
                actions.value(a)->trigger();
            }
        }
    }
    setActiveSubWindow(current);
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
    QString opentab, currenttab;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (!opentab.isEmpty()){
            opentab+="|";
        }
        opentab+=ui->tabWidget->tabText(i);
    }
    currenttab=ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    settings.setValue("main_opentab", opentab);
    settings.setValue("main_currenttab",currenttab);
}
