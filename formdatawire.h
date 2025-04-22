#ifndef FORMDATAWIRE_H
#define FORMDATAWIRE_H

#include <QWidget>
#include "modelro.h"
#include <QDataWidgetMapper>
#include <QCompleter>
#include <QComboBox>
#include <QSettings>
#include <math.h>

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
    QString masEd();
    QString masGroup();
    QString eanEd();
    QString eanGr();
    QString vendorCode();
    QString barCode();
    QString barCodePack();
    QString codeProd();
    QString otkNum();
    QString otkNam();
    QString gost();
    QString description();
    QString upkNum();
    QString adr();
    QString kvoSpool();
    QString masPal();
    QString master();
    int getIdPart();
    bool checkEd(bool checkTu=false);
    bool checkGroup();

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
    void loadSettings();
    void saveSettings();
    QVariant currentData(int row);

private slots:
    void refreshData(QModelIndex index);
    void genEan();
    void setOrigPart();
    void refreshDocType();
    void setKvoPack();

public slots:
    void updPart();
};

#endif // FORMDATAWIRE_H
