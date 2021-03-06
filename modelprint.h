#ifndef MODELPRINT_H
#define MODELPRINT_H

#include <QObject>
#include "tablemodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ModelPrint : public TableModel
{
    Q_OBJECT
public:
    ModelPrint(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    const QString fileName = "printers.json";

public slots:
    void load();
    void save();
    void newPrint();
};

#endif // MODELPRINT_H
