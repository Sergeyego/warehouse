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

void DialogWebView::sendGetReq(QString path)
{
    QByteArray data;
    if (HttpSyncManager::sendGet(path,data)){
        loadDoc(data);
    }
}

void DialogWebView::setSingle(bool s)
{
    single=s;
}

void DialogWebView::loadDoc(const QString &html)
{
    QMap <QString, QByteArray> tmpRes;
    QTextDocument tmpDoc;
    tmpDoc.setHtml(html);
    QTextBlock bl = tmpDoc.begin();
    while(bl.isValid()){
        QTextBlock::iterator it;
        for(it = bl.begin(); !(it.atEnd()); ++it){
            QTextFragment currentFragment = it.fragment();
            if(currentFragment.isValid()){
                if(currentFragment.charFormat().isImageFormat()){
                    QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();
                    QString name=imgFmt.name();
                    if (!tmpRes.contains(name)){
                        QByteArray resp;
                        if (HttpSyncManager::sendGet(name,resp)){
                            tmpRes.insert(name,resp);
                        }
                    }
                }
            }
        }
        bl = bl.next();
    }
    ui->textEdit->document()->clear();
    for (QString res : tmpRes.keys()){
        ui->textEdit->document()->addResource(QTextDocument::ImageResource,res,tmpRes.value(res));
    }
    ui->textEdit->setHtml(html);
}

void DialogWebView::print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if (printDialog.exec()) {
        printer.setPageMargins(QMarginsF(12, 12, 12, 12));
        printer.setPageSize(QPageSize(QPageSize::A5));
        printer.setPageOrientation(QPageLayout::Portrait);
        if (single) {
            printer.setCopyCount(1);
        }
        if (ui->textEdit->document()->size().height()<(ui->textEdit->height()*1.2)){
            QPainter painter(&printer);
            drawDoc(&painter);
        } else {
            ui->textEdit->document()->print(&printer);
        }
    }
    accept();
}

void DialogWebView::drawDoc(QPainter *painter)
{
    QRect rect = painter->viewport();
    QSize size (rect.size());
    size.scale(ui->textEdit->document()->size().toSize(),Qt::KeepAspectRatioByExpanding);
    painter->setWindow(0,0,size.width(),size.height());
    ui->textEdit->document()->drawContents(painter);
}

