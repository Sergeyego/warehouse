#ifndef DIALOGREST_H
#define DIALOGREST_H

#include <QDialog>
#include <QDate>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>

namespace Ui {
class DialogRest;
}

class DialogRest : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRest(QWidget *parent = 0);
    ~DialogRest();
    QDate getDate();

public slots:
    void accept();

private:
    Ui::DialogRest *ui;
};

#endif // DIALOGREST_H
