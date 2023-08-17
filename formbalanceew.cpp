#include "formbalanceew.h"
#include "ui_formbalanceew.h"

FormBalanceEW::FormBalanceEW(bool e, bool w, QWidget *parent) :
    QWidget(parent), ui(new Ui::FormBalanceEW), en_el(e),
    en_wire(w)
{
    ui->setupUi(this);
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->pushButtonSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));

    ui->dateEdit->setDate(QDate::currentDate());

    ui->tableViewMark->setHidden(ui->radioButtonPart->isChecked());

    sqlExecutor = new ProgressExecutor(this);

    modelMark = new TableModel(this);
    modelPart = new TableModel(this);
    modelPart->setDecimal(2);
    modelMark->setDecimal(2);

    QStringList headerPart, headerMark;

    headerPart<<"id"<<"Марка"<<"Диам."<<"Носитель"<<"Партия"<<"Год"<<"Источник"<<"Рецептура/плавка"<<"Комментарий"<<"Количество, кг"<<"key";
    headerMark<<"key"<<"Марка"<<"Диам."<<"Носитель"<<"Количество, кг";

    modelPart->setHeader(headerPart);
    modelMark->setHeader(headerMark);

    proxyModelPart = new QSortFilterProxyModel(this);
    proxyModelPart->setSourceModel(modelPart);
    proxyModelPart->setFilterCaseSensitivity(Qt::CaseSensitive);
    proxyModelPart->setFilterKeyColumn(proxyModelPart->columnCount()-1);

    ui->tableViewMark->setModel(modelMark);
    ui->tableViewPart->setModel(proxyModelPart);

    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(ui->tableViewPart->model()->columnCount()-1,true);
    ui->tableViewMark->setColumnHidden(0,true);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->tableViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(selectMark(QModelIndex)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->tableViewPart,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(edtDesc(QModelIndex)));
}

FormBalanceEW::~FormBalanceEW()
{
    delete ui;
}

void FormBalanceEW::startUpd()
{
    QString query;
    if (en_el){
        query=QString("select 'e:'||cpn.id_part, e.marka || CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' END, "
                      "cast(p.diam as varchar(3)), ep.pack_ed, p.n_s, date_part('year',p.dat_part)::integer as pye, i.nam, rn.nam, p.prim_prod, cpn.kvo, "
                      "e.marka || CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' END||'#'||cast(p.diam as varchar(3))||'#'||ep.pack_ed "
                      "from calc_parti_new('%1') as cpn "
                      "inner join parti p on p.id = cpn.id_part "
                      "inner join elrtr e on e.id = p.id_el "
                      "inner join el_pack ep on ep.id = p.id_pack "
                      "inner join istoch i on i.id = p.id_ist "
                      "left join rcp_nam rn  on rn.id = p.id_rcp "
                      "inner join elrtr_vars ev on ev.id = p.id_var "
                      "where cpn.kvo<>0 "
                      "order by e.marka, p.diam, pye, p.n_s").arg(ui->dateEdit->date().toString("yyyy-MM-dd"));

    } else if (en_wire){
        query=QString("select 'w:'||p.id, pr.nam, d.sdim, k.short|| "
                      "CASE WHEN wp.id<>0 THEN ' ('||wp.mas_ed||' кг)' ELSE '' END, "
                      "m.n_s, cast(date_part('year',m.dat) as integer) as yer, "
                      "s.nam, pb.n_plav, p.prim_prod, c.st, "
                      "pr.nam||'#'||d.sdim||'#'||k.short|| CASE WHEN wp.id<>0 THEN ' ('||wp.mas_ed||' кг)' ELSE '' END "
                      "from wire_parti p "
                      "inner join wire_parti_m as m on p.id_m=m.id "
                      "inner join provol pr on pr.id=m.id_provol "
                      "inner join (select cs.id_wparti, cs.st from wire_calc_stock('%1') cs) c on c.id_wparti=p.id "
                      "inner join diam d on d.id=m.id_diam "
                      "inner join wire_pack_kind k on p.id_pack=k.id "
                      "inner join wire_source s on m.id_source=s.id "
                      "inner join prov_buht pb on pb.id = m.id_buht "
                      "inner join wire_pack wp on wp.id = p.id_pack_type "
                      "where c.st <>0 "
                      "order by pr.nam, d.sdim, k.nam, yer, m.n_s").arg(ui->dateEdit->date().toString("yyyy-MM-dd"));
    }
    if (!query.isEmpty()){
        sqlExecutor->setQuery(query);
        sqlExecutor->start();
    }
}

