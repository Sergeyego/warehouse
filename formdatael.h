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

struct picInfo {
    int cod;
    QByteArray data;
};

class ModelPart : public ModelRo{
    Q_OBJECT
public:
    ModelPart(QObject *parent = nullptr);
    bool refresh(QDate dbeg, QDate dend);
    QVariant data(const QModelIndex &item, int role) const;
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
    ModelPart *modelPart;
    QDataWidgetMapper *mapper;
    QSqlQueryModel *modelPacker;
    QMap <int, QString> docType;
    QMap <int, picInfo> picMap;
    QString strAdr;
    ModelRo *modelGost;
    ModelRo *modelAmp;
    void loadSettings();
    void saveSettings();
    QVariant currentData(int row);
    QString getSrtStr(int id_part);

private slots:
    void refreshData(QModelIndex index);
    void genEan();
    void setKvoPack();
    void refreshDocType();
    void refreshPicMap();

public slots:
    bool updPart();
    void updPacker();
};

#endif // FORMDATAEL_H
