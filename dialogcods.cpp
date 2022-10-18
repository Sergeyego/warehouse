#include "dialogcods.h"
#include "ui_dialogcods.h"

DialogCods::DialogCods(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCods)
{
    ui->setupUi(this);
    relEl =new DbSqlRelation("elrtr","id","marka",this);
    relEl->setFilter("elrtr.id<>0");
    relDiam = new DbSqlRelation("diam","id","sdim",this);
    relDiam->setFilter("diam.id<>0");
    relElPack = new DbSqlRelation("el_pack_view","id","nam",this);
    relElVar = new DbSqlRelation("elrtr_vars","id","nam",this);
    modelElCods = new DbTableModel("td_keys_el",this);
    modelElCods->addColumn("id_el",tr("Марка"),relEl);
    modelElCods->addColumn("id_diam",tr("Диаметр"),relDiam);
    modelElCods->addColumn("id_var",tr("Вариант"),relElVar);
    modelElCods->addColumn("id_pack",tr("Упаковка (ед./групп.)"),relElPack);
    modelElCods->addColumn("cod",tr("Код"));
    modelElCods->setSort("elrtr.marka, diam.sdim, td_keys_el.id_var");
    modelElCods->select();
    ui->tableViewEl->setModel(modelElCods);
    ui->tableViewEl->setColumnWidth(0,180);
    ui->tableViewEl->setColumnWidth(1,100);
    ui->tableViewEl->setColumnWidth(2,120);
    ui->tableViewEl->setColumnWidth(3,210);
    ui->tableViewEl->setColumnWidth(4,100);

    relWire = new DbSqlRelation("provol","id","nam",this);
    relWire->setFilter("provol.id<>0");
    relSpool = new DbSqlRelation("wire_pack_kind","id","short",this);
    relPack = new DbSqlRelation("wire_pack","id","pack_ed",this);
    modelWireCods = new DbTableModel("td_keys_wire",this);
    modelWireCods->addColumn("id_prov",tr("Марка"),relWire);
    modelWireCods->addColumn("id_diam",tr("Диаметр"),relDiam);
    modelWireCods->addColumn("id_spool",tr("Носитель"),relSpool);
    modelWireCods->addColumn("id_pack",tr("Упаковка"),relPack);
    modelWireCods->addColumn("cod",tr("Код"));

    modelWireCods->setSort("provol.nam, diam.sdim, wire_pack_kind.short, wire_pack.pack_ed");
    modelWireCods->select();
    ui->tableViewWire->setModel(modelWireCods);
    ui->tableViewWire->setColumnWidth(0,180);
    ui->tableViewWire->setColumnWidth(1,100);
    ui->tableViewWire->setColumnWidth(2,150);
    ui->tableViewWire->setColumnWidth(3,180);
    ui->tableViewWire->setColumnWidth(4,100);
}

DialogCods::~DialogCods()
{
    delete ui;
}
