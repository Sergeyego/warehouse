#include "formcells.h"
#include "ui_formcells.h"

FormCells::FormCells(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCells)
{
    ui->setupUi(this);
    loadsettings();
    modelCell = new ModelCell(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(modelCell);
    proxyModel->setFilterKeyColumn(0);
    ui->tableView->setModel(proxyModel);

    ui->comboBoxColumn->addItems(modelCell->getHeader());

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->comboBoxColumn,SIGNAL(currentIndexChanged(int)),this,SLOT(setFilterColumn(int)));
    connect(ui->lineEditStr,SIGNAL(textChanged(QString)),proxyModel,SLOT(setFilterFixedString(QString)));
    connect(ui->toolButtonCheckAll,SIGNAL(clicked(bool)),this,SLOT(checkAll()));
    connect(ui->toolButtonUncheckAll,SIGNAL(clicked(bool)),this,SLOT(uncheckAll()));
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(printLbl()));
    connect(ui->radioButtonCell,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->radioButtonCont,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->pushButtonCfgSize,SIGNAL(clicked(bool)),this,SLOT(cfgLblSize()));

    refresh();
}

FormCells::~FormCells()
{
    savesettings();
    delete ui;
}

void FormCells::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    widthLbl = settings.value("widthLbl",50).toDouble();
    heightLbl = settings.value("heightLbl",40).toDouble();
    gapLbl = settings.value("gapLbl",2.5).toDouble();
    ui->radioButtonCont->setChecked(!settings.value("lblIsCell",true).toBool());
    ui->labelLblSize->setText(QString("Этикетка %1 на %2").arg(widthLbl).arg(heightLbl));
}

void FormCells::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("widthLbl",widthLbl);
    settings.setValue("heightLbl",heightLbl);
    settings.setValue("gapLbl",gapLbl);
    settings.setValue("lblIsCell",ui->radioButtonCell->isChecked());
}

void FormCells::refresh()
{
    QStringList head;
    QVector<QVector<QVariant>> data;
    if (ui->radioButtonCell->isChecked()){
        head<<"Ячейка"<<"Штрихкод"<<"Зона"<<"Стеллаж"<<"Позиция"<<"Ярус"<<"Статус ячейки";
        Models::instance()->sync1C->getCells(data);
    } else if (ui->radioButtonCont->isChecked()){
        head<<"Название"<<"Штрихкод"<<"Состояние"<<"Проконтролирован"<<"Упакован";
        Models::instance()->sync1C->getConts(data);
    }
    ui->lineEditStr->clear();
    ui->comboBoxColumn->clear();
    ui->comboBoxColumn->addItems(head);
    modelCell->setModelData(data,head);
    ui->tableView->resizeToContents();
    ui->tableView->setColumnWidth(0,200);
}

void FormCells::setFilterColumn(int col)
{
    proxyModel->setFilterKeyColumn(col);
}

void FormCells::checkAll()
{
    for (int i=0; i<proxyModel->rowCount(); i++){
        proxyModel->setData(proxyModel->index(i,0),Qt::Checked,Qt::CheckStateRole);
    }
}

void FormCells::uncheckAll()
{
    for (int i=0; i<proxyModel->rowCount(); i++){
        proxyModel->setData(proxyModel->index(i,0),Qt::Unchecked,Qt::CheckStateRole);
    }
}

void FormCells::printLbl()
{
    QVector<cellData> data;
    for (int i=0; i<proxyModel->rowCount(); i++){
        if (proxyModel->data(proxyModel->index(i,0),Qt::CheckStateRole).toBool()){
            cellData d;
            d.name=proxyModel->data(proxyModel->index(i,0),Qt::EditRole).toString();
            d.barcode=proxyModel->data(proxyModel->index(i,1),Qt::EditRole).toString();
            data.push_back(d);
        }
    }
    if (data.size()){
        LabelCell l(widthLbl,heightLbl,gapLbl,data);
        l.printLabel();
    }
}

void FormCells::cfgLblSize()
{
    DialogLblSize d(widthLbl,heightLbl,gapLbl);
    if (d.exec()==QDialog::Accepted){
        widthLbl=d.getWidth();
        heightLbl=d.getHeight();
        gapLbl=d.getGap();
        ui->labelLblSize->setText(QString("Этикетка %1 на %2").arg(widthLbl).arg(heightLbl));
    }
}

ModelCell::ModelCell(QObject *parent) : TableModel(parent)
{

}

QVariant ModelCell::data(const QModelIndex &index, int role) const
{
    if (role==Qt::CheckStateRole && index.column()==0){
        bool b = checkMap.value(this->data(index,Qt::EditRole).toString(),false);
        return b? Qt::Checked : Qt::Unchecked;
    }
    return TableModel::data(index,role);
}

bool ModelCell::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::CheckStateRole && index.column()==0){
        checkMap.insert(this->data(index,Qt::EditRole).toString(),value.toBool()? Qt::Checked : Qt::Unchecked);
        emit dataChanged(index,index);
        return true;
    }
    return TableModel::setData(index,value,role);
}

Qt::ItemFlags ModelCell::flags(const QModelIndex &index) const
{
    if (index.column()==0){
        return Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return TableModel::flags(index);
}

void ModelCell::setModelData(const QVector<QVector<QVariant> > &data, const QStringList &hdata)
{
    checkMap.clear();
    return TableModel::setModelData(data,hdata);
}

LabelCell::LabelCell(double w, double h, double g, QVector<cellData> &d, QObject *parent) : LabelBase("LblCell",w,h,g,parent), data(d)
{
    dmSizes[3]=10;
    dmSizes[6]=12;
    dmSizes[10]=14;
    dmSizes[16]=16;
    dmSizes[25]=18;
    dmSizes[31]=20;
    dmSizes[43]=22;
    dmSizes[52]=24;
    dmSizes[64]=26;
    dmSizes[91]=32;
    dmSizes[127]=36;
    dmSizes[169]=40;
    dmSizes[214]=44;
    dmSizes[259]=48;
    dmSizes[304]=52;
    dmSizes[418]=64;
    dmSizes[550]=72;
    dmSizes[682]=80;
    dmSizes[862]=88;
    dmSizes[1042]=96;
    dmSizes[1222]=104;
    dmSizes[1573]=120;
    dmSizes[1954]=132;
    dmSizes[2335]=144;

    setPrintCmdMode(true);
    setCutKvo(data.size());
}

QString LabelCell::getCod()
{
    QString lbl=LabelBase::getCod();
    const double w=getWidth();
    const double h=getHeight();
    const double dm=(h/2.0)-5;
    bool first=true;
    for (cellData d : data){
        if (first){
            first=false;
        } else {
            lbl+=cls();
        }
        int s=getDmSize(d.barcode.size());
        lbl+=block(5,5,w-10,(h/2)-10,d.name,getDots(h*0.2));
        lbl+=dataMatrix((w-dm)/2.0,h/2.0,dm,(dm/s),d.barcode);
        lbl+=print(1);
    }

    return lbl;
}

int LabelCell::getDmSize(int kvo)
{
    int c=kvo;
    while (!dmSizes.contains(c) && c<=2335){
        c++;
    }
    return dmSizes.value(c,22);
}
