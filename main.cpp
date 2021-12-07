#include "mainwindow.h"
#include <QApplication>
#include "db/dblogin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap logo(":/images/simb_labl.png");
    DbLogin d(QObject::tr("Отправка на склад и отгрузка"),logo);
    if (d.exec()!=QDialog::Accepted) exit(1);

    MainWindow w;
    w.show();

    return a.exec();
}
