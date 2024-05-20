#include "dialogwebview.h"
#include "ui_dialogwebview.h"

DialogWebView::DialogWebView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWebView)
{
    ui->setupUi(this);
    ui->textEdit->document()->setUseDesignMetrics(true);
    single=false;
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(print()));
}

DialogWebView::~DialogWebView()
{
    delete ui;
}

bool DialogWebView::sendGetReq(QString path)
{
    QByteArray data;
    bool ok=HttpSyncManager::sendGet(path,data);
    if (ok){
        loadDoc(data);
    }
    return ok;
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
        QPainter painter(&printer);
        if (ui->textEdit->document()->size().height()<(ui->textEdit->height()*1.2)){
            drawDoc(&painter);
        } else {
            //ui->textEdit->document()->print(&printer);
            QRect paintRect=printer.pageLayout().paintRectPixels(printer.resolution());
            QRect rect = QRect(0,0,paintRect.width(),paintRect.height());
            ui->textEdit->document()->setPageSize(rect.size());
            QTextFrameFormat fmt = ui->textEdit->document()->rootFrame()->frameFormat();
            fmt.setMargin(0);
            ui->textEdit->document()->rootFrame()->setFrameFormat(fmt);
            for (int i=0; i<ui->textEdit->document()->pageCount();i++){
                painter.save();
                painter.translate(rect.left(), rect.top() - (i) * rect.height());
                QRectF clip(0, (i) * rect.height(), rect.width(), rect.height());
                ui->textEdit->document()->drawContents(&painter, clip);
                painter.restore();
                if (i!=ui->textEdit->document()->pageCount()-1){
                    printer.newPage();
                }
            }
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

