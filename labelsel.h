#ifndef LABELSEL_H
#define LABELSEL_H

#include <QObject>
#include "labelbase.h"
#include "formdatael.h"

class LabelE60150 : public LabelBase
{
public:
    LabelE60150(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent);
protected:
    virtual QString getCod();
    FormDataEl *data;
};

class LabelG60150 : public LabelBase
{
public:
    LabelG60150(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent);
protected:
    virtual QString getCod();
    virtual QByteArray getImages();
    FormDataEl *data;   
};

#endif // LABELSEL_H
