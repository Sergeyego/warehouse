#-------------------------------------------------
#
# Project created by QtCreator 2021-12-07T09:38:55
#
#-------------------------------------------------

QT       += core gui sql network xml

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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    formacceptance.cpp \
    sync1c.cpp \
    models.cpp \
    formship.cpp \
    dialogcods.cpp

HEADERS += \
        mainwindow.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    formacceptance.h \
    sync1c.h \
    models.h \
    formship.h \
    dialogcods.h

FORMS += \
        mainwindow.ui \
    db/dblogin.ui \
    formacceptance.ui \
    formship.ui \
    dialogcods.ui

RC_FILE = ico.rc

DISTFILES += \
    ico.rc

RESOURCES += \
    res.qrc
