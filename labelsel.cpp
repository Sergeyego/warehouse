#include "labelsel.h"

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
    cod.push_back(diagonal(27,20,27,120));

    cod.push_back(diagonal(45,50,57,50));
    cod.push_back(diagonal(45,85,57,85));
    cod.push_back(diagonal(45,105,57,105));

    cod.push_back(diagonal(11,32,27,32));
    cod.push_back(diagonal(11,68,27,68));
    cod.push_back(diagonal(11,120,45,120));

    cod.push_back(diagonal(15,20,15,68));
    cod.push_back(diagonal(19,32,19,68));
    cod.push_back(diagonal(23,32,23,68));
    cod.push_back(diagonal(11,44,19,44));
    cod.push_back(diagonal(11,56,19,56));

    cod.push_back(ean13(17,6,data->eanEd(),9,0.375,0));
    cod.push_back(pixPol(31,23,10,0.375,data->posPix()));
    cod.push_back(block(54,21,28,6,data->markaSert(),16,90,2));
    cod.push_back(block(56,51,33,10,data->gost(),8,90));
    cod.push_back(block(54,86,18,6,QString::fromUtf8("Арт.")+data->vendorCode(),12,90,2));
    cod.push_back(block(56,106,40,4,data->numerator(),12,90,2));
    QString znam=data->znam();
    if (!znam.isEmpty() && znam!="-"){
        cod.push_back(diagonal(51,106,51,146));
        cod.push_back(block(50,106,40,4,znam,12,90,2));
    }
    cod.push_back(block(44,35,84,16,data->descr(),8,90));
    cod.push_back(dataMatrix(25,106,13,0.7,data->barCodeEd(),90));
    cod.push_back(block(44,121,25,32,data->sert(),9,90));

    QString s=QString::fromUtf8("Диаметр, мм - ")+data->diametr()+"\n";
    s+=QString::fromUtf8("Партия - ")+data->part()+"\n";
    s+=QString::fromUtf8("Масса нетто, кг - ")+data->masEd()+"\n";
    s+=QString::fromUtf8("Дата изг. - ")+data->datePart();

    cod.push_back(block(26,69,36,14,s,10,90));

    cod.push_back(block(26,21,10,14,QString::fromUtf8("Диам.,\nмм"),8,90,2));

    cod.push_back(block(26.5,33,34,3,QString::fromUtf8("Рекомендуемое значение тока, А"),8,90,2));
    cod.push_back(block(22.5,33,34,3,QString::fromUtf8("Положение шва"),8,90,2));

    cod.push_back(block(18.5,33,10,3,QString::fromUtf8("Нижнее"),8,90,2));
    cod.push_back(block(18.5,45,10,3,QString::fromUtf8("Вертик."),8,90,2));
    cod.push_back(block(18.5,57,10,3,QString::fromUtf8("Потолоч."),8,90,2));

    cod.push_back(block(14.5,21,10,3,data->amp().diam,8,90,2));
    cod.push_back(block(14.5,33,10,3,data->amp().bot,8,90,2));
    cod.push_back(block(14.5,45,10,3,data->amp().vert,8,90,2));
    cod.push_back(block(14.5,57,10,3,data->amp().ceil,8,90,2));

    QString dop=QString::fromUtf8("Допустимое содержание влаги в покрытии перед использованием - %1 \%. Режим повторной прокалки: %2.").arg(data->vl()).arg(data->proc());
    cod.push_back(block(10.5,4,142,3,dop,8,90));
    cod.push_back(block(6.5,4,142,3,data->adr(),8,90));

    return cod;
}

LabelG60150::LabelG60150(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelG60150::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(box(3,3,57,147));
    cod.push_back(diagonal(7,3,7,147));
    cod.push_back(diagonal(11,3,11,147));
    cod.push_back(diagonal(11,20,57,20));

    cod.push_back(diagonal(45,20,45,147));
    cod.push_back(diagonal(27,20,27,120));

    cod.push_back(diagonal(45,50,57,50));
    cod.push_back(diagonal(45,85,57,85));
    cod.push_back(diagonal(45,105,57,105));

    cod.push_back(diagonal(11,32,27,32));
    cod.push_back(diagonal(11,68,27,68));
    cod.push_back(diagonal(11,120,45,120));

    cod.push_back(diagonal(15,20,15,68));
    cod.push_back(diagonal(19,32,19,68));
    cod.push_back(diagonal(23,32,23,68));
    cod.push_back(diagonal(11,44,19,44));
    cod.push_back(diagonal(11,56,19,56));

    cod.push_back(ean13(17,6,data->eanGr(),9,0.375,0));
    cod.push_back(pixPol(31,23,10,0.375,data->posPix()));
    cod.push_back(block(54,21,28,6,data->markaSert(),16,90,2));
    cod.push_back(block(56,51,33,10,data->gost(),8,90));
    cod.push_back(block(54,86,18,6,QString::fromUtf8("Арт.")+data->vendorCode(),12,90,2));
    cod.push_back(block(56,106,40,4,data->numerator(),12,90,2));
    QString znam=data->znam();
    if (!znam.isEmpty() && znam!="-"){
        cod.push_back(diagonal(51,106,51,146));
        cod.push_back(block(50,106,40,4,znam,12,90,2));
    }
    cod.push_back(block(44,35,84,16,data->descr(),8,90));
    cod.push_back(dataMatrix(25,106,13,0.7,data->barCodeGr(),90));
    cod.push_back(block(44,121,25,/*32*/21,data->sert(),9,90));

    QString s=QString::fromUtf8("Диаметр, мм - ")+data->diametr()+"\n";
    s+=QString::fromUtf8("Партия - ")+data->part()+"\n";
    s+=QString::fromUtf8("Масса нетто, кг - ")+data->masGr()+"\n";
    s+=QString::fromUtf8("Дата изг. - ")+data->datePart();

    cod.push_back(block(26,69,36,14,s,10,90));

    cod.push_back(block(26,21,10,14,QString::fromUtf8("Диам.,\nмм"),8,90,2));

    cod.push_back(block(26.5,33,34,3,QString::fromUtf8("Рекомендуемое значение тока, А"),8,90,2));
    cod.push_back(block(22.5,33,34,3,QString::fromUtf8("Положение шва"),8,90,2));

    cod.push_back(block(18.5,33,10,3,QString::fromUtf8("Нижнее"),8,90,2));
    cod.push_back(block(18.5,45,10,3,QString::fromUtf8("Вертик."),8,90,2));
    cod.push_back(block(18.5,57,10,3,QString::fromUtf8("Потолоч."),8,90,2));

    cod.push_back(block(14.5,21,10,3,data->amp().diam,8,90,2));
    cod.push_back(block(14.5,33,10,3,data->amp().bot,8,90,2));
    cod.push_back(block(14.5,45,10,3,data->amp().vert,8,90,2));
    cod.push_back(block(14.5,57,10,3,data->amp().ceil,8,90,2));

    QString dop=QString::fromUtf8("Допустимое содержание влаги в покрытии перед использованием - %1 \%. Режим повторной прокалки: %2.").arg(data->vl()).arg(data->proc());
    cod.push_back(block(10.5,4,142,3,dop,8,90));
    cod.push_back(block(6.5,4,142,3,data->adr(),8,90));

    return cod;
}

QByteArray LabelG60150::getImages()
{
    return bitmap(12,124,11,19,":/images/sign.BMP");
}
