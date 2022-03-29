#include "mainwindow.h"
#include <QApplication>
#include "db/dblogin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/icon.ico"));

    QPixmap logo(":/images/simb_labl.png");
    DbLogin d(QObject::tr("Маркировка, отправение на склад и отгрузка"),logo);
    if (d.exec()!=QDialog::Accepted) exit(1);

    QDir dir(":fonts");
    QFileInfoList list = dir.entryInfoList();
    foreach (QFileInfo i, list) {
        QFontDatabase::addApplicationFont(i.absoluteFilePath());
    }

    MainWindow w;
    w.show();

    return a.exec();
}
