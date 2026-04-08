#include "formbalance.h"
#include "ui_formbalance.h"

FormBalance::FormBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBalance)
{
    ui->setupUi(this);
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->pushButtonSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));

    loadSettings();

    progressDialog = new ProgressReportDialog(this);

    manager = new QNetworkAccessManager(this);

    ui->tableViewMark->hide();
    ui->dateEdit->setDate(QDate::currentDate());
    QStringList headerPart, headerMark;

    headerPart<<"t"<<"Номенклатура"<<"Упаковка"<<"Партия"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг"<<"Зона"<<"Ячейка"<<"Поддон"<<"id_part"<<"id_kis";
    headerMark<<"t"<<"Номенклатура"<<"Код КИС"<<"Количество, кг"<<"План приход, кг"<<"План расход, кг";


    modelPart = new TableModel(this);
    modelPart->setHeader(headerPart);
    proxyModelPart = new QSortFilterProxyModel(this);
    proxyModelPart->setSourceModel(modelPart);
    proxyModelPart->setSortRole(Qt::EditRole);
    proxyModelPart->setFilterCaseSensitivity(Qt::CaseSensitive);
    proxyModelPart->setFilterKeyColumn(14);
    ui->tableViewPart->setModel(proxyModelPart);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(13,true);
    ui->tableViewPart->setColumnHidden(14,true);
    ui->tableViewPart->resizeToContents();

    modelMark = new TableModel(this);
    modelMark->setHeader(headerMark);
    proxyModelMark = new QSortFilterProxyModel(this);
    proxyModelMark->setSourceModel(modelMark);
    proxyModelMark->setSortRole(Qt::EditRole);
    proxyModelMark->setFilterCaseSensitivity(Qt::CaseSensitive);

    ui->tableViewMark->setModel(proxyModelMark);
    ui->tableViewMark->setColumnHidden(0,true);
    ui->tableViewMark->setColumnHidden(2,true);
    ui->tableViewMark->resizeToContents();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),this,SLOT(createModelData()));
    connect(ui->checkBoxWire,SIGNAL(clicked(bool)),this,SLOT(createModelData()));
    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->checkBoxOt,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->tableViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updPart(QModelIndex)));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(createPackList()));
}

FormBalance::~FormBalance()
{
    saveSettings();
    delete ui;
}

void FormBalance::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("balance_splitter_width").toByteArray());
}

void FormBalance::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("balance_splitter_width",ui->splitter->saveState());
}

void FormBalance::startUpd()
{
    QDate dat=ui->dateEdit->date();
    bool getall=!ui->checkBoxOt->isChecked();
    QUrl url(Models::instance()->appServer()+"/wms/balance/"+dat.toString("yyyy-MM-dd")+"?getall="+(getall ? "true":"false"));
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(upd()));
    progressDialog->show();
}

void FormBalance::upd()
{
    progressDialog->hide();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+data,QMessageBox::Cancel);
        } else {
            respDoc=QJsonDocument::fromJson(data);
            createModelData();
        }
        reply->deleteLater();
    }
}

void FormBalance::setByPart()
{
    bool by_p=ui->radioButtonPart->isChecked();
    ui->tableViewMark->setHidden(by_p);
    if (by_p){
        proxyModelPart->setFilterFixedString(QString());
        ui->tableViewPart->resizeToContents();
    } else if (ui->tableViewMark->model()->rowCount()){
        ui->tableViewMark->resizeToContents();
        ui->tableViewMark->selectRow(0);
    }
}

void FormBalance::save()
{
    QString title = tr("Наличие на ")+ui->dateEdit->date().toString("dd-MM-yy");
    if (ui->radioButtonPart->isChecked()){
        title+=tr(" по партиям");
        ui->tableViewPart->save(title,2,true);
    } else {
        title+=tr(" по маркам");
        ui->tableViewMark->save(title,2,true);
    }
}

