#include "formreqreport.h"
#include "ui_formreqreport.h"

FormReqReport::FormReqReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReqReport)
{
    ui->setupUi(this);
    ui->toolButtonUp->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp)));
    ui->toolButtonDown->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown)));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelReport = new TableModel(this);
    ui->tableView->setModel(modelReport);

    executor = new ProgressExecutor(this);

    addColumn(tr("Продукция"),"prod",true);
    addColumn(tr("Номер заявки"),"num");
    addColumn(tr("Представительство"),"cat");
    addColumn(tr("Получатель"),"pol");
    addColumn(tr("Номенклатура"),"marka",true);
    addColumn(tr("Причина не поставки"),"cause");

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(executor,SIGNAL(finished()),this,SLOT(execFinished()));
    connect(ui->toolButtonUp,SIGNAL(clicked(bool)),this,SLOT(colUp()));
    connect(ui->toolButtonDown,SIGNAL(clicked(bool)),this,SLOT(colDown()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(saveXlsx()));
}

FormReqReport::~FormReqReport()
{
    delete ui;
}

void FormReqReport::addColumn(QString title, QString colName,  bool checked)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(Qt::DisplayRole,title);
    item->setData(Qt::UserRole,colName);
    item->setData(Qt::CheckStateRole,checked ? Qt::Checked : Qt::Unchecked);
    ui->listWidget->addItem(item);
}

void FormReqReport::upd()
{
    QString query="select ";
    QString strCols;
    QString strNamCols;
    for (int i=0; i<ui->listWidget->model()->rowCount();i++){
        if (ui->listWidget->item(i)->checkState()==Qt::Checked){
            if (!strCols.isEmpty()){
                strCols+=", ";
                strNamCols+=", ";
            }
            strCols+=ui->listWidget->item(i)->data(Qt::UserRole).toString();
            strNamCols+=ui->listWidget->item(i)->data(Qt::UserRole).toString()+" as \""+ui->listWidget->item(i)->data(Qt::DisplayRole).toString()+"\"";
        }
    }
    query+=strNamCols;
    if (!strNamCols.isEmpty()){
        query+=", ";
    }
    query+="sum(plan) as \"Запланировано, кг\", sum(cor) as \"Скорректировано, кг\", sum(ship) as \"Отгружено, кг\", sum(ost) as \"Осталось отгрузить, кг\"";
    query+=QString(" from calc_request('%1','%2')" ).arg(ui->dateEditBeg->date().toString("yyyy-MM-dd")).arg(ui->dateEditEnd->date().toString("yyyy-MM-dd"));

    if (ui->radioButtonElrtr->isChecked() ^ ui->radioButtonProvol->isChecked()){
        QString flt = (ui->radioButtonElrtr->isChecked()) ? "Электроды" : "Проволока";
        query+=QString("where prod = '%1'").arg(flt);

    }

    if (!strCols.isEmpty()){
        query+=" group by "+strCols;
        query+=" order by "+strCols;
    }
    //qDebug()<<query;
    executor->setQuery(query);
    executor->start();
}

void FormReqReport::execFinished()
{
    QVector<QVector<QVariant>> data=executor->getData();
    QStringList titles=executor->getTitles();
    QVector<QVariant> sums;
    int colCount= titles.size();
    sums.resize(colCount);
    if (colCount>4){
        sums[0]=tr("ИТОГО");
    }
    for (QVector<QVariant> dt : data){
        for (int i=colCount-4; i<colCount; i++){
            sums[i]=sums[i].toDouble()+dt[i].toDouble();
        }
    }
    if (data.size()>1){
        data.push_back(sums);
    }
    modelReport->setModelData(data,titles);
    ui->tableView->resizeToContents();
}

void FormReqReport::colUp()
{
    QListWidgetItem *sx,*sxm1;
    for(int x=1; x<ui->listWidget->count(); x++){
         if(ui->listWidget->item(x)->isSelected()){
            sxm1 = ui->listWidget->takeItem(x-1);
            sx = ui->listWidget->takeItem(x-1);
            ui->listWidget->insertItem(x-1,sxm1);
            ui->listWidget->insertItem(x-1,sx);
            ui->listWidget->item(x-1)->setSelected(true);
         }
    }
}

void FormReqReport::colDown()
{
    QListWidgetItem *sx,*sxm1;
    for(int x=ui->listWidget->count()-2; x>=0; x--){
         if(ui->listWidget->item(x)->isSelected()){
            sxm1 = ui->listWidget->takeItem(x);
            sx = ui->listWidget->takeItem(x);
            ui->listWidget->insertItem(x,sxm1);
            ui->listWidget->insertItem(x,sx);
            ui->listWidget->item(x+1)->setSelected(true);
         }
    }
}

void FormReqReport::saveXlsx()
{
    QString tit=tr("Отчет по отгрузкам ООО СЗСМ с ")+ui->dateEditBeg->date().toString("dd.MM.yyyy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yyyy");
    ui->tableView->save(tit,2,true,Qt::LandscapeOrientation);
}
