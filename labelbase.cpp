#include "labelbase.h"

LabelBase::LabelBase(QString nam, double w, double h, double g, QObject *parent) : QObject(parent), name(nam), width(w), height(h), gap(g)
{
    dpi=200;
}

QString LabelBase::getName()
{
    return name;
}

void LabelBase::setDpi(int d)
{
    dpi=d;
}

double LabelBase::getWidth()
{
    return width;
}

double LabelBase::getHeight()
{
    return height;
}

double LabelBase::getGap()
{
    return gap;
}

QString LabelBase::getCod()
{
    QString cod;
    cod.push_back("DIRECTION 1,0\n");
    cod.push_back("CLS\n");
    cod.push_back(QString("SIZE %1 mm,%2 mm\n").arg(width).arg(height));
    cod.push_back(QString("GAP %1 mm,0 mm\n").arg(gap));
    cod.push_back("CODEPAGE UTF-8\n");
    cod.push_back(QString("DENSITY %1\n").arg(12));
    return cod;
}

int LabelBase::getDots(double mm)
{
    return dpi*mm/25;
}

QString LabelBase::normalize(QString t)
{
    return t.replace("\"","'");
}

QString LabelBase::ean13(double x, double y, QString ean, double h, double s, int rotation)
{
    QString c;
    if (ean.size()==12){
        c=QString("BARCODE %1,%2,\"EAN13\",%3,2,%7,%4,%5,\"%6\"\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(h)).arg(getDots(s)).arg(getDots(s)).arg(ean).arg(rotation);
    }
    return c;
}

QString LabelBase::print(int count)
{
    return QString("PRINT %1\n").arg(count);
}

QString LabelBase::qrCode(double x, double y, QString t, int cellWidth)
{
    return QString ("QRCODE %1,%2,M,%3,A,0,M2, \"%4\"\n").arg(getDots(x)).arg(getDots(y)).arg(cellWidth).arg(normalize(t));
}

QString LabelBase::logo(double x, double y)
{
    return QString("PUTBMP %1,%2, \"logo.BMP\",1,100\n").arg(getDots(x)).arg(getDots(y));
}

QString LabelBase::text(double x, double y, QString t, int size, int rotation)
{
    return QString("TEXT %1,%2,\"0\",%6,%3,%4,\"%5\"\n").arg(getDots(x)).arg(getDots(y)).arg(size).arg(size).arg(normalize(t)).arg(rotation);
}

QString LabelBase::block(double x, double y, double w, double h, QString t, int size, int rotation)
{
    return QString("BLOCK %1,%2,%3,%4,\"0\",%5,%6,%7,0,0,1,\"%8\"\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(w)).arg(getDots(h)).arg(rotation).arg(size).arg(size).arg(normalize(t));
}

void LabelBase::printLabel()
{
    DialogPrintLabel d(this);
    d.exec();
    //QString c=getCod();
    //printer->printDecode(c);
}

void LabelBase::calibrate()
{
    QString cmd;
    cmd+=QString("SIZE %1 mm, %2 mm\n").arg(width).arg(height);
    cmd+=QString("AUTODETECT %1, %2\n").arg(getDots(height)).arg(getDots(gap));
}