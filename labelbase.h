#ifndef LABELBASE_H
#define LABELBASE_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include "dialogprintlabel.h"

class LabelBase : public QObject
{
    Q_OBJECT
public:
    explicit LabelBase(QString nam, double w, double h, double g, QObject *parent = nullptr);
    ~LabelBase();
    QString getName();
    void setDpi(int d);
    double getWidth();
    double getHeight();
    double getGap();
    QString print(int count);
    QString calibrate();
    QString cut(bool en);
    virtual QString getCod();
    QString getPrinterName();
    void setPrinterName(QString n);
    void loadSettings();
    void saveSettings();
    void setPrintCmdMode(bool b);

private:
    double width;
    double height;
    double gap;
    int dpi;
    QString name;
    QString printerName;
    bool printCmdMode;

protected:
    int getDots(double mm);
    QString normalize(QString t);
    QString logo(double x, double y);
    QString text(double x, double y, QString t, int size=10, int rotation=0);
    QString block(double x, double y, double w, double h, QString t, int size=10, int rotation=0);
    QString ean13(double x, double y, QString ean, double h=9, double s=0.375, int rotation=0);
    QString qrCode(double x, double y, QString t, int cellWidth=5);
    QString dataMatrix(double x, double y, double size, double sizeCell, QString data);
    QString otkStamp(double x, double y, QString num);
    QString cls();

signals:

public slots:
    void printLabel();
};

#endif // LABELBASE_H