void FormBalanceEW::upd()
{
    QVector<QVector<QVariant>> data=sqlExecutor->getData();
    QVector<QVector<QVariant>> dataMark;
    QMultiMap<QString,double> map;
    double sum=0;

    for (QVector<QVariant> dt : data){
        double kvo=dt.at(dt.size()-2).toDouble();
        sum+=kvo;
        QString key=dt.at(dt.size()-1).toString();
        map.insert(key,kvo);
    }

    QStringList keys=map.uniqueKeys();
    for (QString key:keys){
        double kvo=0;
        QStringList colums=key.split("#");
        QVector<QVariant> dt;
        dt.push_back(key);
        for (QString col:colums){
            dt.push_back(col);
        }
        QList<double> vals=map.values(key);
        for (double val:vals){
            kvo+=val;
        }
        dt.push_back(kvo);
        dataMark.push_back(dt);
    }

    QVector<QVariant> dt;
    dt.resize(modelPart->columnCount());
    dt[1]=tr("Итого");
    dt[modelPart->columnCount()-2]=sum;
    data.push_back(dt);

    QVector<QVariant> dtm;
    dtm.resize(modelMark->columnCount());
    dtm[1]=tr("Итого");
    dtm[modelMark->columnCount()-1]=sum;
    dataMark.push_back(dtm);

    modelMark->setModelData(dataMark);
    modelPart->setModelData(data);

    ui->labelSum->setText("Итого: "+QLocale().toString(sum,'f',2)+" кг");

    setByPart();
}

void FormBalanceEW::setByPart()
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

void FormBalanceEW::selectMark(QModelIndex index)
{
    QString key = ui->tableViewMark->model()->data(ui->tableViewMark->model()->index(index.row(),0),Qt::EditRole).toString();
    proxyModelPart->setFilterRegExp("^"+QRegularExpression::escape(key)+"$");
    ui->tableViewPart->resizeToContents();
}

void FormBalanceEW::save()
{
    QString title = tr("Наличие ");
    if (en_el){
        title+=tr("электродов");
    }
    if (en_wire){
        if (en_el){
            title+=tr(", ");
        }
        title+=tr("проволоки");
    }
    title+=tr(" на ")+ui->dateEdit->date().toString("dd.MM.yy");
    if (ui->radioButtonPart->isChecked()){
        title+=tr(" по партиям");
        ui->tableViewPart->save(title,2,true);
    } else {
        title+=tr(" по маркам");
        ui->tableViewMark->save(title,2,true);
    }
}

void FormBalanceEW::edtDesc(QModelIndex index)
{
    QString key = ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),ui->tableViewPart->model()->columnCount()-1),Qt::EditRole).toString();
    if (key.isEmpty()){
        return;
    }
    key=key.replace("#"," ");
    QString descr = ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),8),Qt::EditRole).toString();
    QString part = ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),4),Qt::EditRole).toString();
    bool ok;
    QString text = QInputDialog::getText(this, tr("Введите комментарий"),key+tr(" партия № ")+part, QLineEdit::Normal,descr, &ok);
    if (ok){
        QString id = ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toString();
        QStringList idl=id.split(":");
        if (idl.size()>1){
            QString pref=idl.at(0);
            int id_part=idl.at(1).toInt();
            QSqlQuery query;
            if (pref=="e"){
                query.prepare("update parti set prim_prod = :text where id = :id");
            } else if (pref=="w"){
                query.prepare("update wire_parti set prim_prod = :text where id = :id");
            }
            if (!query.executedQuery().isEmpty()){
                query.bindValue(":text",text);
                query.bindValue(":id",id_part);
                if (query.exec()){
                    QModelIndex descIndex = ui->tableViewPart->model()->index(index.row(),8);
                    ui->tableViewPart->model()->setData(descIndex,text,Qt::EditRole);
                } else {
                    QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
                }
            }
        }
    }
}
