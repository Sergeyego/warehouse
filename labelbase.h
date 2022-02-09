#ifndef LABELBASE_H
#define LABELBASE_H

#include <QObject>
#include "dialogprintlabel.h"

class LabelBase : public QObject
{
    Q_OBJECT
public:
    explicit LabelBase(QString nam, double w, double h, double g, QObject *parent = nullptr);
    QString getName();
    void setDpi(int d);
    double getWidth();
    double getHeight();
    double getGap();
    QString print(int count);
    virtual QString getCod();

protected:
    double width;
    double height;
    double gap;
    int dpi;
    QString name;
    int getDots(double mm);
    QString normalize(QString t);
    QString logo(double x, double y);
    QString text(double x, double y, QString t, int size=10, int rotation=0);
    QString block(double x, double y, double w, double h, QString t, int size=10, int rotation=0);
    QString ean13(double x, double y, QString ean, double h=9, double s=0.375, int rotation=0);
    QString qrCode(double x, double y, QString t, int cellWidth=5);

signals:

public slots:
    void printLabel();
    void calibrate();
};

#endif // LABELBASE_H
