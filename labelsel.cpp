#include "labelsel.h"


LabelE5040::LabelE5040(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelE5040::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(ean13(7,3,data->eanEd(),9,0.375,0));
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
    QString s=QString::fromUtf8("Марка - ")+data->marka()+"\n";
    s+=QString::fromUtf8("Диаметр, мм - ")+data->diametr()+"\n";
    s+=QString::fromUtf8("Партия - ")+data->part()+"\n";
    s+=QString::fromUtf8("Масса нетто, кг - ")+data->masGr()+"\n";
    s+=QString::fromUtf8("Дата изг. - ")+data->datePart();
    cod.push_back(block(2,18,35,20,s,10));
    cod.push_back(dataMatrix(37,25,13,0.7,data->barCodeGr()));

    return cod;
}

LabelE60150::LabelE60150(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelE60150::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(box(3,3,57,147));
    cod.push_back(diagonal(7,3,7,147));
    cod.push_back(diagonal(11,3,11,147));
    cod.push_back(diagonal(11,20,57,20));

    cod.push_back(diagonal(45,20,45,147));
    cod.push_back(diagonal(27,20,27,/*147*/105));

    cod.push_back(diagonal(45,50,57,50));
    cod.push_back(diagonal(45,85,57,85));
    cod.push_back(diagonal(45,105,57,105));

    cod.push_back(diagonal(11,32,27,32));
    cod.push_back(diagonal(11,68,27,68));
    cod.push_back(diagonal(11,105,/*27*/45,105));

    cod.push_back(diagonal(15,20,15,68));
    cod.push_back(diagonal(19,32,19,68));
    cod.push_back(diagonal(23,32,23,68));
    cod.push_back(diagonal(11,44,19,44));
    cod.push_back(diagonal(11,56,19,56));

    cod.push_back(diagonal(51,106,51,146));
    cod.push_back(ean13(17,6,data->eanEd(),9,0.375,0));
    cod.push_back(pixPol(31,23,10,0.375,data->posPix()));
    cod.push_back(block(54,21,28,6,data->marka(),16,90,2));
    cod.push_back(block(56,51,33,10,data->gost(),10,90));
    cod.push_back(block(54,86,18,6,tr("Арт.")+data->vendorCode(),12,90,2));
    cod.push_back(block(56,106,42,4,data->numerator(),12,90,2));
    cod.push_back(block(50,106,42,4,data->znam(),12,90,2));
    cod.push_back(block(44,35,/*98*/70,16,data->descr(),10,90));
    //cod.push_back(dataMatrix(42,134,13,0.7,data->barCodeEd(),90));

    cod.push_back(block(44,106,32,41,data->sert(),10,90));



    return cod;
}

LabelG60150::LabelG60150(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelG60150::getCod()
{
    QString cod=LabelBase::getCod();

    return cod;
}
