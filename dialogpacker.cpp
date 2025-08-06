#include "dialogpacker.h"
#include "ui_dialogpacker.h"

DialogPacker::DialogPacker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPacker)
{
    ui->setupUi(this);
    modelPacker = new ModelRo(this);
    ui->tableView->setModel(modelPacker);
    upd();
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(printBadge()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updEmpl()));
}

DialogPacker::~DialogPacker()
{
    delete ui;
}

double DialogPacker::getPix(double mm)
{
    //qDebug()<<screen()->devicePixelRatio()<<screen()->logicalDotsPerInch()<<screen()->physicalDotsPerInch();
    return screen()->logicalDotsPerInch()==qreal(96) ? (screen()->devicePixelRatio()*96*mm/25.4) : (screen()->logicalDotsPerInch()*mm/25.4);
}

bool DialogPacker::isSelected()
{
    bool b=false;
    for (int i=0; i<ui->tableView->model()->rowCount();i++){
        if (ui->tableView->selectionModel()->isRowSelected(i,QModelIndex())){
            b=true;
            break;
        }
    }
    return b;
}

void DialogPacker::upd()
{
    QSqlQuery query;
    query.prepare("select ep.fio, ep.job, ep.tabel from el_packer ep");
    if (modelPacker->execQuery(query)){
        modelPacker->setHeaderData(0,Qt::Horizontal,tr("ФИО"));
        modelPacker->setHeaderData(1,Qt::Horizontal,tr("Должность"));
        modelPacker->setHeaderData(2,Qt::Horizontal,tr("Табельный номер"));
        ui->tableView->resizeToContents();
    }
}

void DialogPacker::updEmpl()
{
    QByteArray data;
    if (HttpSyncManager::sendGet("/rab/sync",data)){
        Models::instance()->relMaster->refreshModel();
        Models::instance()->relRabPack->refreshModel();
        upd();
    }
}

void DialogPacker::printBadge()
{
    if (isSelected()){
        QPrinter printer;
        printer.setPageSize(QPageSize(QPageSize::A5));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setFullPage(true);
        QPrintPreviewDialog preview(&printer, this);
        connect(&preview,SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
        preview.exec();
    } else {
        QMessageBox::information(this,tr("Предупреждение"),tr("Не выбрано ни одного работника."),QMessageBox::Ok);
    }
}

void DialogPacker::drawBadge(QPainter *painter, int pos, int row)
{
    QString fio=ui->tableView->model()->data(ui->tableView->model()->index(row,0),Qt::EditRole).toString();
    QString job=ui->tableView->model()->data(ui->tableView->model()->index(row,1),Qt::EditRole).toString();
    QString tabel=ui->tableView->model()->data(ui->tableView->model()->index(row,2),Qt::EditRole).toString();

    QByteArray data;
    HttpSyncManager::sendGet("/barcode/code128/300.png?data="+tabel+"&height=80",data);
    QImage barcode;
    barcode.loadFromData(data);

    painter->save();

    //qDebug()<<pos;

    int xbeg=getPix(15.5);
    int ybeg=-getPix(18)-getPix(56);
    if (pos==1){
        ybeg-=getPix(56);
    } else if (pos==2){
        xbeg+=getPix(89);
    } else if (pos==3){
        xbeg+=getPix(89);
        ybeg-=getPix(56);
    }

    painter->rotate(90);

    QRect frect = QRect(xbeg+getPix(5),ybeg+getPix(5),getPix(79),getPix(20));
    //painter->drawRect(frect);
    painter->setFont(QFont("Droid Sans",16));
    painter->drawText(frect,fio);

    QRect jrect = QRect(xbeg+getPix(5),ybeg+getPix(25),getPix(79),getPix(5));
    //painter->drawRect(jrect);
    painter->setFont(QFont("Droid Sans",10));
    painter->drawText(jrect,job);

    QRect trect = QRect(xbeg+getPix(5),ybeg+getPix(35),getPix(20),getPix(10));
    painter->drawRect(trect);
    painter->setFont(QFont("Droid Sans",20));
    painter->drawText(trect,tabel);

    QRect brect = QRect(xbeg+getPix(50),ybeg+getPix(32),getPix(30),getPix(20));
    //painter->drawRect(brect);
    painter->drawImage(brect,barcode);

    painter->restore();
}


void DialogPacker::print(QPrinter *p)
{
    QList<int> list;
    for (int i=0; i<ui->tableView->model()->rowCount();i++){
        if (ui->tableView->selectionModel()->isRowSelected(i,QModelIndex())){
            list.append(i);
        }
    }

    QPainter painter(p);
    double dx=18;
    double dy=15.5;
    double w=112;
    double h=178;

    int kvo=4;

    int pageCount=list.size()/kvo;
    if (list.size()%kvo){
        pageCount++;
    }

    int n=0;

    for (int m=0; m<pageCount; m++){
        painter.drawRect(getPix(dx),getPix(dy),getPix(w),getPix(h));
        painter.drawLine(getPix(dx),getPix(dy+h/2),getPix(dx+w),getPix(dy+h/2));
        painter.drawLine(getPix(dx+w/2),getPix(dy),getPix(dx+w/2),getPix(dy+h));

        for (int i=0; i<kvo; i++){
            if (list.size()>n){
                drawBadge(&painter,i,list.at(n));
            }
            n++;
        }

        if (m<pageCount-1){
            p->newPage();
        }
    }
}
