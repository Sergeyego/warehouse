#ifndef FORMPACK_H
#define FORMPACK_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "db/dbmapper.h"
#include "models.h"
#include "dialogwebview.h"

namespace Ui {
class FormPack;
}

class ModelPack : public DbTableModel
{
    Q_OBJECT
public:
    explicit ModelPack(QWidget *parent = nullptr);
    void refresh(QDate dat, int id_src);

private slots:
    void calcSum();

signals:
    void sigSum(QString s);
};

class FormPack : public QWidget
{
    Q_OBJECT

public:
    explicit FormPack(QWidget *parent = nullptr);
    ~FormPack();

private:
    Ui::FormPack *ui;
    DbMapper *mapper;
    ModelPack *modelPack;
    void loadSettings();
    void saveSettings();
    TableModel *modelMaster;
    int getIdSrc();

private slots:
    void upd();
    void updCont();
    void updMaster();
    void packList();
    void packNakl();
};

#endif // FORMPACK_H
