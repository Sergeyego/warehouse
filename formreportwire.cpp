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

    modelReport = new TableModel(this);
    QStringList headerList;
    headerList<<tr("Марка проволоки")<<tr("Ф")<<tr("Катушка")<<tr("Налич. на нач.")<<tr("Пост.c пр-ва")<<tr("Возвр.от потр.");
    headerList<<tr("Вернули в цех")<<tr("Отгрузили")<<tr("Ост.на конец");
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
    QString query=QString("select * from "
                          "( "
                          "select pr.nam as nam, d.sdim as dim, k.nam as kat, "
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
                          "group by pr.nam, d.sdim, k.nam "
                          "order by pr.nam, d.sdim, k.nam "
                          ") q "
                          "where (q.beg<>0) or (q.post<>0) or (q.cex<>0) or (q.otgr<>0) or (q.ost<>0)").arg(ui->dateEditBeg->date().addDays(-1).toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));
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
    ui->tableView->save(tit,2,true,Qt::LandscapeOrientation);
}
