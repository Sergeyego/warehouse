#ifndef MODELPRINT_H
#define MODELPRINT_H

#include <QObject>
#include "db/tablemodel.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

class ModelPrint : public TableModel
{
    Q_OBJECT
public:
    ModelPrint(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    QString fileName;
    const int defaultDensity = 7;

public slots:
    void load();
    void save();
    void newPrint(QString adr);
};

#endif // MODELPRINT_H
