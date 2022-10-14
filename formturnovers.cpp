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
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),turnoversModel,SLOT(setElEn(bool)));
    connect(ui->checkBoxWire,SIGNAL(clicked(bool)),turnoversModel,SLOT(setWireEn(bool)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

FormTurnovers::~FormTurnovers()
{
    delete ui;
}

void FormTurnovers::refresh()
{
    Models::instance()->relKis->refreshModel();
    turnoversModel->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    ui->tableView->resizeToContents();
}

void FormTurnovers::save()
{
    ui->tableView->save(tr("Обороты с ")+ui->dateEditBeg->date().toString("dd.MM.yy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yy"),1,true,Qt::LandscapeOrientation);
}

TurnoversModel::TurnoversModel(QObject *parent) : TableModel(parent)
{
    el_en=true;
    wire_en=true;
    setDecimal(2);
    QStringList head;
    head<<"Номенклатура"<<"Наличие на \nначало периода, кг"<<"Поступление \nс производства, кг"<<"Возврат \nот потребителей, кг"<<"Поступление \nдругое, кг"
       <<"Возврат \nв цех, кг"<<"Отгрузка \nпотребителю, кг"<<"Расход \nдругое, кг"<<"Коррект./\nинвент.(+/-), кг"<<"Остаток на \nконец периода, кг";
    setHeader(head);
}

void TurnoversModel::refresh(QDate beg, QDate end)
{
    Models::instance()->sync1C->getAcceptanceTurnovers(beg,end,acc);
    Models::instance()->sync1C->getShipTurnovers(beg,end,ship);
    Models::instance()->sync1C->getBalanceAndTurnovers(beg,end,bal);
    recalc();
}

void TurnoversModel::setElEn(bool b)
{
    el_en=b;
    recalc();
}

void TurnoversModel::setWireEn(bool b)
{
    wire_en=b;
    recalc();
}

void TurnoversModel::recalc()
{
    QVector<QVector<QVariant>> data;

    double itogo_beg_kvo=0;
    double itogo_rec_ind=0;
    double itogo_rec_ret=0;
    double itogo_rec_oth=0;
    double itogo_exp_ret=0;
    double itogo_exp_shp=0;
    double itogo_exp_oth=0;
    double itogo_inv=0;
    double itogo_end_kvo=0;

    QAbstractItemModel *kisModel = Models::instance()->relKis->model();

    for (int i=0; i<kisModel->rowCount(); i++){

        QString kis=kisModel->data(kisModel->index(i,0),Qt::EditRole).toString();
        QString t = kisModel->data(kisModel->index(i,2),Qt::EditRole).toString();

        if ((t=="w" && !wire_en) || (t=="e" && !el_en) || (!bal.contains(kis) && !acc.contains(kis) && !ship.contains(kis))){
            continue;
        }

        QVector<QVariant> row;
        double beg_kvo=0;
        double rec_ind=0;
        double rec_ret=0;
        double rec_oth=0;
        double exp_ret=0;
        double exp_shp=0;
        double exp_oth=0;
        double inv=0;
        double end_kvo=0;

        getBal(kis,beg_kvo,end_kvo);
        getAcc(kis,rec_ind,rec_ret,rec_oth);
        getShip(kis,exp_shp,exp_ret,exp_oth);

        inv=beg_kvo+rec_ind+rec_ret+rec_oth-exp_ret-exp_shp-exp_oth-end_kvo;
        inv*=-1;

        row.push_back(kisModel->data(kisModel->index(i,1),Qt::EditRole).toString());
        row.push_back(beg_kvo);
        row.push_back(rec_ind);
        row.push_back(rec_ret);
        row.push_back(rec_oth);
        row.push_back(exp_ret);
        row.push_back(exp_shp);
        row.push_back(exp_oth);
        row.push_back(inv);
        row.push_back(end_kvo);
        data.push_back(row);

        itogo_beg_kvo+=beg_kvo;
        itogo_rec_ind+=rec_ind;
        itogo_rec_ret+=rec_ret;
        itogo_rec_oth+=rec_oth;
        itogo_exp_ret+=exp_ret;
        itogo_exp_shp+=exp_shp;
        itogo_exp_oth+=exp_oth;
        itogo_inv+=inv;
        itogo_end_kvo+=end_kvo;
    }

    QVector<QVariant> itogo_row;
    itogo_row.push_back("Итого");
    itogo_row.push_back(itogo_beg_kvo);
    itogo_row.push_back(itogo_rec_ind);
    itogo_row.push_back(itogo_rec_ret);
    itogo_row.push_back(itogo_rec_oth);
    itogo_row.push_back(itogo_exp_ret);
    itogo_row.push_back(itogo_exp_shp);
    itogo_row.push_back(itogo_exp_oth);
    itogo_row.push_back(itogo_inv);
    itogo_row.push_back(itogo_end_kvo);
    data.push_back(itogo_row);
    setModelData(data);
}

void TurnoversModel::getAcc(QString kis, double &ind, double &ret, double &oth)
{
    QList <accInfo> alist = acc.values(kis);
    for (accInfo a : alist){
        if(a.source=="Приемка из производства"){
            ind+=a.kvo;
        } else if (a.source=="Возврат от клиента"){
            ret+=a.kvo;
        } else {
            oth+=a.kvo;
        }
    }
}

void TurnoversModel::getShip(QString kis, double &shp, double &ret, double &oth)
{
    QList <accInfo> slist = ship.values(kis);
    for (accInfo s : slist){
        if(s.source=="Отгрузка потребителю"){
            shp+=s.kvo;
        } else if (s.source=="Возврат в производство"){
            ret+=s.kvo;
        } else {
            oth+=s.kvo;
        }
    }
}

void TurnoversModel::getBal(QString kis, double &beg, double &end)
{
    QList <turnInfo> tlist = bal.values(kis);
    for (turnInfo tinfo : tlist){
        beg+=tinfo.beg_kvo;
        end+=tinfo.end_kvo;
    }
}
