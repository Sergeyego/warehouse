#include "labelbase.h"

LabelBase::LabelBase(QString nam, double w, double h, double g, QObject *parent) : QObject(parent), width(w), height(h), gap(g), name(nam)
{
    dpi=200;
    density=12;
    printCmdMode=false;
    cutMode=true;
    cutKvo=0;
    loadSettings();
}

LabelBase::~LabelBase()
{
    saveSettings();
}

QString LabelBase::getName()
{
    return name;
}

void LabelBase::setDpi(int d)
{
    dpi=d;
}

void LabelBase::setDensity(int d)
{
    density=d;
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
    if (cutMode){
        cod.push_back(QString("GAP %1 mm,0 mm\n").arg(gap));
    } else {
        cod.push_back(QString("BLINE %1 mm,0 mm\n").arg(gap));
    }
    cod.push_back("CODEPAGE UTF-8\n");
    cod.push_back(QString("DENSITY %1\n").arg(density));
    return cod;
}

QByteArray LabelBase::getImages()
{
    return QByteArray();
}

QString LabelBase::getPrinterName()
{
    return printerName;
}

void LabelBase::setPrinterName(QString n)
{
    printerName=n;
    saveSettings();
}

void LabelBase::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    printerName=settings.value(name+"_printerName").toString();
}

void LabelBase::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue(name+"_printerName",printerName);
}

void LabelBase::setPrintCmdMode(bool b)
{
    printCmdMode=b;
}

void LabelBase::setCut(bool b)
{
    cutMode=b;
}

