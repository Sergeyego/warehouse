#-------------------------------------------------
#
# Project created by QtCreator 2021-12-07T09:38:55
#
#-------------------------------------------------

QT       += core gui sql network xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warehouse
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(xlsx/qtxlsx.pri)

SOURCES += \
    dialogpacker.cpp \
    dialogreqload.cpp \
    dialogwebview.cpp \
    formcert.cpp \
    formpack.cpp \
    formreqreport.cpp \
    formrequests.cpp \
    httpsyncmanager.cpp \
        main.cpp \
        mainwindow.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    qftp/qftp.cpp \
    qftp/qurlinfo.cpp \
    reader.cpp \
    sync1c.cpp \
    models.cpp \
    formship.cpp \
    dialogcods.cpp \
    formpoluch.cpp \
    formbalance.cpp \
    olap/axiswidget.cpp \
    olap/cubewidget.cpp \
    olap/cubic.cpp \
    olap/olapmodel.cpp \
    olap/tableview.cpp \
    olap/dialogolapflt.cpp \
    olap/formfilter.cpp \
    formacceptanceel.cpp \
    formacceptancewire.cpp \
    formaccnaklel.cpp \
    modelro.cpp \
    formaccnaklwire.cpp \
    dialogcfgprint.cpp \
    modelprint.cpp \
    formlabelsel.cpp \
    formdatael.cpp \
    labelbase.cpp \
    dialogprintlabel.cpp \
    labelsel.cpp \
    tprinter.cpp \
    dialogcmd.cpp \
    formlabelswire.cpp \
    formdatawire.cpp \
    labelswire.cpp \
    doublelineedit.cpp \
    formcells.cpp \
    dialoglblsize.cpp \
    formdrv.cpp \
    formbalanceew.cpp \
    progressreportdialog.cpp \
    formreportel.cpp \
    formreportwire.cpp \
    formretwire.cpp \
    formretel.cpp \
    formmovel.cpp \
    dialogrest.cpp \
    formmovwire.cpp \
    formshipcons.cpp \
    dialogshipload.cpp \
    db/dbcombobox.cpp \
    db/dbdateedit.cpp \
    db/dbrelationeditdialog.cpp \
    db/executor.cpp \
    db/tablemodel.cpp \
    progressexecutor.cpp \
    invoicemanager.cpp

HEADERS += \
    dialogpacker.h \
    dialogreqload.h \
    dialogwebview.h \
    formcert.h \
    formpack.h \
    formreqreport.h \
    formrequests.h \
    httpsyncmanager.h \
        mainwindow.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    qftp/qftp.h \
    qftp/qurlinfo.h \
    reader.h \
    sync1c.h \
    models.h \
    formship.h \
    dialogcods.h \
    formpoluch.h \
    formbalance.h \
    olap/axiswidget.h \
    olap/cubewidget.h \
    olap/cubic.h \
    olap/olapmodel.h \
    olap/tableview.h \
    olap/dialogolapflt.h \
    olap/formfilter.h \
    formacceptanceel.h \
    formacceptancewire.h \
    formaccnaklel.h \
    modelro.h \
    formaccnaklwire.h \
    dialogcfgprint.h \
    modelprint.h \
    formlabelsel.h \
    formdatael.h \
    labelbase.h \
    dialogprintlabel.h \
    labelsel.h \
    tprinter.h \
    dialogcmd.h \
    formlabelswire.h \
    formdatawire.h \
    labelswire.h \
    doublelineedit.h \
    formcells.h \
    dialoglblsize.h \
    formdrv.h \
    formbalanceew.h \
    progressreportdialog.h \
    formreportel.h \
    formreportwire.h \
    formretwire.h \
    formretel.h \
    formmovel.h \
    dialogrest.h \
    formmovwire.h \
    formshipcons.h \
    dialogshipload.h \
    db/dbcombobox.h \
    db/dbdateedit.h \
    db/dbrelationeditdialog.h \
    db/executor.h \
    db/tablemodel.h \
    progressexecutor.h \
    invoicemanager.h

FORMS += \
    dialogpacker.ui \
    dialogreqload.ui \
    dialogwebview.ui \
    formcert.ui \
    formpack.ui \
    formreqreport.ui \
    formrequests.ui \
        mainwindow.ui \
    db/dblogin.ui \
    formship.ui \
    dialogcods.ui \
    formpoluch.ui \
    formbalance.ui \
    olap/axiswidget.ui \
    olap/cubewidget.ui \
    olap/dialogolapflt.ui \
    olap/formfilter.ui \
    formacceptanceel.ui \
    formacceptancewire.ui \
    formaccnaklel.ui \
    formaccnaklwire.ui \
    dialogcfgprint.ui \
    formlabelsel.ui \
    formdatael.ui \
    dialogprintlabel.ui \
    dialogcmd.ui \
    formlabelswire.ui \
    formdatawire.ui \
    formcells.ui \
    dialoglblsize.ui \
    formdrv.ui \
    formbalanceew.ui \
    progressreportdialog.ui \
    formreportel.ui \
    formreportwire.ui \
    formretwire.ui \
    formretel.ui \
    formmovel.ui \
    dialogrest.ui \
    formmovwire.ui \
    formshipcons.ui \
    dialogshipload.ui \
    db/dbrelationeditdialog.ui \
    reader.ui

RC_FILE = ico.rc

DISTFILES += \
    ico.rc

RESOURCES += \
    res.qrc
