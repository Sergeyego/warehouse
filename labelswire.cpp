#include "labelswire.h"

LabelE801016::LabelE801016(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{
    setCut(false);
}

QString LabelE801016::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(text(6.25,50,QString::fromUtf8("Марка - ")+data->marka(),12));
    cod.push_back(text(6.25,55,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),12));
    cod.push_back(text(6.25,60,QString::fromUtf8("Плавка - ")+data->plavka(),12));
    cod.push_back(text(6.25,65,QString::fromUtf8("Партия - ")+data->part(),12));
    cod.push_back(text(6.25,70,QString::fromUtf8("Тип носителя - ")+data->spool(),12));
    cod.push_back(text(6.25,75,QString::fromUtf8("Код продукции - ")+data->codeProd(),12));
    cod.push_back(text(6.25,80,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),12));
    cod.push_back(text(6.25,85,QString::fromUtf8("Дата изг. - ")+data->datePart(),12));
    cod.push_back(ean13(13.75,33.75,data->eanEd(),9,0.375,0));

    cod.push_back(otkStamp(55,20,data->otkNum()));
    return cod;
}

QByteArray LabelE801016::getImages()
{
    return bitmap(15,13.75,39,17,":/images/logo.BMP");
}

LabelG95110::LabelG95110(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{
    setCut(false);
}

QString LabelG95110::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(text(6.25,32.5,QString::fromUtf8("Проволока сварочная"),12));
    cod.push_back(text(45,31.875,data->marka(),14));
    cod.push_back(block(6.25,37.5,86.25,10,data->gost(),12));
    cod.push_back(block(6.25,47.5,86.25,15,data->description(),10));
    cod.push_back(text(6.25,62.5,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),12));
    cod.push_back(text(6.25,67.5,QString::fromUtf8("Плавка - ")+data->plavka(),12));
    cod.push_back(text(6.25,72.5,QString::fromUtf8("Партия № ")+data->part(),12));
    cod.push_back(text(6.25,77.5,QString::fromUtf8("Тип носителя - ")+data->spool(),12));
    cod.push_back(text(45,62.5,QString::fromUtf8("Дата изг. - ")+data->datePart(),12));
    cod.push_back(text(45,67.5,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),12));
    cod.push_back(text(45,72.5,QString::fromUtf8("Упаковщик № ")+data->upkNum(),12));
    cod.push_back(block(6.25,82.5,67,18,data->getSert(),10));
    cod.push_back(dataMatrix(75,82.5,15,0.85,data->barCode()));
    cod.push_back(block(6.25,100.5,86.25,7,data->adr(),10));
    cod.push_back(ean13(50,13.75,data->eanGr(),12.5,0.375));
    cod.push_back(otkStamp(60,37,data->otkNum()));
    return cod;
}

QByteArray LabelG95110::getImages()
{
    return bitmap(6.25,13.75,39,17,":/images/logo.BMP");
}

LabelE4570::LabelE4570(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelE4570::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(ean13(5,20.5,data->eanEd(),9,0.375,0));
    cod.push_back(text(2.5,35,QString::fromUtf8("Марка - ")+data->marka(),10));
    cod.push_back(text(2.5,39,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),10));
    cod.push_back(text(2.5,43,QString::fromUtf8("Плавка - ")+data->plavka(),10));
    cod.push_back(text(2.5,47,QString::fromUtf8("Партия - ")+data->part(),10));

    QRegExp reg("^L-(\\d+)$");

    if (reg.indexIn(data->spool())==-1){ //обычная этикетка
        cod.push_back(text(2.5,51,QString::fromUtf8("Тип носителя - ")+data->spool(),10));
        cod.push_back(text(2.5,55,QString::fromUtf8("Код продукции - ")+data->codeProd(),10));
        cod.push_back(text(2.5,59,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),10));
        cod.push_back(text(2.5,63,QString::fromUtf8("Дата изг. - ")+data->datePart(),10));
        cod.push_back(otkStamp(33,40,data->otkNum()));
    } else { //длинномер
        cod.push_back(text(2.5,51,QString::fromUtf8("Длина, мм - ")+reg.cap(1),10));
        cod.push_back(text(2.5,55,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),10));
        cod.push_back(text(2.5,59,QString::fromUtf8("Дата изг. - ")+data->datePart(),10));
        cod.push_back(block(2.5,63,40,5,data->gost(),10));
        cod.push_back(dataMatrix(31,43,12,0.7,data->barCode()));
    }
    return cod;
}

