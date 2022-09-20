#include "formshipcons.h"
#include "ui_formshipcons.h"

FormShipCons::FormShipCons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShipCons)
{
    ui->setupUi(this);
    loadsettings();
    ui->pushButtonUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));
    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    ui->comboBoxPolFlt->setModel(Models::instance()->relPol->model());
    ui->comboBoxPolFlt->setModelColumn(Models::instance()->relPol->columnDisplay());
    ui->comboBoxPolFlt->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->comboBoxPolFlt->completer()->setCaseSensitivity(Qt::CaseInsensitive);

    modelShip = new ModelShipCons(this);
    ui->tableViewShip->setModel(modelShip);
    ui->tableViewShip->setColumnHidden(0,true);
    ui->tableViewShip->setColumnWidth(1,55);
    ui->tableViewShip->setColumnWidth(2,70);
    ui->tableViewShip->setColumnWidth(3,350);
    for (int i=4; i<modelShip->columnCount(); i++){
        ui->tableViewShip->setColumnHidden(i,true);
    }

    mapper = new DbMapper(ui->tableViewShip,this);
    ui->horizontalLayoutMapper->insertWidget(1,mapper);

    mapper->addMapping(ui->lineEditNum,1);
    mapper->addMapping(ui->dateEdit,2);
    mapper->addMapping(ui->comboBoxPol,3);
    mapper->addMapping(ui->comboBoxType,4);
    mapper->setDefaultFocus(3);
    mapper->addEmptyLock(ui->tableViewEl);
    mapper->addEmptyLock(ui->tableViewWire);
    mapper->addEmptyLock(ui->pushButtonCods);
    mapper->addEmptyLock(ui->pushButtonXML);
    mapper->addLock(ui->pushButtonUpd);
    mapper->addLock(ui->checkBoxOnly);
    mapper->addLock(ui->comboBoxPolFlt);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updPol()));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),this,SLOT(updShip()));
    connect(ui->comboBoxPolFlt,SIGNAL(currentIndexChanged(int)),this,SLOT(updShip()));
    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentShip(int)));
    //connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));

    updPol();
}

FormShipCons::~FormShipCons()
{
    savesettings();
    delete ui;
}

void FormShipCons::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("ship_cons_splitter_width").toByteArray());
}

void FormShipCons::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("ship_cons_splitter_width",ui->splitter->saveState());
}

void FormShipCons::updShip()
{
    if ((this->sender()==ui->comboBoxPolFlt && ui->checkBoxOnly->isChecked()) || (this->sender()!=ui->comboBoxPolFlt)){
        int id_pol=-1;
        if (ui->checkBoxOnly->isChecked()){
            id_pol=ui->comboBoxPolFlt->model()->data(ui->comboBoxPolFlt->model()->index(ui->comboBoxPolFlt->currentIndex(),0),Qt::EditRole).toInt();
        }
        modelShip->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_pol);
    }
}

void FormShipCons::updPol()
{
    ui->comboBoxPolFlt->blockSignals(true);
    Models::instance()->relPol->refreshModel();
    ui->comboBoxPolFlt->blockSignals(false);

    Models::instance()->modelElPart->setMinDate(ui->dateEditBeg->date().addYears(-4),(sender()==ui->pushButtonUpd));
    Models::instance()->modelWirePart->setMinDate(ui->dateEditBeg->date().addYears(-4),(sender()==ui->pushButtonUpd));

    updShip();
}

void FormShipCons::setCurrentShip(int index)
{
    int id_ship=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,0),Qt::EditRole).toInt();
    QString id_pol=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(index,3),Qt::EditRole).toString();
    ui->lineEditPol->setText(Models::instance()->relPol->data(id_pol,2).toString());
    //modelShipEl->refresh(id_ship);
    //modelShipWire->refresh(id_ship);
}

ModelShipCons::ModelShipCons(QObject *parent) : DbTableModel("sertifikat",parent)
{
    addColumn("id",tr("id"));
    addColumn("nom_s",tr("Номер"));
    addColumn("dat_vid",tr("Дата"));
    addColumn("id_pol",tr("Получатель"),Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),Models::instance()->relShipType);
    setSort("sertifikat.dat_vid, sertifikat.nom_s");
    setDefaultValue(4,1);
}

void ModelShipCons::refresh(QDate beg, QDate end, int id_pol)
{
    QString filter="sertifikat.dat_vid between '"+beg.toString("yyyy-MM-dd")+"' and '"
            +end.toString("yyyy-MM-dd")+"'";
    if (id_pol!=-1){
        filter+=" and sertifikat.id_pol = "+QString::number(id_pol);
    }
    this->setFilter(filter);
    this->select();
}

bool ModelShipCons::insertRow(int row, const QModelIndex &parent)
{
    select();
    int old_num=0;
    if (rowCount()>0) {
        old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
    }
    setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}