void LabelBase::setCutKvo(int kvo)
{
    cutKvo=kvo;
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

QString LabelBase::ean128(double x, double y, QString ean, double h, double s, int rotation)
{
    return QString("BARCODE %1,%2,\"EAN128\",%3,2,%7,%4,%5,\"%6\"\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(h)).arg(getDots(s)).arg(getDots(s)).arg(ean).arg(rotation);
}

QString LabelBase::print(int count)
{
    return QString("PRINT %1\n").arg(count);
}

QString LabelBase::qrCode(double x, double y, QString t, int cellWidth)
{
    return QString ("QRCODE %1,%2,M,%3,A,0,M2, \"%4\"\n").arg(getDots(x)).arg(getDots(y)).arg(cellWidth).arg(normalize(t));
}

QString LabelBase::dataMatrix(double x, double y, double size, double sizeCell, QString data, int rotation)
{
    return QString ("DMATRIX %1,%2,%3,%4,x%5,r%6, \"%7\"\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(size)).arg(getDots(size)).arg(getDots(sizeCell)).arg(rotation).arg(normalize(data));
}

QString LabelBase::otkStamp(double x, double y, QString num, bool rotation)
{
    QString cod;
    if (!num.isEmpty()){
        cod.push_back(QString("CIRCLE %1,%2,%3,%4\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(11)).arg(getDots(0.5)));
        if (!rotation){
            cod.push_back(QString::fromUtf8("TEXT %1,%2,\"0\",0,12,12,\"ОТК\"\n").arg(getDots(x+2.0)).arg(getDots(y+2.0)));
            cod.push_back(QString::fromUtf8("TEXT %1,%2,\"0\",0,12,12,\"%3\"\n").arg(getDots(x+3.5)).arg(getDots(y+6.0)).arg(num));
        } else {
            cod.push_back(QString::fromUtf8("TEXT %1,%2,\"0\",90,12,12,\"ОТК\"\n").arg(getDots(x+9.5)).arg(getDots(y+2.0)));
            cod.push_back(QString::fromUtf8("TEXT %1,%2,\"0\",90,12,12,\"%3\"\n").arg(getDots(x+5.0)).arg(getDots(y+3.5)).arg(num));
        }
    }
    return cod;
}

QString LabelBase::box(double x, double y, double x1, double y1, double lineThickness, double radius)
{
    return QString("BOX %1,%2,%3,%4,%5,%6\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(x1)).arg(getDots(y1)).arg(getDots(lineThickness)).arg(getDots(radius));
}

QString LabelBase::diagonal(double x, double y, double x1, double y1, double lineThickness)
{
    return QString("DIAGONAL %1,%2,%3,%4,%5\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(x1)).arg(getDots(y1)).arg(getDots(lineThickness));
}

QString LabelBase::pixPol(double x, double y, double h, double lineThickness, int dat)
{
    double s=h*0.15;
    double sh=s*0.25;

    QString cod;

    if (dat & 1){
        cod.push_back(diagonal(x,y,x,y+h,lineThickness));
        cod.push_back(diagonal(x+sh,y+h-s,x,y+h,lineThickness));
        cod.push_back(diagonal(x-sh,y+h-s,x,y+h,lineThickness));
    }
    if (dat & 2) {
        cod.push_back(diagonal(x,y,x+h,y,lineThickness));
        cod.push_back(diagonal(x+h-s,y-sh,x+h,y,lineThickness));
        cod.push_back(diagonal(x+h-s,y+sh,x+h,y,lineThickness));
    }
    if (dat & 4) {
        cod.push_back(diagonal(x+h,y,x+h,y+h,lineThickness));
        cod.push_back(diagonal(x+h-sh,y+h-s,x+h,y+h,lineThickness));
        cod.push_back(diagonal(x+h+sh,y+h-s,x+h,y+h,lineThickness));
    }
    if (dat & 8){
        cod.push_back(diagonal(x+h/2,y,x+h/2,y+h,lineThickness));
        cod.push_back(diagonal(x+h/2-sh,y+h-s,x+h/2,y+h,lineThickness));
        cod.push_back(diagonal(x+h/2+sh,y+h-s,x+h/2,y+h,lineThickness));
    }
    if (dat & 16) {
        cod.push_back(diagonal(x,y+h,x+h,y+h,lineThickness));
        cod.push_back(diagonal(x+s,y+h-sh,x,y+h,lineThickness));
        cod.push_back(diagonal(x+s,y+h+sh,x,y+h,lineThickness));
    }
    if (dat & 32) {
        cod.push_back(diagonal(x,y,x+h/2,y+h/2,lineThickness));
        cod.push_back(diagonal(x+s*0.866,y+s*0.5,x,y,lineThickness));
        cod.push_back(diagonal(x+s*0.5,y+s*0.866,x,y,lineThickness));
    }

    return cod;
}

QString LabelBase::cls()
{
    return QString("CLS\n");
}

QByteArray LabelBase::bitmap(double x, double y, double w, double h, const QString &fileName, int rotation)
{
    int hw = getDots(w);
    int hd = getDots(h);

    if (hw<8){
        hw=8;
    }
    while (hw % 8 != 0){
        hw--;
    }

    int byteCount=hw/8;

    QByteArray arr;
    QImage img(fileName);

    if (rotation){
        QTransform mat;
        mat.rotate(rotation);
        img = img.transformed(mat);

    }
    img = img.scaled(hw,hd);

    int n=0;
    char b=0;
    for(int x=0; x<img.height();x++){
        for(int y=0; y<img.width(); y++){
            QColor currentColor=img.pixelColor(y,x);
            if (currentColor==Qt::white || currentColor.alpha()==0){
                b |= (1 << (7-n));
            }
            if (n>=7){
                arr.append(b);
                n=0;
                b=0;
            } else {
                n++;
            }
        }
    }
    QString str=QString("BITMAP %1,%2,%3,%4,0,").arg(getDots(x)).arg(getDots(y)).arg(byteCount).arg(hd);
    arr.prepend(str.toUtf8());
    arr.append(QString("\n").toUtf8());
    return arr;
}

QString LabelBase::text(double x, double y, QString t, int size, int rotation)
{
    return QString("TEXT %1,%2,\"0\",%6,%3,%4,\"%5\"\n").arg(getDots(x)).arg(getDots(y)).arg(size).arg(size).arg(normalize(t)).arg(rotation);
}

QString LabelBase::block(double x, double y, double w, double h, QString t, int size, int rotation, int align)
{
    return QString("BLOCK %1,%2,%3,%4,\"0\",%5,%6,%7,0,%8,1,\"%9\"\n").arg(getDots(x)).arg(getDots(y)).arg(getDots(w)).arg(getDots(h)).arg(rotation).arg(size).arg(size).arg(align).arg(normalize(t));
}

void LabelBase::printLabel()
{
    DialogPrintLabel d(this);
    d.setPrintCmdMode(printCmdMode);
    d.setCut(cutMode);
    d.exec();
}

QString LabelBase::calibrate()
{
    QString cmd;
    cmd+=QString("SIZE %1 mm, %2 mm\n").arg(width).arg(height);
    cmd+=QString("AUTODETECT %1, %2\n").arg(getDots(height)).arg(getDots(gap));
    return cmd;
}

QString LabelBase::cut(bool en)
{
    QString cmd;
    if (en && !printCmdMode){
        cmd+=QString("SET CUTTER BATCH\n");
    } else if (en && printCmdMode && cutKvo>0){
        cmd+=QString("SET CUTTER %1\n").arg(cutKvo);
    } else {
        cmd+=QString("SET CUTTER OFF\n");
    }
    return cmd;
}
