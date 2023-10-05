#ifndef DIALOGREQLOAD_H
#define DIALOGREQLOAD_H

#include <QDialog>
#include "qftp/qftp.h"
#include "db/dbtablemodel.h"
#include "models.h"
#include <QInputDialog>
#include "dialogcods.h"

namespace Ui {
class DialogReqLoad;
}

class ModelEl : public DbTableModel {
    Q_OBJECT
public:
    ModelEl(QWidget *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRow(int row, const QModelIndex &parent);
    void addData(QString code, QString nom, double kvo, QString comment);
    void clearData();
    bool check();
};

class ModelWire : public DbTableModel {
    Q_OBJECT
public:
    ModelWire(QWidget *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRow(int row, const QModelIndex &parent);
    void addData(QString code, QString nom, double kvo, QString comment);
    void clearData();
    bool check();
};

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
    QString xmldata;
    int delay=0;
    bool ftpGet(QString name);
    bool ftpDel(QString name);
    QMap<int, QBuffer*> mapBuffer;
    TableModel *filesModel;
    void parceXml(QIODevice *dev);
    ModelEl *modelEl;
    ModelWire *modelWire;
    void setCurrentPol(QString inn);
    void setCurrentCat(QString cat);
    void HighlightComboBox(QComboBox *combo);
    bool check();
    bool insertNewRequest();
    bool updateRequest(int id);
    void insertChange(int id);
    bool updateRequestEl(int id);
    bool updateRequestWire(int id);
    void syncPol();

private slots:
    void codes1C();
    void updData(QModelIndex index);
    void updateFtpInfo();
    void createTmpTables();
    void ftpConnect();
    void updateList();
    void ftpCommandFinished(int commandId, bool error);
    void ftpCommandStart(int commandId);
    void addToList(const QUrlInfo &urlInfo);
    void clearData();
    void setHighPalette();
    void createPol();
    void loadReq();
};

#endif // DIALOGREQLOAD_H
