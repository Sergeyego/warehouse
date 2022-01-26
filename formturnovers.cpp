#include "formturnovers.h"
#include "ui_formturnovers.h"

FormTurnovers::FormTurnovers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTurnovers)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),QDate::currentDate().month(),1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    turnoversModel = new TurnoversModel(this);
    ui->tableView->setModel(turnoversModel);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),this,SLOT(refresh()));
    connect(ui->checkBoxWire,SIGNAL(clicked(bool)),this,SLOT(refresh()));
}

FormTurnovers::~FormTurnovers()
{
    delete ui;
}

void FormTurnovers::refresh()
{
    turnoversModel->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->checkBoxEl->isChecked(),ui->checkBoxWire->isChecked());
    ui->tableView->resizeToContents();
}

void FormTurnovers::save()
{
    ui->tableView->save(tr("Обороты с ")+ui->dateEditBeg->date().toString("dd.MM.yy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yy"),1,true);
}

TurnoversModel::TurnoversModel(QObject *parent) : TableModel(parent)
{

}

void TurnoversModel::refresh(QDate beg, QDate end, bool el, bool wire)
{
    QStringList head;
    head<<"Номенклатура"<<"Наличие на \n начало периода, кг";
    QVector<QVector<QVariant>> data;
    QMultiHash<QString,partInfo> bal;
    Models::instance()->sync1C->getBalance(beg,bal);
    QList<QString> beglist = bal.uniqueKeys();
    for (QString kis : beglist){
        QVector<QVariant> row;
        row.push_back(kis);
        double kvo=0;
        QList <partInfo> plist = bal.values(kis);
        for (partInfo pinfo : plist){
            kvo+=pinfo.kvo;
        }
        row.push_back(kvo);
        data.push_back(row);
    }
    setModelData(data,head);
}
