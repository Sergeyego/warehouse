#include "dialogprintpacklist.h"
#include "ui_dialogprintpacklist.h"

DialogPrintPackList::DialogPrintPackList(QTextDocument *d, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrintPackList), doc(d)
{
    ui->setupUi(this);
    single=true;

    ui->textEdit->setDocument(doc);

    connect(ui->pushButtonClose,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(print()));
}

DialogPrintPackList::~DialogPrintPackList()
{
    delete ui;
}

void DialogPrintPackList::setSingle(bool b)
{
    single=b;
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
        if (single) {
            printer.setNumCopies(1);
        }
        QPainter painter(&printer);
        drawDoc(&painter);
    }
    accept();
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

bool Code128Img::createCode128(QImage &image, QString str)
{
    Code128::BarCode m_Code = Code128::encode(str);

    float m_Width(250);
    float m_Height(80);

    int m_CodeLength = 0;

    for (int i=0;i<m_Code.length();i++){
        m_CodeLength+=m_Code[i];
    }

    float lineWidth = m_Width / m_CodeLength;

    QImage img(m_Width,m_Height,QImage::Format_RGB32);
    img.fill( Qt::white );
    QPainter painter(&img);
    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    float fontHeight = painter.fontMetrics().height();

    float left = 0;
    for (int i=0;i<m_Code.length();i++){

        float width = m_Code[i] * lineWidth;

        if ( i % 2 == 0 ){
            QRectF bar(left, 0, width, m_Height - fontHeight );
            painter.fillRect(bar, Qt::SolidPattern);
        }

        left+= width;
    }

    QRectF box(0, m_Height - fontHeight , left, fontHeight);
    painter.drawText(box, str, Qt::AlignHCenter | Qt::AlignVCenter);
    image=img;

    return m_Code.length()>0;
}


