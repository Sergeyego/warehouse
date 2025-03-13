#ifndef FORMCERT_H
#define FORMCERT_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include "reader.h"
#include "modelro.h"

class ModelDataShip : public ModelRo
{
    Q_OBJECT
public:
    ModelDataShip(QObject *parent);
    void refresh(int id_ship);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    QMap <QString, QString> mapProd;
};

namespace Ui {
class FormCert;
}

class FormCert : public QWidget
{
    Q_OBJECT

public:
    explicit FormCert(QWidget *parent = nullptr);
    ~FormCert();
    void setIdShip(int id);

private:
    Ui::FormCert *ui;
    Reader *reader;
    QString n_s;
    int year;
    ModelDataShip *modelDataShip;

private slots:
    void currentRowChanged(QModelIndex index);
    void printAll();
    void pdfAll();
};

#endif // FORMCERT_H
