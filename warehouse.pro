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
        main.cpp \
        mainwindow.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    sync1c.cpp \
    models.cpp \
    formship.cpp \
    dialogcods.cpp \
    formpoluch.cpp \
    formbalance.cpp \
    tablemodel.cpp \
    formturnovers.cpp \
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
    dialogprintpacklist.cpp \
    qr/qrencode.cpp \
    formcells.cpp \
    dialoglblsize.cpp \
    code128/code128.cpp \
    formdrv.cpp

HEADERS += \
        mainwindow.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    sync1c.h \
    models.h \
    formship.h \
    dialogcods.h \
    formpoluch.h \
    formbalance.h \
    tablemodel.h \
    formturnovers.h \
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
    dialogprintpacklist.h \
    qr/qrencode.h \
    formcells.h \
    dialoglblsize.h \
    code128/code128.h \
    formdrv.h

FORMS += \
        mainwindow.ui \
    db/dblogin.ui \
    formship.ui \
    dialogcods.ui \
    formpoluch.ui \
    formbalance.ui \
    formturnovers.ui \
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
    dialogprintpacklist.ui \
    formcells.ui \
    dialoglblsize.ui \
    formdrv.ui

RC_FILE = ico.rc

DISTFILES += \
    ico.rc

RESOURCES += \
    res.qrc
