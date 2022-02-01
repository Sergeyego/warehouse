#ifndef DIALOGOLAPFLT_H
#define DIALOGOLAPFLT_H

#include <QDialog>
#include "olapmodel.h"
#include <QListWidget>

namespace Ui {
class DialogOlapFlt;
}

class DialogOlapFlt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOlapFlt(ProxyDataModel *m, QWidget *parent = 0);
    ~DialogOlapFlt();

private:
    Ui::DialogOlapFlt *ui;
    ProxyDataModel *proxyModel;
};

#endif // DIALOGOLAPFLT_H
