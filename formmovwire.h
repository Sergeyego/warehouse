#ifndef FORMMOVWIRE_H
#define FORMMOVWIRE_H

#include <QWidget>
#include "progressexecutor.h"
#include "modelro.h"
#include "db/tablemodel.h"

namespace Ui {
class FormMovWire;
}

class ModelNamWire : public ModelRo
{
    Q_OBJECT
public:
    ModelNamWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelPackWire : public ModelRo
{
    Q_OBJECT
public:
    ModelPackWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelPerePackWire : public ModelRo
{
    Q_OBJECT
public:
    ModelPerePackWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelBreakWire : public ModelRo
{
    Q_OBJECT
public:
    ModelBreakWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelStockWire : public ModelRo
{
    Q_OBJECT
public:
    ModelStockWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class ModelShipWire : public ModelRo
{
    Q_OBJECT
public:
    ModelShipWire(QObject *parent=0);
    void refresh(int id_part);
signals:
    void sigSum(QString s);
};

class FormMovWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormMovWire(QWidget *parent = 0);
    ~FormMovWire();

private:
    Ui::FormMovWire *ui;
    void loadSettings();
    void saveSettings();
    ProgressExecutor *executorPart;
    TableModel *modelPart;

    ModelNamWire *modelNamWire;
    ModelPackWire *modelPackWire;
    ModelPerePackWire *modelPerePackWire;
    ModelBreakWire *modelBreakWire;
    ModelStockWire *modelStockWire;
    ModelShipWire *modelShipWire;

private slots:
    void startUpd();
    void upd();
    void updInfo(QModelIndex index);
    void enSavePrim();
    void savePrim();
};

#endif // FORMMOVWIRE_H
