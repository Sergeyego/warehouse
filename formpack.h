#ifndef FORMPACK_H
#define FORMPACK_H

#include <QWidget>
#include "modelro.h"
#include "db/dbtablemodel.h"
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
    void refresh(QDate dat, bool is_pack);

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
    ModelRo *modelDate;
    ModelPack *modelPack;

private slots:
    void upd();
    void updCont();
    void packList();
};

#endif // FORMPACK_H