void FormBalance::updPart(QModelIndex index)
{
    QString key = ui->tableViewMark->model()->data(ui->tableViewMark->model()->index(index.row(),2),Qt::EditRole).toString();
    proxyModelPart->setFilterRegularExpression("^"+QRegularExpression::escape(key)+"$");
    if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->resizeToContents();
        ui->tableViewPart->selectRow(0);
    }
}

void FormBalance::createPackList()
{
    int id_part;
    QString prefix;
    double kvo=0;
    QString cont;
    QTableView *tableView = ui->tableViewPart;
    QModelIndex ind=tableView->model()->index(tableView->currentIndex().row(),13);
    if (ind.isValid()){
        id_part=tableView->model()->data(ind,Qt::EditRole).toInt();
        prefix=tableView->model()->data(tableView->model()->index(ind.row(),0),Qt::EditRole).toString();
        kvo=tableView->model()->data(tableView->model()->index(ind.row(),7),Qt::EditRole).toDouble();
        cont=tableView->model()->data(tableView->model()->index(ind.row(),12),Qt::EditRole).toString();
        DialogWebView d;
        if (d.sendGetReq("packlists/old/"+prefix+"/"+cont+"/"+QString::number(id_part)+"/"+QString::number(kvo))){
            d.exec();
        }
    }
}

void FormBalance::createModelData()
{
    if (respDoc.isArray()){
        QVector<QVector<QVariant>> tmpd;
        QMap<QString, celSum> map;
        QJsonArray arr = respDoc.array();
        double sum=0;

        for (const QJsonValue &v : std::as_const(arr)){
            QVector<QVariant> row;
            QJsonObject o = v.toObject();

            QString prefix = o.value("prefix").toString();
            if ((prefix=='e' && !ui->checkBoxEl->isChecked()) || (prefix=='w' && !ui->checkBoxWire->isChecked())){
                continue;
            }

            row.push_back(prefix);
            row.push_back(o.value("name").toString());
            row.push_back(o.value("pack").toString());
            row.push_back(o.value("part").toString());
            row.push_back(o.value("ist").toString());
            row.push_back(o.value("rcpplav").toString());
            row.push_back(o.value("prim").toString());
            row.push_back(o.value("kvo").toDouble());
            row.push_back(o.value("prich").toDouble());
            row.push_back(o.value("rasch").toDouble());
            row.push_back(o.value("zone").toString());
            row.push_back(o.value("cell").toString());
            row.push_back(o.value("cont").toString());
            row.push_back(o.value("id_part").toInt());
            row.push_back(o.value("id_kis").toString());
            tmpd.push_back(row);

            sum+=o.value("kvo").toDouble();

            QString key=o.value("prefix").toString()+'#'+o.value("name").toString()+'#'+o.value("id_kis").toString();
            celSum sm;
            sm.kvo=o.value("kvo").toDouble();
            sm.prich=o.value("prich").toDouble();
            sm.rasch=o.value("rasch").toDouble();

            if (!map.contains(key)){
                map.insert(key,sm);
            } else {
                celSum ols=map.value(key);
                ols.kvo+=sm.kvo;
                ols.prich+=sm.prich;
                ols.rasch+=sm.rasch;
                map[key]=ols;
            }
        }

        QVector<QVector<QVariant>> dataMark;
        for (const QString &mk : map.keys()){
            QVector<QVariant> row;
            QStringList l = mk.split('#');
            if (l.size()==3){
                for (int i=0; i<l.size(); i++){
                    row.push_back(l.at(i));
                }
                celSum s = map.value(mk);
                row.push_back(s.kvo);
                row.push_back(s.prich);
                row.push_back(s.rasch);
                dataMark.push_back(row);
            }
        }

        ui->labelSum->setText("Итого: "+QLocale().toString(sum,'f',2)+" кг");

        modelMark->setModelData(dataMark);
        ui->tableViewMark->resizeToContents();

        modelPart->setModelData(tmpd);
        ui->tableViewPart->resizeToContents();
    }
}
