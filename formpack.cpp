#include "formpack.h"
#include "ui_formpack.h"

FormPack::FormPack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPack)
{
    ui->setupUi(this);
    loadSettings();

    modelPack = new ModelPack(this);
    ui->tableViewPack->setModel(modelPack);
    ui->tableViewPack->setColumnHidden(0,true);
    ui->tableViewPack->setColumnHidden(1,true);
    ui->tableViewPack->setColumnWidth(2,120);
    ui->tableViewPack->setColumnWidth(3,80);
    ui->tableViewPack->setColumnWidth(4,140);
    ui->tableViewPack->setColumnHidden(5,true);
    ui->tableViewPack->setColumnWidth(6,330);
    ui->tableViewPack->setColumnHidden(7,true);
    ui->tableViewPack->setColumnWidth(8,80);
    ui->tableViewPack->setColumnHidden(9,true);
    ui->tableViewPack->setColumnHidden(10,true);

    mapper = new DbMapper(ui->tableViewPack,this);
    mapper->addMapping(ui->dateTimeEdit,2);
    mapper->addMapping(ui->comboBoxCex,3);
    mapper->addMapping(ui->comboBoxRab,4);
    mapper->addMapping(ui->comboBoxOp,5);
    mapper->addMapping(ui->comboBoxPart,6);
    mapper->addMapping(ui->comboBoxPal,7);
    mapper->addMapping(ui->lineEditKvo,8);
    mapper->addMapping(ui->lineEditKvoPack,9);
    mapper->addMapping(ui->comboBoxMaster,10);

    mapper->addLock(ui->pushButtonUpd);
    mapper->addLock(ui->dateEdit);
    mapper->addLock(ui->radioButtonPack);
    mapper->addLock(ui->radioButtonPerePack);
    mapper->addLock(ui->radioButtonTermoPack);
    mapper->addEmptyLock(ui->pushButtonnNakl);
    mapper->addEmptyLock(ui->pushButtonPackList);
    ui->horizontalLayoutMap->insertWidget(0,mapper);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonPack,SIGNAL(clicked(bool)),this,SLOT(updCont()));
    connect(ui->radioButtonPerePack,SIGNAL(clicked(bool)),this,SLOT(updCont()));
    connect(ui->radioButtonTermoPack,SIGNAL(clicked(bool)),this,SLOT(updCont()));
    connect(ui->dateEdit,SIGNAL(userDateChanged(QDate)),this,SLOT(updCont()));
    connect(modelPack,SIGNAL(sigSum(QString)),ui->labelSum,SLOT(setText(QString)));
    connect(ui->pushButtonPackList,SIGNAL(clicked(bool)),this,SLOT(packList()));

    upd();
}

FormPack::~FormPack()
{
    saveSettings();
    delete ui;
}

void FormPack::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("pack_splitter_width").toByteArray());
}

void FormPack::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("pack_splitter_width",ui->splitter->saveState());
}

void FormPack::upd()
{
    ui->dateEdit->setDate(QDate::currentDate());
    updCont();
}

void FormPack::updCont()
{
    int id_src=0;
    if (ui->radioButtonPack->isChecked()){
        id_src=1;
    } else if (ui->radioButtonPerePack->isChecked()){
        id_src=2;
    }
    modelPack->refresh(ui->dateEdit->date(),id_src);
}

void FormPack::packList()
{
    int id = mapper->modelData(mapper->currentIndex(),0).toInt();
    DialogWebView d;
    d.sendGetReq("packlists/elrtr/"+QString::number(id));
    d.exec();
}

ModelPack::ModelPack(QWidget *parent) : DbTableModel("el_pallet_op",parent)
{
    addColumn("id",tr("id"));
    addColumn("id_src",tr("id_src"));
    addColumn("dtm",tr("Время"));
    addColumn("id_cex",tr("Цех"),Models::instance()->relCex);
    addColumn("id_rab",tr("Работник"),Models::instance()->relRabPack);
    addColumn("id_op",tr("Операция"),Models::instance()->relPackOp);
    addColumn("id_parti",tr("Партия"),Models::instance()->relElPart);
    addColumn("id_pallet",tr("Поддон"),Models::instance()->relPallet);
    addColumn("kvo",tr("Кол-во, кг"));
    addColumn("pack_kvo",tr("Кол-во мест, кг"));
    addColumn("id_main_rab",tr("Мастер"),Models::instance()->relMaster);
    setSort("el_pallet_op.dtm");

    connect(this,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
    connect(this,SIGNAL(sigUpd()),this,SLOT(calcSum()));
}

void ModelPack::refresh(QDate dat, int id_src)
{
    setFilter("el_pallet_op.dtm::date = '"+dat.toString("yyyy-MM-dd")+"' and el_pallet_op.id_src = "+QString::number(id_src));
    setDefaultValue(1,id_src);
    setDefaultValue(2,dat);
    select();
}

void ModelPack::calcSum()
{
    double sum=0;
    QString title = "Упаковано";
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,8),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (title + tr(" итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : title;
    emit sigSum(s);
}