PackElDoc::PackElDoc(FormDataEl *data, QObject *parent) : QTextDocument(parent)
{
    QString palBarcode;
    QString prefix="E";
    QSqlQuery query;
    query.prepare("insert into pallets (datetime, prefix) values (:datetime, :prefix) returning id");
    query.bindValue(":datetime",QDateTime::currentDateTime());
    query.bindValue(":prefix",prefix);
    if (query.exec()){
        if (query.next()){
            palBarcode=prefix+QString("%1").arg((query.value(0).toInt()),10-prefix.length(),'d',0,QChar('0'));
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    QFont titleFont("Droid Sans",15);
    QFont title2Font("Droid Sans",12);
    QFont normalFont("Droid Sans",12);
    titleFont.setBold(true);
    title2Font.setBold(true);

    QTextBlockFormat formatLeft;
    formatLeft.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    formatLeft.setBottomMargin(14);

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

    QImage code128;
    if (Code128Img::createCode128(code128,palBarcode)){
        this->addResource(QTextDocument::ImageResource, QUrl("code128"),code128);
        QTextImageFormat qrformat;
        qrformat.setName("code128");
        qrformat.setHeight(80);
        cursor.insertImage(qrformat);
    }

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
    if (QRImg::createQr(qr,data->barCodePack()+palBarcode)){
        this->addResource(QTextDocument::ImageResource, QUrl("qrcode"),qr);
        QTextImageFormat qrformat;
        qrformat.setName("qrcode");
        qrformat.setHeight(150);
        cursor.insertImage(qrformat);
    }
}

PackWireDoc::PackWireDoc(FormDataWire *data, QObject *parent) : QTextDocument(parent)
{
    QString palBarcode;
    QString prefix="W";
    QSqlQuery query;
    query.prepare("insert into pallets (datetime, prefix) values (:datetime, :prefix) returning id");
    query.bindValue(":datetime",QDateTime::currentDateTime());
    query.bindValue(":prefix",prefix);
    if (query.exec()){
        if (query.next()){
            palBarcode=prefix+QString("%1").arg((query.value(0).toInt()),10-prefix.length(),'d',0,QChar('0'));
        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    QFont titleFont("Droid Sans",15);
    QFont title2Font("Droid Sans",12);
    QFont normalFont("Droid Sans",12);
    titleFont.setBold(true);
    title2Font.setBold(true);

    QTextBlockFormat formatLeft;
    formatLeft.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    formatLeft.setBottomMargin(14);

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

    QImage code128;
    if (Code128Img::createCode128(code128,palBarcode)){
        this->addResource(QTextDocument::ImageResource, QUrl("code128"),code128);
        QTextImageFormat qrformat;
        qrformat.setName("code128");
        qrformat.setHeight(80);
        cursor.insertImage(qrformat);
    }

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

    QRegExp reg("^L-(\\d+)$");

    if (reg.indexIn(data->spool())==-1){ //обычная этикетка
        cursor.insertText(QString("Тип носителя   "),textNormalFormat);
        cursor.insertText(data->spool()+"\n",textTitleFormat);

        cursor.insertText(QString("Количество кассет   "),textNormalFormat);
        cursor.insertText(data->kvoSpool(),textTitleFormat);
        cursor.insertText(QString("   Масса нетто "),textNormalFormat);
        cursor.insertText(data->masPal(),textTitleFormat);
        cursor.insertText(QString(" кг\n"),textNormalFormat);

        cursor.insertText(QString("Мастер   "),textNormalFormat);
        cursor.insertText(data->master()+"\n",textTitleFormat);
    } else { //длинномер
        cursor.insertText(QString("Длина, мм   "),textNormalFormat);
        cursor.insertText(reg.cap(1)+"\n",textTitleFormat);

        cursor.insertText(QString("Количество мест   "),textNormalFormat);
        cursor.insertText(data->kvoSpool(),textTitleFormat);
        cursor.insertText(QString("   Масса нетто "),textNormalFormat);
        cursor.insertText(data->masPal(),textTitleFormat);
        cursor.insertText(QString(" кг\n"),textNormalFormat);

        cursor.insertText(QString("Упаковщик  _______________________\n"),textNormalFormat);
    }

    cursor.insertText(QString("Дата упаковки   "),textNormalFormat);
    cursor.insertText(data->datePack(),textTitleFormat);

    cursor.insertBlock(formatCenter);

    QImage qr;
    if (QRImg::createQr(qr,data->barCodePack()+palBarcode)){
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

PackNaklDoc::PackNaklDoc(QString kis, QObject *parent) : QTextDocument(parent)
{
    naklInfo info;
    QVector<naklDataInfo> datainfo;

     Models::instance()->sync1C->getNakl(kis,info,datainfo);

    QFont titleFont("Droid Sans",8);
    QFont normalSmallFont("Droid Sans",5);
    QFont normalFont("Droid Sans",8);
    titleFont.setBold(true);

    QTextBlockFormat formatLeft;
    formatLeft.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    QTextBlockFormat formatRight;
    formatRight.setAlignment(Qt::AlignRight|Qt::AlignVCenter);

    QTextBlockFormat formatCenter;
    formatCenter.setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    QTextCharFormat textTitleFormat;
    textTitleFormat.setFont(titleFont);

    QTextCharFormat textNormalSmallFormat;
    textNormalSmallFormat.setFont(normalSmallFont);

    QTextCharFormat textNormalFormat;
    textNormalFormat.setFont(normalFont);
    textNormalFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);

    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);
    cursor.setBlockFormat(formatRight);

    QImage code128;
    if (Code128Img::createCode128(code128,kis)){
        this->addResource(QTextDocument::ImageResource, QUrl("code128"),code128);
        QTextImageFormat qrformat;
        qrformat.setName("code128");
        qrformat.setHeight(55);
        cursor.insertImage(qrformat);
    }

    cursor.insertBlock(formatCenter);

    cursor.insertText(QString("Накладная №   "),textNormalFormat);
    cursor.insertText(kis,textTitleFormat);
    cursor.insertText(QString("    От   "),textNormalFormat);
    cursor.insertText(info.date.toString("dd.MM.yyyy hh:mm:ss"),textTitleFormat);

    cursor.insertBlock(formatLeft);
    cursor.insertText(QString("Кому   "),textNormalFormat);
    cursor.insertText(info.to+"\n",textTitleFormat);

    cursor.insertText(QString("От кого   "),textNormalFormat);
    cursor.insertText(info.from,textTitleFormat);

    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignHCenter);
    QBrush brush = tableFormat.borderBrush();
    brush.setColor(QColor(Qt::gray));
    tableFormat.setBorderBrush(brush);
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(0);

    QTextTable *table = cursor.insertTable(datainfo.size()+2,6,tableFormat);

    cursor=table->cellAt(0,0).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("№"),textTitleFormat);

    cursor=table->cellAt(0,1).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Наименование товара"),textTitleFormat);

    cursor=table->cellAt(0,2).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Партия"),textTitleFormat);

    cursor=table->cellAt(0,3).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Рецептура/плавка"),textTitleFormat);

    cursor=table->cellAt(0,4).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Поддон"),textTitleFormat);

    cursor=table->cellAt(0,5).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Количество, кг"),textTitleFormat);
    int n=1;
    double sum=0;

    for (naklDataInfo data : datainfo){
        cursor=table->cellAt(n,0).firstCursorPosition();
        cursor.setBlockFormat(formatRight);
        cursor.insertText(QString::number(n),textNormalFormat);

        cursor=table->cellAt(n,1).firstCursorPosition();
        cursor.setBlockFormat(formatLeft);
        cursor.insertText(data.name,textNormalFormat);

        cursor=table->cellAt(n,2).firstCursorPosition();
        cursor.setBlockFormat(formatLeft);
        cursor.insertText(data.parti,textNormalFormat);

        cursor=table->cellAt(n,3).firstCursorPosition();
        cursor.setBlockFormat(formatLeft);
        cursor.insertText(data.rcp,textNormalFormat);

        cursor=table->cellAt(n,4).firstCursorPosition();
        cursor.setBlockFormat(formatLeft);
        cursor.insertText(data.barcodecont,textNormalFormat);

        cursor=table->cellAt(n,5).firstCursorPosition();
        cursor.setBlockFormat(formatRight);
        cursor.insertText(QLocale().toString(data.kvo,'f',2),textNormalFormat);
        sum+=data.kvo;
        n++;
    }

    table->mergeCells(n,0,1,5);

    cursor=table->cellAt(n,1).firstCursorPosition();
    cursor.setBlockFormat(formatLeft);
    cursor.insertText(QString("Итого"),textTitleFormat);

    cursor=table->cellAt(n,5).firstCursorPosition();
    cursor.setBlockFormat(formatRight);
    cursor.insertText(QLocale().toString(sum,'f',2),textNormalFormat);

    cursor.movePosition(QTextCursor::End);
    cursor.setBlockFormat(formatLeft);
    cursor.insertBlock();
    cursor.insertBlock();

    cursor.insertText(QString("Сдал  _________________  _________________    Принял _________________  _________________\n"),textNormalFormat);
    cursor.insertText(QString("                        (подпись)             (расшифровка подписи)                                (подпись)              (расшифровка подписи)\n"),textNormalSmallFormat);
    cursor.insertBlock();
    cursor.insertText(QString("Водитель  _________________  _________________\n"),textNormalFormat);
    cursor.insertText(QString("                                      (подпись)           (расшифровка подписи)"),textNormalSmallFormat);

}
