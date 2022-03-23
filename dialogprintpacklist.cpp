#include "dialogprintpacklist.h"
#include "ui_dialogprintpacklist.h"

DialogPrintPackList::DialogPrintPackList(QTextDocument *d, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrintPackList), doc(d)
{
    ui->setupUi(this);
    ui->textEdit->setDocument(doc);

    connect(ui->pushButtonClose,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(print()));
}

DialogPrintPackList::~DialogPrintPackList()
{
    delete ui;
}

void DialogPrintPackList::drawDoc(QPainter *painter)
{
    QRect rect = painter->viewport();
    QSize size (rect.size());
    size.scale(doc->size().toSize(),Qt::KeepAspectRatioByExpanding);
    painter->setWindow(0,0,size.width(),size.height());
    doc->drawContents(painter);
}

void DialogPrintPackList::print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if (printDialog.exec()) {
        printer.setPageMargins(QMarginsF(15, 15, 15, 15));
        printer.setPageSize(QPagedPaintDevice::A5);
        printer.setOrientation(QPrinter::Portrait);
        QPainter painter(&printer);
        drawDoc(&painter);
    }
}

bool QRImg::createQr(QImage &image, QString str)
{
    QrEncode qr;
    bool ok=qr.encodeData(0,0,true,-1,str.toUtf8().data());
    const int scale=10;
    int s=1;
    if (ok) s=(qr.size()>0)? qr.size() : 1;
    QImage img(s*scale,s*scale,QImage::Format_RGB32);
    QPainter painter(&img);
    if(ok){
        QColor fg("black");
        QColor bg("white");
        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,s*scale,s*scale);
        painter.setBrush(fg);
        for(int y=0; y<s; y++){
            for(int x=0; x<s; x++){
                if(qr.data(y,x)){
                    const double rx1=x*scale, ry1=y*scale;
                    QRectF r(rx1, ry1, scale, scale);
                    painter.drawRects(&r,1);
                }
            }
        }
    } else {
        QColor error("red");
        painter.setBrush(error);
        painter.drawRect(0,0,scale-1,scale-1);
    }
    image=img;
    return ok;
}

