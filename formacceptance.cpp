#include "formacceptance.h"
#include "ui_formacceptance.h"

FormAcceptance::FormAcceptance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAcceptance)
{
    ui->setupUi(this);
    sync1C = new Sync1C(this);

    connect(ui->pushButton1C,SIGNAL(clicked(bool)),sync1C,SLOT(syncCatalogEl()));
}

FormAcceptance::~FormAcceptance()
{
    delete ui;
}
