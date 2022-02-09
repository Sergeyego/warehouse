#include "labelsel.h"


LabelE5040::LabelE5040(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelE5040::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(ean13(7,3,data->eanEd(),9,0.375,0));
    /*QString s=QString::fromUtf8("Марка - ")+data->marka()+"\n";
    s+=QString::fromUtf8("Диаметр, мм - ")+data->diametr()+"\n";
    s+=QString::fromUtf8("Партия - ")+data->part()+"\n";
    s+=QString::fromUtf8("Масса нетто, кг - ")+data->masEd()+"\n";
    s+=QString::fromUtf8("Дата изг. - ")+data->datePart();
    cod.push_back(block(3,18,25,20,s,10));
    cod.push_back(qrCode(30,18,data->qrCode(),4));*/
    cod.push_back(text(5,18,QString::fromUtf8("Марка - ")+data->marka()));
    cod.push_back(text(5,22,QString::fromUtf8("Диаметр, мм - ")+data->diametr()));
    cod.push_back(text(5,26,QString::fromUtf8("Партия - ")+data->part()));
    cod.push_back(text(5,30,QString::fromUtf8("Масса нетто, кг - ")+data->masEd()));
    cod.push_back(text(5,34,QString::fromUtf8("Дата изг. - ")+data->datePart()));
    return cod;
}

LabelG5040::LabelG5040(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelG5040::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(ean13(7,3,data->eanGr(),9,0.375,0));
    cod.push_back(text(5,18,QString::fromUtf8("Марка - ")+data->marka()));
    cod.push_back(text(5,22,QString::fromUtf8("Диаметр, мм - ")+data->diametr()));
    cod.push_back(text(5,26,QString::fromUtf8("Партия - ")+data->part()));
    cod.push_back(text(5,30,QString::fromUtf8("Масса нетто, кг - ")+data->masGr()));
    cod.push_back(text(5,34,QString::fromUtf8("Дата изг. - ")+data->datePart()));
    return cod;
}