PackElDoc::PackElDoc(FormDataEl *data, QObject *parent) : QTextDocument(parent)
{
    QFont titleFont("Droid Serif",15);
    QFont title2Font("Droid Serif",12);
    QFont normalFont("Droid Sans",12);
    titleFont.setBold(true);
    title2Font.setBold(true);

    QTextBlockFormat formatLeft;
    formatLeft.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    formatLeft.setBottomMargin(20);

    QTextBlockFormat formatCenter;
    formatCenter.setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    QTextCharFormat textTitleFormat;
    textTitleFormat.setFont(titleFont);

    QTextCharFormat textTitle2Format;
    textTitle2Format.setFont(title2Font);

    QTextCharFormat textNormalFormat;
    textNormalFormat.setFont(normalFont);
    textNormalFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);

    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);

    cursor.insertBlock(formatCenter,textTitleFormat);
    cursor.insertText(QString("УПАКОВОЧНЫЙ ЛИСТ").toUpper());
    cursor.insertBlock(formatCenter,textTitle2Format);
    cursor.insertText(QString("электроды покрытые металлические\n").toUpper());
    cursor.insertText(QString("для ручной дуговой сварки\n").toUpper());

    cursor.insertBlock(formatLeft,textNormalFormat);

    cursor.insertText(QString("Марка   "),textNormalFormat);
    cursor.insertText(data->marka(),textTitleFormat);
    cursor.insertText(QString("   Ø   "),textNormalFormat);
    cursor.insertText(data->diametr(),textTitleFormat);
    cursor.insertText(QString(" мм\n"),textNormalFormat);

    cursor.insertText(QString("Партия   "),textNormalFormat);
    cursor.insertText(data->part(),textTitleFormat);
    cursor.insertText(QString("   Дата выпуска   "),textNormalFormat);
    cursor.insertText(data->datePart(),textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertText(QString("Количество мест   "),textNormalFormat);
    cursor.insertText(data->kvoPackPal(),textTitleFormat);
    cursor.insertText(QString("   Масса нетто "),textNormalFormat);
    cursor.insertText(data->masPal(),textTitleFormat);
    cursor.insertText(QString(" кг\n"),textNormalFormat);

    cursor.insertText(QString("Дата упаковки   "),textNormalFormat);
    cursor.insertText(data->datePack(),textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertText(QString("Упаковщик   _______________ "),textNormalFormat);
    cursor.insertText(data->packer(),textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertBlock(formatCenter);

    QImage qr;
    if (QRImg::createQr(qr,data->barCodePack())){
        this->addResource(QTextDocument::ImageResource, QUrl("qrcode"),qr);
        QTextImageFormat qrformat;
        qrformat.setName("qrcode");
        qrformat.setHeight(150);
        cursor.insertImage(qrformat);
    }
}

PackWireDoc::PackWireDoc(FormDataWire *data, QObject *parent) : QTextDocument(parent)
{
    QFont titleFont("Droid Serif",15);
    QFont title2Font("Droid Serif",12);
    QFont normalFont("Droid Sans",12);
    titleFont.setBold(true);
    title2Font.setBold(true);

    QTextBlockFormat formatLeft;
    formatLeft.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    formatLeft.setBottomMargin(18);

    QTextBlockFormat formatCenter;
    formatCenter.setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    QTextCharFormat textTitleFormat;
    textTitleFormat.setFont(titleFont);

    QTextCharFormat textTitle2Format;
    textTitle2Format.setFont(title2Font);

    QTextCharFormat textNormalFormat;
    textNormalFormat.setFont(normalFont);
    textNormalFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);

    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);

    cursor.insertBlock(formatCenter,textTitleFormat);
    cursor.insertText(QString("УПАКОВОЧНЫЙ ЛИСТ").toUpper());
    cursor.insertBlock(formatCenter,textTitle2Format);

    cursor.insertBlock(formatLeft,textNormalFormat);

    cursor.insertText(QString("Марка   "),textNormalFormat);
    cursor.insertText(data->marka(),textTitleFormat);
    cursor.insertText(QString("   Ø   "),textNormalFormat);
    cursor.insertText(data->diametr(),textTitleFormat);
    cursor.insertText(QString(" мм\n"),textNormalFormat);

    cursor.insertText(QString("Плавка   "),textNormalFormat);
    cursor.insertText(data->plavka()+"\n",textTitleFormat);

    cursor.insertText(QString("Партия   "),textNormalFormat);
    cursor.insertText(data->part()+"   ",textTitleFormat);

    cursor.insertText(QString("Тип носителя   "),textNormalFormat);
    cursor.insertText(data->spool()+"\n",textTitleFormat);

    cursor.insertText(QString("Количество кассет   "),textNormalFormat);
    cursor.insertText(data->kvoSpool(),textTitleFormat);
    cursor.insertText(QString("   Масса нетто "),textNormalFormat);
    cursor.insertText(data->masPal(),textTitleFormat);
    cursor.insertText(QString(" кг\n"),textNormalFormat);

    cursor.insertText(QString("Мастер   "),textNormalFormat);
    cursor.insertText(data->master()+"\n",textTitleFormat);

    cursor.insertText(QString("ОТК   "),textNormalFormat);
    cursor.insertText(data->otkNam()+"\n",textTitleFormat);

    cursor.insertText(QString("Дата упаковки   "),textNormalFormat);
    cursor.insertText(data->datePack(),textTitleFormat);
    //cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertBlock(formatCenter);

    QImage qr;
    if (QRImg::createQr(qr,data->barCodePack())){
        this->addResource(QTextDocument::ImageResource, QUrl("qrcode"),qr);
        QTextImageFormat qrformat;
        qrformat.setName("qrcode");
        qrformat.setHeight(150);
        cursor.insertImage(qrformat);
    }

    cursor.insertBlock(formatCenter,textTitleFormat);
    cursor.insertText(QString("\n"));
    cursor.insertText(QString("НЕ БРОСАТЬ!").toUpper());
}