QByteArray LabelE4570::getImages()
{
    return bitmap(2,2,39,17,":/images/logo.BMP");
}

LabelG100100::LabelG100100(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{
}

QString LabelG100100::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(text(6.25,25,QString::fromUtf8("Проволока сварочная"),12));
    cod.push_back(text(45,24.375,data->marka(),14));
    cod.push_back(block(6.25,30,86.25,10,data->gost(),12));
    cod.push_back(block(6.25,40,/*86.25*/70,15,data->description(),10));
    cod.push_back(text(6.25,55,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),12));
    cod.push_back(text(6.25,60,QString::fromUtf8("Плавка - ")+data->plavka(),12));
    cod.push_back(text(6.25,65,QString::fromUtf8("Партия № ")+data->part(),12));
    cod.push_back(text(6.25,70,QString::fromUtf8("Тип носителя - ")+data->spool(),12));
    cod.push_back(text(45,55,QString::fromUtf8("Дата изг. - ")+data->datePart(),12));
    cod.push_back(text(45,60,QString::fromUtf8("Масса нетто, кг - ")+data->masGroup(),12));
    cod.push_back(text(45,65,QString::fromUtf8("Упаковщик № ")+data->upkNum(),12));
    cod.push_back(block(6.25,75,74,15,data->getSert(),10));
    cod.push_back(dataMatrix(80,75,15,0.85,data->barCode()));
    cod.push_back(block(6.25,90,86.25,7,data->adr(),10));
    cod.push_back(ean13(50,6.25,data->eanGr(),12.5,0.375));
    cod.push_back(otkStamp(60,29.5,data->otkNum()));
    return cod;
}

QByteArray LabelG100100::getImages()
{
    QByteArray arr;
    arr.push_back(bitmap(6.25,6.25,39,17,":/images/logo.BMP"));
    arr.push_back(bitmap(76.25,40,20,12,":/images/sign.BMP",270));
    return arr;
}

LabelG100100Pal::LabelG100100Pal(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{
    setPrintCmdMode(true);
    setCutKvo(1);
}

QString LabelG100100Pal::getCod()
{
    QString palBarcode=Models::instance()->createPalBarcode("W");

    QString cod=LabelBase::getCod();
    cod.push_back(ean128(4,4,palBarcode,13));
    if (!data->eanGr().isEmpty()){
        cod.push_back(dataMatrix(70,67,20,1,data->barCodePack()+palBarcode));
    }
    cod.push_back(text(6.25,24,QString::fromUtf8("УПАКОВОЧНЫЙ ЛИСТ"),16));
    cod.push_back(text(6.25,31,QString::fromUtf8("Марка - ")+data->marka(),14));
    cod.push_back(text(6.25,37,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),14));
    cod.push_back(text(6.25,43,QString::fromUtf8("Плавка - ")+data->plavka(),14));
    cod.push_back(text(6.25,49,QString::fromUtf8("Партия - ")+data->part()+QString::fromUtf8(" Дата выпуска - ")+data->datePart(),14));

    QRegExp reg("^L-(\\d+)$");

    if (reg.indexIn(data->spool())==-1){ //обычная этикетка
        cod.push_back(text(6.25,55,QString::fromUtf8("Тип носителя - ")+data->spool(),14));
        cod.push_back(text(6.25,61,QString::fromUtf8("Количество кассет - ")+data->kvoSpool(),14));
        cod.push_back(text(6.25,67,QString::fromUtf8("Масса нетто, кг - ")+data->masPal(),14));
        cod.push_back(text(6.25,73,QString::fromUtf8("Мастер - ")+data->master(),14));
    } else { //длинномер
        cod.push_back(text(6.25,55,QString::fromUtf8("Длина, мм - ")+reg.cap(1),14));
        cod.push_back(text(6.25,61,QString::fromUtf8("Количество мест - ")+data->kvoSpool(),14));
        cod.push_back(text(6.25,67,QString::fromUtf8("Масса нетто, кг - ")+data->masPal(),14));
        cod.push_back(text(6.25,73,QString::fromUtf8("Упаковщик ___________________"),14));
    }
    cod.push_back(text(6.25,79,QString::fromUtf8("Дата упаковки - ")+data->datePack(),14));
    cod.push_back(text(6.25,90,QString::fromUtf8("НЕ БРОСАТЬ!"),16));
    cod.push_back(print(1));
    return cod;
}

