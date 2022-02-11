#ifndef FORMDATAWIRE_H
#define FORMDATAWIRE_H

#include <QWidget>
#include "modelro.h"
#include <QDataWidgetMapper>
#include <QCompleter>
#include <QComboBox>

namespace Ui {
class FormDataWire;
}

class FormDataWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormDataWire(QWidget *parent = 0);
    ~FormDataWire();
    QString getSert();
    QString marka();
    QString diametr();
    QString plavka();
    QString part();
    QString datePart();
    QString datePack();
    QString spool();
    QString mas();
    QString eanEd();
    QString eanGr();
    QString barCode();
    QString codeProd();
    QString otkNum();
    QString otkNam();
    QString gost();
    QString description();
    QString upkNum();
    QString adr();
    QString kvoSpool();
    QString master();

private:
    Ui::FormDataWire *ui;
    ModelRo *modelPart;
    ModelRo *modelTu;
    ModelRo *modelOtk;
    ModelRo *modelNam;
    ModelRo *modelOdobr;
    ModelRo *modelPartOrig;
    ModelRo *modelMaster;
    QDataWidgetMapper *mapper;
    QMap <int, QString> docType;
    bool selectPart();
    QString strAdr;
    QString site;
    QString getNum(QComboBox *c);

private slots:
    void refreshData(QModelIndex index);
    void genEan();
    void setOrigPart();
    void refreshDocType();

public slots:
    void updPart();
};

#endif // FORMDATAWIRE_H
