#ifndef DIALOGREQLOAD_H
#define DIALOGREQLOAD_H

#include <QDialog>
#include "qftp/qftp.h"
#include "db/dbtablemodel.h"
#include "models.h"

namespace Ui {
class DialogReqLoad;
}

class DialogReqLoad : public QDialog
{
    Q_OBJECT

public:
    explicit DialogReqLoad(QWidget *parent = nullptr);
    ~DialogReqLoad();

private:
    Ui::DialogReqLoad *ui;
    QFtp *ftpClient;
    QString ftphost;
    QString ftpuser;
    QString ftppassword;
    QString ftppath;
    int delay=0;
    bool ftpGet(QString name);
    QBuffer buffer;
    TableModel *filesModel;
    void parceXml(QIODevice *dev);

private slots:
    void updData(QModelIndex index);
    void updateFtpInfo();
    void ftpConnect();
    void updateList();
    void ftpCommandFinished(int commandId, bool error);
    void ftpCommandStart(int commandId);
    void addToList(const QUrlInfo &urlInfo);
    void clearData();
};

#endif // DIALOGREQLOAD_H
