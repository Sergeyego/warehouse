#include "dialogwebview.h"
#include "ui_dialogwebview.h"

DialogWebView::DialogWebView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWebView)
{
    ui->setupUi(this);

    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(print()));
}

DialogWebView::~DialogWebView()
{
    delete ui;
}

void DialogWebView::setUrl(QString url)
{
    ui->webEngineView->setUrl(QUrl(url));
}

void DialogWebView::print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if (printDialog.exec()) {
        printer.setPageMargins(QMarginsF(12, 12, 12, 12));
        printer.setPageSize(QPageSize(QPageSize::A5));
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setResolution(300);
        /*if (single) {
            printer.setCopyCount(1);
        }*/
        PrintHandler handler;
        handler.setPage(ui->webEngineView->page());
        handler.printDocument(&printer);
    }
    accept();
}
