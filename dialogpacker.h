#ifndef DIALOGPACKER_H
#define DIALOGPACKER_H

#include <QDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include "modelro.h"
#include "httpsyncmanager.h"
#include "models.h"
#include <QScreen>

namespace Ui {
class DialogPacker;
}

class DialogPacker : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPacker(QWidget *parent = nullptr);
    ~DialogPacker();

private:
    Ui::DialogPacker *ui;
    ModelRo *modelPacker;
    double getPix(double mm);
    bool isSelected();

private slots:
    void upd();
    void updEmpl();
    void printBadge();
    void drawBadge(QPainter *painter, int pos, int row);
    void print(QPrinter *p);
};

#endif // DIALOGPACKER_H
