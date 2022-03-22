#ifndef FORMCELLS_H
#define FORMCELLS_H

#include <QWidget>
#include "models.h"
#include "tablemodel.h"
#include "labelbase.h"
#include "dialoglblsize.h"
#include <QSortFilterProxyModel>

namespace Ui {
class FormCells;
}

struct cellData {
    QString name;
    QString barcode;
};

class LabelCell : public LabelBase
{
public:
    LabelCell(double w, double h, double g, QVector<cellData> &d, QObject *parent = 0);
    QString getCod();
private:
    QVector<cellData> data;
    QMap<int, int> dmSizes;
    int getDmSize(int kvo);
};

class ModelCell : public TableModel
{
    Q_OBJECT
public:
    ModelCell(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setModelData(const QVector<QVector<QVariant> > &data, const QStringList &hdata);

private:
    QMap <QString, bool> checkMap;
};

class FormCells : public QWidget
{
    Q_OBJECT

public:
    explicit FormCells(QWidget *parent = 0);
    ~FormCells();

private:
    Ui::FormCells *ui;
    ModelCell *modelCell;
    QSortFilterProxyModel *proxyModel;
    double widthLbl, heightLbl, gapLbl;
    void loadsettings();
    void savesettings();

private slots:
    void refresh();
    void setFilterColumn(int col);
    void checkAll();
    void uncheckAll();
    void printLbl();
    void cfgLblSize();
};

#endif // FORMCELLS_H
