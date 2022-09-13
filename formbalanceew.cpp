#include "formbalanceew.h"
#include "ui_formbalanceew.h"

FormBalanceEW::FormBalanceEW(bool e, bool w, QWidget *parent) :
    QWidget(parent), en_el(e), en_wire(w),
    ui(new Ui::FormBalanceEW)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    ui->tableViewMark->setHidden(ui->radioButtonPart->isChecked());

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

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonMark,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->radioButtonPart,SIGNAL(clicked(bool)),this,SLOT(setByPart()));
    connect(ui->tableViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(selectMark(QModelIndex)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormBalanceEW::~FormBalanceEW()
{
    delete ui;
}

double FormBalanceEW::loadData(QVector<QVector<QVariant> > &data, QMultiMap<QString,double> &map, QSqlQuery &query)
{
    double sum=0;
    int colCount=query.record().count();
    while (query.next()){
        QVector<QVariant> dt;
        for (int i=0; i<colCount; i++){
            dt.push_back(query.value(i));
        }
        QString key=query.value(1).toString()+"#"+query.value(2).toString()+"#"+query.value(3).toString();
        dt.push_back(key);
        double kvo=query.value(colCount-1).toDouble();
        map.insert(key,kvo);
        sum+=kvo;
        data.push_back(dt);
    }
    return sum;
}

void FormBalanceEW::upd()
{
    QVector<QVector<QVariant>> data;
    QVector<QVector<QVariant>> dataMark;
    QMultiMap<QString,double> map;
    double sum=0;

    if (en_el){
        QSqlQuery queryEl;
        queryEl.prepare("select 'e:'||cpn.id_part, e.marka || CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' END, "
                        "cast(p.diam as varchar(3)), ep.pack_ed, p.n_s, date_part('year',p.dat_part)::integer as pye, i.nam, rn.nam, p.prim_prod, cpn.kvo "
                        "from calc_parti_new(:d) as cpn "
                        "inner join parti p on p.id = cpn.id_part "
                        "inner join elrtr e on e.id = p.id_el "
                        "inner join el_pack ep on ep.id = p.id_pack "
                        "inner join istoch i on i.id = p.id_ist "
                        "left join rcp_nam rn  on rn.id = p.id_rcp "
                        "inner join elrtr_vars ev on ev.id = p.id_var "
                        "where cpn.kvo<>0 "
                        "order by e.marka, p.diam, pye, p.n_s");
        queryEl.bindValue(":d",ui->dateEdit->date());
        if (queryEl.exec()){
            sum+=loadData(data,map,queryEl);
        } else {
            QMessageBox::critical(this,tr("Ошибка"),queryEl.lastError().text(),QMessageBox::Cancel);
        }
    }

    if (en_wire){
        QSqlQuery queryWire;
        queryWire.prepare("select 'w:'||p.id, pr.nam, d.sdim, k.short|| "
                          "CASE WHEN wp.id<>0 THEN ' ('||wp.mas_ed||' кг)' ELSE '' END, "
                          "m.n_s, cast(date_part('year',m.dat) as integer) as yer, "
                          "s.nam, pb.n_plav, p.prim_prod, c.st "
                          "from wire_parti p "
                          "inner join wire_parti_m as m on p.id_m=m.id "
                          "inner join provol pr on pr.id=m.id_provol "
                          "inner join (select cs.id_wparti, cs.st from wire_calc_stock(:d) cs) c on c.id_wparti=p.id "
                          "inner join diam d on d.id=m.id_diam "
                          "inner join wire_pack_kind k on p.id_pack=k.id "
                          "inner join wire_source s on m.id_source=s.id "
                          "inner join prov_buht pb on pb.id = m.id_buht "
                          "inner join wire_pack wp on wp.id = p.id_pack_type "
                          "where c.st <>0 "
                          "order by pr.nam, d.sdim, k.nam, yer, m.n_s");
        queryWire.bindValue(":d",ui->dateEdit->date());
        if (queryWire.exec()){
            sum+=loadData(data,map,queryWire);
        } else {
            QMessageBox::critical(this,tr("Ошибка"),queryWire.lastError().text(),QMessageBox::Cancel);
        }
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
    proxyModelPart->setFilterRegularExpression("^"+QRegularExpression::escape(key)+"$");
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
