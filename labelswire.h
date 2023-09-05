#ifndef LABELSWIRE_H
#define LABELSWIRE_H

#include <QObject>
#include "labelbase.h"
#include "formdatawire.h"
#include "models.h"

class LabelE801016 : public LabelBase
{
public:
    LabelE801016(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent);
protected:
    virtual QString getCod();
    virtual QByteArray getImages();
    FormDataWire *data;
};

class LabelG95110 : public LabelBase
{
public:
    LabelG95110(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent);
protected:
    virtual QString getCod();
    virtual QByteArray getImages();
    FormDataWire *data;
};

class LabelE4570 : public LabelBase
{
public:
    LabelE4570(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent);
protected:
    virtual QString getCod();
    virtual QByteArray getImages();
    FormDataWire *data;
};

class LabelG100100 : public LabelBase
{
public:
    LabelG100100(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent);
protected:
    virtual QString getCod();
    virtual QByteArray getImages();
    FormDataWire *data;
};

class LabelG100100Pal : public LabelBase
{
public:
    LabelG100100Pal(QString nam, double w, double h, double g, FormDataWire *d, QObject *parent);
protected:
    virtual QString getCod();
    FormDataWire *data;
};

#endif // LABELSWIRE_H
