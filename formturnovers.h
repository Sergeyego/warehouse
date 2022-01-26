#ifndef FORMTURNOVERS_H
#define FORMTURNOVERS_H

#include <QWidget>
#include "tablemodel.h"
#include "models.h"

namespace Ui {
class FormTurnovers;
}

class TurnoversModel : public TableModel
{
    Q_OBJECT

public:
    TurnoversModel(QObject *parent = nullptr);
    void refresh(QDate beg, QDate end, bool el = true, bool wire = true);

};

class FormTurnovers : public QWidget
{
    Q_OBJECT

public:
    explicit FormTurnovers(QWidget *parent = 0);
    ~FormTurnovers();

private:
    Ui::FormTurnovers *ui;
    TurnoversModel *turnoversModel;
private slots:
    void refresh();
    void save();
};

#endif // FORMTURNOVERS_H
