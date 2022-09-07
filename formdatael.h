#ifndef FORMDATAEL_H
#define FORMDATAEL_H

#include <QWidget>
#include "modelro.h"
#include <QDataWidgetMapper>
#include <QSettings>
#include <math.h>
#include <QItemDelegate>
#include "models.h"

namespace Ui {
class FormDataEl;
}

struct ampInfo {
    QString diam;
    QString bot;
    QString vert;
    QString ceil;
};

class FormDataEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormDataEl(QWidget *parent = 0);
    ~FormDataEl();
    QString marka();
    QString markaSert();
    QString diametr();
    QString part();
    QString datePart();
    QString datePack();
    QString packer();
    QString packEd();
    QString packGr();
    QString masEd();
    QString masGr();
    QString masPal();
    QString kvoPackPal();
    QString eanEd();
    QString eanGr();
    QString codePart();
    QString barCodeEd();
    QString barCodeGr();
    QString barCodePack();
    QString descr();
    QString numerator();
    QString znam();
    QString vendorCode();
    QString gost();
    QString sert();
    QString proc();
    QString vl();
    QString adr();
    ampInfo amp();
    int posPix();
    bool check();

private:
    Ui::FormDataEl *ui;
    ModelRo *modelPart;
    QDataWidgetMapper *mapper;
    QSqlQueryModel *modelPacker;
    DbRelation *relPosPix;
    QString strAdr;
    ModelRo *modelGost;
    ModelRo *modelAmp;
    bool selectPart();
    void loadSettings();
    void saveSettings();
    QVariant currentData(int row);
    QString getSrtStr(int id_part);

private slots:
    void refreshData(QModelIndex index);
    void genEan();
    void setKvoPack();

public slots:
    void updPart();
    void updPacker();
};

class CustomDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CustomDelegate(QObject *parent=0);
    void setEditorData(QWidget *editor,const QModelIndex &index) const;
};

#endif // FORMDATAEL_H
