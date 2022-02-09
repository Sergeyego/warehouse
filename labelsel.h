#ifndef LABELSEL_H
#define LABELSEL_H

#include <QObject>
#include "labelbase.h"
#include "formdatael.h"

class LabelE5040 : public LabelBase
{
public:
    LabelE5040(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent);
protected:
    virtual QString getCod();
    FormDataEl *data;

};

class LabelG5040 : public LabelBase
{
public:
    LabelG5040(QString nam, double w, double h, double g, FormDataEl *d, QObject *parent);
protected:
    virtual QString getCod();
    FormDataEl *data;
};

#endif // LABELSEL_H
