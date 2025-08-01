#include "formreportwire.h"
#include "ui_formreportwire.h"

FormReportWire::FormReportWire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportWire)
{
    ui->setupUi(this);

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->pushButtonSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    sqlExecutor = new ProgressExecutor(this);

    modelReport = new ModelRepWire(this);
    QStringList headerList;
    headerList<<tr("Марка проволоки")<<tr("Ф")<<tr("Катушка")<<tr("Налич. на нач.")<<tr("План")<<tr("Пост.c пр-ва")<<tr("Возвр.от потр.");
    headerList<<tr("Вернули в цех")<<tr("Отгрузили")<<tr("Ост.на конец")<<tr("Запас");
    modelReport->setHeader(headerList);
    modelReport->setDecimal(2);

    ui->tableView->setModel(modelReport);
    ui->tableView->resizeToContents();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormReportWire::~FormReportWire()
{
    delete ui;
}

void FormReportWire::startUpd()
{
    QString query=QString("select coalesce(q.nam,plr.prov), coalesce(q.dim,plr.dim), coalesce(q.kat,plr.kat), coalesce(q.beg,0), coalesce(plr.kvo,0) as plan, coalesce(q.post,0), coalesce(q.vozv,0), "
                            "coalesce(q.cex,0), coalesce(q.otgr,0), coalesce(q.ost,0), coalesce(q.ost,0)-coalesce(plr.kvo,0) as zap from "
                            "( "
                            "select pr.nam as nam, d.sdim as dim, k.nam||' ('||wp.pack_ed||')' as kat, m.id_provol as id_prov, m.id_diam as id_diam, p.id_pack as id_spool, p.id_pack_type as id_pack, "
                            "sum(c1.st) as beg, sum(c2.sp)-sum(c1.sp) as post, sum(c2.spv)-sum(c1.spv) as vozv, sum(c2.sb)-sum(c1.sb) as cex, sum(c2.so)-sum(c1.so) as otgr, sum(c2.st) as ost "
                            "from wire_parti p "
                            "inner join wire_parti_m as m on p.id_m=m.id "
                            "inner join (select cs.id_wparti, cs.sp, cs.spv, cs.so, cs.sb, cs.st "
                            "from wire_calc_stock('%1') cs) c1 on c1.id_wparti=p.id "
                            "inner join (select cs.id_wparti, cs.sp, cs.spv, cs.so, cs.sb, cs.st "
                            "from wire_calc_stock('%2') cs) c2 on c2.id_wparti=p.id "
                            "inner join provol pr on pr.id=m.id_provol "
                            "inner join diam d on d.id=m.id_diam "
                            "inner join wire_pack_kind k on p.id_pack=k.id "
                            "inner join wire_pack wp on wp.id=p.id_pack_type "
                            "group by pr.nam, d.sdim, k.nam, m.id_provol, m.id_diam, p.id_pack, p.id_pack_type, wp.pack_ed "
                            ") q "
                            "full join( "
                            "select pr.nam as prov, pd.sdim as dim, s.nam||' ('||sp.pack_ed||')' as kat, c.id_mark as id_prov, c.id_diam as id_diam, c.id_src as id_spool, c.id_pack as id_pack, sum(c.ost) as kvo "
                            "from calc_request('%3','%4') as c "
                            "inner join provol pr on pr.id=c.id_mark "
                            "inner join diam pd on pd.id=c.id_diam "
                            "inner join wire_pack_kind s on s.id=c.id_src "
                            "inner join wire_pack sp on sp.id=c.id_pack "
                            "where c.prod='Проволока' group by c.id_mark, c.id_diam, c.id_src, c.id_pack, pr.nam, pd.sdim, s.nam, sp.pack_ed "
                            ") as plr on plr.id_prov=q.id_prov and plr.id_diam=q.id_diam and plr.id_spool=q.id_spool and plr.id_pack=q.id_pack "
                            "where (q.beg<>0) or (q.post<>0) or (q.cex<>0) or (q.otgr<>0) or (q.ost<>0) or (plr.kvo<>0) "
                            "order by q.nam, q.dim, q.nam "
                            ).arg(ui->dateEditBeg->date().addDays(-1).toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd")).arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    sqlExecutor->setQuery(query);
    sqlExecutor->start();
}

void FormReportWire::upd()
{
    QVector<QVector<QVariant>> data=sqlExecutor->getData();
    QVector<QVariant> sums;
    int colCount = modelReport->columnCount();
    sums.resize(colCount);
    sums[0]=tr("Итого");
    for (QVector<QVariant> dt : data){
        for (int i=3; i<colCount; i++){
            sums[i]=sums[i].toDouble()+dt[i].toDouble();
        }
    }
    data.push_back(sums);
    modelReport->setModelData(data);
    ui->tableView->resizeToContents();
}

void FormReportWire::save()
{
    QString tit=tr("Отчет по проволоке ООО СЗСМ с ")+ui->dateEditBeg->date().toString("dd.MM.yyyy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yyyy");
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->save(tit,2,true,Qt::LandscapeOrientation);
    ui->tableView->setColumnHidden(10,false);
}

ModelRepWire::ModelRepWire(QObject *parent) : TableModel(parent)
{

}

QVariant ModelRepWire::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole && index.column()==10){
        return this->data(index,Qt::EditRole).toDouble()<0 ? QVariant(QColor(255,200,100)) : TableModel::data(index,role);
    }
    return TableModel::data(index,role);
}
