#include "formreportel.h"
#include "ui_formreportel.h"

FormReportEl::FormReportEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReportEl)
{
    ui->setupUi(this);

    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->pushButtonSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    sqlExecutor = new ProgressExecutor(this);

    modelReport = new ModelRepEl(this);
    QStringList headerList;
    headerList<<tr("Марка")<<tr("ф")<<tr("Склад на\n нач. пер.")<<tr("План\n пр-ва")<<tr("Поступ.\n с пр-ва")<<tr("В т.ч. за\n посл.\n день");
    headerList<<tr("Поступ.\n от др.\n производ.")<<tr("Возвр.,\n переуп.\n (+/-)")<<tr("Неконд.")<<tr("Итого,\n поступл.");
    headerList<<tr("Отгруз. с\n нач. пер.")<<tr("В т.ч.за\n посл.\n день")<<tr("Остаток\n на кон.\n пер.")<<tr("Цех")<<tr("Запас");
    modelReport->setHeader(headerList);
    modelReport->setDecimal(1);

    ui->tableView->setModel(modelReport);
    ui->tableView->resizeToContents();

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(startUpd()));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(upd()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormReportEl::~FormReportEl()
{
    delete ui;
}

void FormReportEl::startUpd()
{
    QDate begDate = ui->dateEditBeg->date();
    if (begDate.day()!=1) {
        QMessageBox::information(this,tr("Замечание"),tr("Начало периода должно быть первым числом месяца."),QMessageBox::Ok);
        return;
    }
    QDate endDate = ui->dateEditEnd->date();
    QString order;
    if (ui->radioButtonUsov->isChecked()){
        order="e.id_u,";
    }
    QString query = QString("select coalesce(m.el, prod.el), coalesce(m.dim, prod.dim), coalesce(m.ostls,0), coalesce(m.plan,0), "
                            "coalesce(m.suma,0), coalesce(m.sumtd,0), coalesce(m.sumc,0), coalesce(m.sumb,0), coalesce(m.sume,0), "
                            "coalesce(m.sumin,0), coalesce(m.sumot,0), coalesce(m.sumjust,0), coalesce(m.kvors,0), coalesce(prod.kvo,0), "
                            "coalesce(m.kvors,0)+coalesce(prod.kvo,0)-coalesce(m.plan,0)-coalesce(m.sumtd,0) "
                            "from calc_marka_y_new(0,'%1','%2') as m "
                            "full join ( "
                            "    select p.id_el as id_el, (el.marka || CASE WHEN p.id_var<>1 THEN ' /'||ev.nam ||'/' ELSE '' end) as el, "
                            "    p.diam as dim, p.id_var as id_var, sum(pr.ostend) as kvo "
                            "    from calc_prod('%3') as pr "
                            "    inner join parti p on p.id=pr.id_part "
                            "    inner join elrtr el on el.id=p.id_el "
                            "    inner join elrtr_vars ev on ev.id=p.id_var "
                            "    where pr.ostend<>0 "
                            "    group by p.id_el, p.diam, p.id_var, el.marka, ev.nam "
                            ") as prod on prod.id_el=m.id_el and prod.dim=m.dim and prod.id_var=m.id_var "
                            "left join elrtr as e on e.id=coalesce(m.id_el, prod.id_el) "
                            "order by %4 coalesce(m.el, prod.el), coalesce(m.dim, prod.dim)")
                        .arg(begDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd")).arg(endDate.addDays(-1).toString("yyyy-MM-dd")).arg(order);
    sqlExecutor->setQuery(query);
    sqlExecutor->start();
}

void FormReportEl::upd()
{
    QVector<QVector<QVariant>> data=sqlExecutor->getData();
    QVector<QVariant> sums;
    int colCount = modelReport->columnCount();
    sums.resize(colCount);
    sums[0]=tr("Итого");
    for (QVector<QVariant> dt : data){
        for (int i=2; i<colCount; i++){
            sums[i]=sums[i].toDouble()+dt[i].toDouble();
        }
    }
    data.push_back(sums);
    modelReport->setModelData(data);
    ui->tableView->resizeToContents();
}

void FormReportEl::save()
{
    int rows,cols;
    rows=ui->tableView->model()->rowCount();
    cols=ui->tableView->model()->columnCount();

    if (rows*cols>1){
        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();

        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        pageSetup.orientation=XlsxPageSetup::landscape;
        ws->setPageSetup(pageSetup);

        QFont defaultFont("Arial", 10);
        QFont titleFont("Arial", 10);
        titleFont.setBold(true);
        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);

        Format standardFormat;
        standardFormat.setBorderStyle(Format::BorderThin);
        standardFormat.setFont(defaultFont);

        Format headerFormat=strFormat;
        headerFormat.setFont(titleFont);
        headerFormat.setTextWarp(true);
        headerFormat.setHorizontalAlignment(Format::AlignHCenter);
        headerFormat.setVerticalAlignment(Format::AlignVCenter);

        Format edFormat=strFormat;
        edFormat.setHorizontalAlignment(Format::AlignHCenter);
        edFormat.setVerticalAlignment(Format::AlignVCenter);

        Format titleFormat;
        titleFormat.setFont(titleFont);

        QString fnam="Отчет по электродам ООО СЗСМ с "+ui->dateEditBeg->date().toString("dd.MM.yyyy")+" по "+ui->dateEditEnd->date().toString("dd.MM.yyyy");
        ws->writeString(CellReference("A1"),fnam,titleFormat);

        ws->setColumnWidth(1,1,28.9);
        ws->setColumnWidth(2,2,6.05);
        ws->setColumnWidth(3,13,10.5);

        ws->writeString(CellReference("A3"),QString("Марка"),headerFormat);
        ws->writeString(CellReference("B3"),QString("ф"),headerFormat);
        ws->writeString(CellReference("C3"),QString("Склад на начало месяца"),headerFormat);
        ws->writeString(CellReference("D3"),QString("План на месяц"),headerFormat);
        ws->writeString(CellReference("E3"),QString("Поступление на склад"),headerFormat);
        ws->writeString(CellReference("J3"),QString("Итого, поступление за месяц"),headerFormat);
        ws->writeString(CellReference("K3"),QString("Отгрузили"),headerFormat);
        ws->writeString(CellReference("M3"),QString("Остаток на складе на конец месяца"),headerFormat);
        ws->mergeCells(CellRange("E3:I3"),headerFormat);
        ws->mergeCells(CellRange("K3:L3"),headerFormat);

        ws->setRowHeight(4,4,44.6);

        ws->mergeCells(CellRange("A3:A4"),headerFormat);
        ws->mergeCells(CellRange("B3:B4"),headerFormat);
        ws->mergeCells(CellRange("C3:C4"),headerFormat);
        ws->mergeCells(CellRange("D3:D4"),headerFormat);
        ws->writeString(CellReference("E4"),QString("Производство"),headerFormat);
        ws->writeString(CellReference("F4"),QString("В т.ч. за последн. день"),headerFormat);
        ws->writeString(CellReference("G4"),QString("Поступл. от др. производ."),headerFormat);
        ws->writeString(CellReference("H4"),QString("Возврат на склад, переупаковка(+/-)"),headerFormat);
        ws->writeString(CellReference("I4"),QString("Некондиция"),headerFormat);
        ws->mergeCells(CellRange("J3:J4"),headerFormat);
        ws->writeString(CellReference("K4"),QString("С начала месяца"),headerFormat);
        ws->writeString(CellReference("L4"),QString("В т.ч. за последн. день"),headerFormat);
        ws->mergeCells(CellRange("M3:M4"),headerFormat);

        ws->writeBlank(CellReference("A5"),edFormat);
        ws->writeString(CellReference("B5"),QString("мм"),edFormat);
        for (int i=3; i<=13;i++){
            ws->writeString(5,i,QString("тонн"),edFormat);
        }

        int row=6;
        const int begRow=row;

        for (int i=0;i<rows-1;i++){
            for(int j=0;j<cols-2;j++){
                int role=Qt::EditRole;
                QVariant value=ui->tableView->model()->data(ui->tableView->model()->index(i,j),role);
                if (j==0){
                    ws->writeString(i+begRow,j+1,value.toString(),strFormat);
                } else if (j==1){
                    ws->writeNumeric(i+begRow,j+1,value.toDouble(),standardFormat);
                } else {
                    if (value.toDouble()!=0){
                        numFormat.setNumberFormat("0.0000");
                        ws->writeNumeric(i+begRow,j+1,value.toDouble()/1000.0,numFormat);
                    } else {
                        numFormat.setNumberFormat("0.0000");
                        ws->writeBlank(i+begRow,j+1,numFormat);
                    }
                }
            }
            row++;
        }

        ws->writeString(row,1,QString("Итого"),strFormat);
        ws->writeBlank(row,2,strFormat);
        numFormat.setNumberFormat("0.0000");
        ws->write(row,3,QString("=SUM(C%1:C%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,4,QString("=SUM(D%1:D%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,5,QString("=SUM(E%1:E%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,6,QString("=SUM(F%1:F%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,7,QString("=SUM(G%1:G%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,8,QString("=SUM(H%1:H%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,9,QString("=SUM(I%1:I%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,10,QString("=SUM(J%1:J%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,11,QString("=SUM(K%1:K%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,12,QString("=SUM(L%1:L%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,13,QString("=SUM(M%1:M%2)").arg(begRow).arg(row-1),numFormat);

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить документ"),
                                                        dir.path()+"/"+fnam+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

ModelRepEl::ModelRepEl(QObject *parent) : TableModel(parent)
{

}

QVariant ModelRepEl::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole && index.column()==14){
        return this->data(index,Qt::EditRole).toDouble()<0 ? QVariant(QColor(255,200,100)) : TableModel::data(index,role);
    }
    return TableModel::data(index,role);
}