LabelE7035::LabelE7035(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelE7035::getCod()
{
    QString cod=LabelBase::getCod();
    cod.push_back(ean13(43,2.5,data->eanEd(),9,0.25,0));
    cod.push_back(text(2.5,2.5,QString::fromUtf8("Марка - ")+data->marka(),10));
    cod.push_back(text(2.5,6,QString::fromUtf8("Диаметр, мм - ")+data->diametr(),10));
    cod.push_back(text(2.5,9.5,QString::fromUtf8("Плавка - ")+data->plavka(),10));
    cod.push_back(text(2.5,13,QString::fromUtf8("Партия - ")+data->part(),10));

    QRegExp reg("^L-(\\d+)$");

    if (reg.indexIn(data->spool())==-1){ //обычная этикетка
        cod.push_back(text(2.5,16.5,QString::fromUtf8("Тип носителя - ")+data->spool(),10));
        cod.push_back(text(2.5,20,QString::fromUtf8("Код продукции - ")+data->codeProd(),10));
        cod.push_back(text(2.5,23.5,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),10));
        cod.push_back(text(2.5,27,QString::fromUtf8("Дата изг. - ")+data->datePart(),10));
    } else { //длинномер
        cod.push_back(text(2.5,16.5,QString::fromUtf8("Длина, мм - ")+reg.cap(1),10));
        cod.push_back(text(2.5,20,QString::fromUtf8("Масса нетто, кг - ")+data->masEd(),10));
        cod.push_back(text(2.5,23.5,QString::fromUtf8("Дата изг. - ")+data->datePart(),10));
        cod.push_back(block(2.5,27,40,7,data->gost(),10));
    }

    cod.push_back(otkStamp(41,16,data->otkNum()));
    cod.push_back(dataMatrix(55,20,12,0.7,data->barCode()));
    return cod;
}

QByteArray LabelE7035::getImages()
{
    return LabelBase::getImages();
}

LabelWG60150::LabelWG60150(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent) : LabelBase(nam,w,h,g,parent), data(d)
{

}

QString LabelWG60150::getCod()
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

    cod.push_back(diagonal(11,68,27,68));
    cod.push_back(diagonal(11,120,45,120));

    cod.push_back(ean13(17,6,data->eanGr(),9,0.375,0));
    cod.push_back(otkStamp(13.5,56,data->otkNum(),true));
    cod.push_back(block(54,21,28,6,data->marka(),16,90,2));
    cod.push_back(block(56,51,33,10,data->gost(),8,90));
    cod.push_back(block(54,86,18,6,QString::fromUtf8("Арт.")+data->vendorCode(),12,90,2));
    cod.push_back(block(56,106,40,4,QString::fromUtf8("Проволока сварочная"),12,90,2));
    cod.push_back(block(50,106,40,4,data->diametr()+" "+data->marka(),12,90,2));

    cod.push_back(block(44,55,65,17,data->description(),10,90));
    cod.push_back(dataMatrix(25,106,13,0.7,data->barCode(),90));
    cod.push_back(block(44,121,25,21,data->getSert(),9,90));

    QString s=QString::fromUtf8("Диаметр, мм - ")+data->diametr()+"\n";
    s+=QString::fromUtf8("Партия - ")+data->part()+"\n";
    s+=QString::fromUtf8("Масса нетто, кг - ")+data->masGroup()+"\n";
    s+=QString::fromUtf8("Дата изг. - ")+data->datePart();

    cod.push_back(block(26,69,36,14,s,10,90));

    QString s1=QString::fromUtf8("Плавка - ")+data->plavka()+"\n";
    s1+=QString::fromUtf8("Тип носителя - ")+data->spool()+"\n";
    s1+=QString::fromUtf8("Упаковщик № ")+data->upkNum()+"\n";

    cod.push_back(block(26,22,40,14,s1,10,90));

    cod.push_back(block(6.5,4,142,3,data->adr(),8,90));

    return cod;
}

QByteArray LabelWG60150::getImages()
{
    QByteArray cod;
    cod+=bitmap(12,124,11,19,":/images/sign.BMP");
    cod+=bitmap(29,22,15,30,":/images/logo.BMP",90);
    return cod;
}
