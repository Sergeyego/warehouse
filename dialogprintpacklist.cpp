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
        printer.setPageMargins(QMarginsF(12, 12, 12, 12));
        printer.setPageSize(QPageSize(QPageSize::A5));
        printer.setPageOrientation(QPageLayout::Portrait);
        if (single) {
            printer.setCopyCount(1);
        }
        if (doc->size().height()<700){
            QPainter painter(&printer);
            drawDoc(&painter);
        } else {
            doc->print(&printer);
        }
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
    QString palBarcode=Models::instance()->createPalBarcode("E");

    PackElDoc::packInfo inf;
    inf.cont=palBarcode;
    inf.marka=data->marka();
    inf.diam=data->diametr();
    inf.part=data->part();
    inf.datePart=data->datePart();
    inf.kvoM=data->kvoPackPal();
    inf.kvo=data->masPal();
    inf.datePack=data->datePack();
    inf.packer=data->packer();
    inf.barcode=data->barCodePack();
    inf.eanGr=data->eanGr();

    createDoc(inf);
}

PackElDoc::PackElDoc(int id_part, double kvo, QString cont, QObject *parent) : QTextDocument(parent)
{
    QSqlQuery query;
    query.prepare("select p.id, e.marka, p.diam, p.n_s, p.dat_part, ep.mass_ed, ee.ean_group "
                  "from parti p "
                  "inner join elrtr e on e.id = p.id_el "
                  "inner join el_pack ep on ep.id = p.id_pack "
                  "left join ean_el ee on ee.id_el = p.id_el and ee.id_diam = (select d.id from diam d where d.diam=p.diam) and ee.id_pack = p.id_pack "
                  "where p.id = :id_part");
    query.bindValue(":id_part",id_part);
    if (query.exec()){
        if (query.next()){
            PackElDoc::packInfo inf;
            inf.cont=cont;
            inf.marka=query.value(1).toString();
            inf.diam=QLocale().toString(query.value(2).toDouble(),'f',1);
            inf.part=query.value(3).toString();
            inf.datePart=query.value(4).toDate().toString("dd.MM.yyyy");
            double mas_ed=query.value(5).toDouble();
            int kvoP=0;
            if (mas_ed>0){
                kvoP=kvo/mas_ed;
            }
            inf.kvoM=QString::number(kvoP);
            inf.kvo=QString::number(kvo);
            inf.datePack=QDate::currentDate().toString("dd.MM.yyyy");
            QString ean_gr=query.value(6).toString();
            inf.eanGr=ean_gr;

            QString barcode;
            QString ean=ean_gr;
            ean.resize(13,' ');
            QString part=query.value(3).toString();
            part.resize(4,' ');
            QString id='e'+query.value(0).toString();
            id.resize(8,'_');
            QString year=QString::number(query.value(4).toDate().year());
            year.resize(4,' ');
            barcode = ean+id+part+'-'+year;
            int ikvoM=kvo*100;
            barcode+=QString("%1").arg(ikvoM,6,'d',0,QChar('0'));
            barcode+=QString("%1").arg(kvoP,4,'d',0,QChar('0'));

            inf.barcode=barcode;

            createDoc(inf);
        }

    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

}

void PackElDoc::createDoc(PackElDoc::packInfo &inf)
{
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
    if (Code128Img::createCode128(code128,inf.cont)){
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
    cursor.insertText(inf.marka,textTitleFormat);
    cursor.insertText(QString("   Ø   "),textNormalFormat);
    cursor.insertText(inf.diam,textTitleFormat);
    cursor.insertText(QString(" мм\n"),textNormalFormat);

    cursor.insertText(QString("Партия   "),textNormalFormat);
    cursor.insertText(inf.part,textTitleFormat);
    cursor.insertText(QString("   Дата выпуска   "),textNormalFormat);
    cursor.insertText(inf.datePart,textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertText(QString("Количество мест   "),textNormalFormat);
    cursor.insertText(inf.kvoM,textTitleFormat);
    cursor.insertText(QString("   Масса нетто "),textNormalFormat);
    cursor.insertText(inf.kvo,textTitleFormat);
    cursor.insertText(QString(" кг\n"),textNormalFormat);

    cursor.insertText(QString("Дата упаковки   "),textNormalFormat);
    cursor.insertText(inf.datePack,textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertText(QString("Упаковщик   _______________ "),textNormalFormat);
    cursor.insertText(inf.packer,textTitleFormat);
    cursor.insertText(QString("\n"),textNormalFormat);

    cursor.insertBlock(formatCenter);

    QString cont=inf.cont;
    if (cont.size()!=10){
        cont.resize(10);
        cont.fill('0');
    }

    QImage qr;
    if (!inf.eanGr.isEmpty() && QRImg::createQr(qr,inf.barcode+cont)){
        this->addResource(QTextDocument::ImageResource, QUrl("qrcode"),qr);
        QTextImageFormat qrformat;
        qrformat.setName("qrcode");
        qrformat.setHeight(150);
        cursor.insertImage(qrformat);
    }
}

PackWireDoc::PackWireDoc(FormDataWire *data, QObject *parent) : QTextDocument(parent)
{
    QString palBarcode=Models::instance()->createPalBarcode("W");

    PackWireDoc::packInfo inf;
    inf.cont=palBarcode;
    inf.marka=data->marka();
    inf.diam=data->diametr();
    inf.part=data->part();
    inf.plavka=data->plavka();
    inf.spool=data->spool();
    inf.kvoM=data->kvoSpool();
    inf.kvo=data->masPal();
    inf.datePack=data->datePack();
    inf.packer=data->master();
    inf.barcode=data->barCodePack();
    inf.eanGr=data->eanGr();

    createDoc(inf);
}

PackWireDoc::PackWireDoc(int id_part, double kvo, QString cont, QObject *parent) : QTextDocument(parent)
{
    QSqlQuery query;
    query.prepare("select wp.id, p.nam, d.diam, wpm.n_s, wpm.dat, wp2.mas_ed, coalesce(we.ean_group, we.ean_ed), wpk.short, pb.n_plav "
                  "from wire_parti wp "
                  "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                  "inner join provol p on p.id = wpm.id_provol "
                  "inner join diam d on d.id = wpm.id_diam "
                  "inner join wire_pack wp2 on wp2.id = wp.id_pack_type "
                  "left join wire_ean we on we.id_prov = wpm.id_provol and we.id_diam = wpm.id_diam and we.id_spool = wp.id_pack and we.id_pack = wp.id_pack_type "
                  "inner join wire_pack_kind wpk on wpk.id = wp.id_pack "
                  "inner join prov_buht pb on pb.id = wpm.id_buht "
                  "where wp.id = :id_part");
    query.bindValue(":id_part",id_part);
    if (query.exec()){
        if (query.next()){
            PackWireDoc::packInfo inf;
            inf.cont=cont;
            inf.marka=query.value(1).toString();
            inf.diam=QLocale().toString(query.value(2).toDouble(),'f',1);
            inf.part=query.value(3).toString();
            double mas_ed=query.value(5).toDouble();
            int kvoP=0;
            if (mas_ed>0){
                kvoP=kvo/mas_ed;
            }
            inf.kvoM=QString::number(kvoP);
            inf.kvo=QString::number(kvo);
            inf.datePack=QDate::currentDate().toString("dd.MM.yyyy");
            QString ean_gr=query.value(6).toString();
            inf.eanGr=ean_gr;

            QString barcode;
            QString ean=ean_gr;
            ean.resize(13,' ');
            QString part=query.value(3).toString();
            part.resize(4,' ');
            QString id='w'+query.value(0).toString();
            id.resize(8,'_');
            QString year=QString::number(query.value(4).toDate().year());
            year.resize(4,' ');
            barcode = ean+id+part+'-'+year;
            int ikvoM=kvo*100;
            barcode+=QString("%1").arg(ikvoM,6,'d',0,QChar('0'));
            barcode+=QString("%1").arg(kvoP,4,'d',0,QChar('0'));

            inf.barcode=barcode;

            inf.spool=query.value(7).toString();
            inf.plavka=query.value(8).toString();

            createDoc(inf);
        }

    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

void PackWireDoc::createDoc(PackWireDoc::packInfo &inf)
{

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
    if (Code128Img::createCode128(code128,inf.cont)){
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
    cursor.insertText(inf.marka,textTitleFormat);
    cursor.insertText(QString("   Ø   "),textNormalFormat);
    cursor.insertText(inf.diam,textTitleFormat);
    cursor.insertText(QString(" мм\n"),textNormalFormat);

    cursor.insertText(QString("Плавка   "),textNormalFormat);
    cursor.insertText(inf.plavka+"\n",textTitleFormat);

    cursor.insertText(QString("Партия   "),textNormalFormat);
    cursor.insertText(inf.part+"   ",textTitleFormat);

    QRegExp reg("^L-(\\d+)$");

    if (reg.indexIn(inf.spool)==-1){ //обычная этикетка
        cursor.insertText(QString("Тип носителя   "),textNormalFormat);
        cursor.insertText(inf.spool+"\n",textTitleFormat);

        cursor.insertText(QString("Количество кассет   "),textNormalFormat);
        cursor.insertText(inf.kvoM,textTitleFormat);
        cursor.insertText(QString("   Масса нетто "),textNormalFormat);
        cursor.insertText(inf.kvo,textTitleFormat);
        cursor.insertText(QString(" кг\n"),textNormalFormat);

        cursor.insertText(QString("Мастер   "),textNormalFormat);
        cursor.insertText(inf.packer+"\n",textTitleFormat);
    } else { //длинномер
        cursor.insertText(QString("Длина, мм   "),textNormalFormat);
        cursor.insertText(reg.cap(1)+"\n",textTitleFormat);

        cursor.insertText(QString("Количество мест   "),textNormalFormat);
        cursor.insertText(inf.kvoM,textTitleFormat);
        cursor.insertText(QString("   Масса нетто "),textNormalFormat);
        cursor.insertText(inf.kvo,textTitleFormat);
        cursor.insertText(QString(" кг\n"),textNormalFormat);

        cursor.insertText(QString("Упаковщик  _______________________\n"),textNormalFormat);
    }

    cursor.insertText(QString("Дата упаковки   "),textNormalFormat);
    cursor.insertText(inf.datePack,textTitleFormat);

    cursor.insertBlock(formatCenter);

    QString cont=inf.cont;
    if (cont.size()!=10){
        cont.resize(10);
        cont.fill('0');
    }

    QImage qr;
    if (!inf.eanGr.isEmpty() && QRImg::createQr(qr,inf.barcode+cont)){
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
    createDoc(info,datainfo);
}

PackNaklDoc::PackNaklDoc(int id_ship, QObject *parent) : QTextDocument(parent)
{
    naklInfo info;
    QVector<naklDataInfo> datainfo;

    QSqlQuery query;
    query.prepare("select st.prefix||date_part('year',sp.dat_vid)||'-'||sp.nom_s, sp.dat_vid, p.naim "
                  "from ship_plan sp "
                  "inner join sert_type st on st.id = sp.id_type "
                  "inner join poluch p on p.id = sp.id_pol "
                  "where sp.id = :id_ship ");
    query.bindValue(":id_ship",id_ship);

    QSqlQuery queryEl;
    queryEl.prepare("select e.marka ||' ф '|| cast(p.diam as varchar(3)), p.n_s ||'-'||date_part('year',p.dat_part), rn.nam, spe.massa "
                    "from ship_plan_el spe "
                    "inner join parti p on p.id = spe.id_part "
                    "inner join elrtr e on e.id = p.id_el "
                    "left join rcp_nam rn on rn.id = p.id_rcp "
                    "where spe.id_sert = :id_ship "
                    "order by spe.id");
    queryEl.bindValue(":id_ship",id_ship);

    QSqlQuery queryWire;
    queryWire.prepare("select p2.nam ||' ф '||cast(d.diam as varchar(3))||' '||wpk.short , wpm.n_s||'-'||date_part('year',wpm.dat) , pb.n_plav, spw.m_netto "
                      "from ship_plan_wire spw "
                      "inner join wire_parti wp on wp.id = spw.id_wparti "
                      "inner join wire_parti_m wpm on wpm.id = wp.id_m "
                      "inner join provol p2 on p2.id = wpm.id_provol "
                      "inner join diam d on d.id = wpm.id_diam "
                      "inner join prov_buht pb on pb.id = wpm.id_buht "
                      "inner join wire_pack_kind wpk on wpk.id = wp.id_pack "
                      "where spw.id_ship = :id_ship "
                      "order by spw.id");
    queryWire.bindValue(":id_ship",id_ship);

    if (query.exec()){
        if (query.next()){
            QDateTime t;
            t.setDate(query.value(1).toDate());
            info.number=query.value(0).toString();
            info.date=t;
            info.from=tr("Склад готовой продукции");
            info.to=query.value(2).toString();

            if (queryEl.exec()){
                while (queryEl.next()){
                    naklDataInfo di;
                    di.name=queryEl.value(0).toString();
                    di.parti=queryEl.value(1).toString();
                    di.rcp=queryEl.value(2).toString();
                    di.kvo=queryEl.value(3).toDouble();
                    datainfo.push_back(di);
                }
            } else {
                QMessageBox::critical(nullptr,tr("Ошибка"),queryEl.lastError().text(),QMessageBox::Ok);
            }

            if (queryWire.exec()){
                while (queryWire.next()){
                    naklDataInfo di;
                    di.name=queryWire.value(0).toString();
                    di.parti=queryWire.value(1).toString();
                    di.rcp=queryWire.value(2).toString();
                    di.kvo=queryWire.value(3).toDouble();
                    datainfo.push_back(di);
                }
            } else {
                QMessageBox::critical(nullptr,tr("Ошибка"),queryWire.lastError().text(),QMessageBox::Ok);
            }

        }
    } else {
        QMessageBox::critical(nullptr,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }

    createDoc(info,datainfo);
}

void PackNaklDoc::createDoc(naklInfo &info, QVector<naklDataInfo> &datainfo)
{
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
    if (Code128Img::createCode128(code128,info.number)){
        this->addResource(QTextDocument::ImageResource, QUrl("code128"),code128);
        QTextImageFormat qrformat;
        qrformat.setName("code128");
        qrformat.setHeight(70);
        cursor.insertImage(qrformat);
    }

    cursor.insertBlock(formatCenter);

    cursor.insertText(QString("Накладная №   "),textNormalFormat);
    cursor.insertText(info.number,textTitleFormat);
    cursor.insertText(QString("    От   "),textNormalFormat);
    QString formatDate = info.date.time().toString("hh:mm:ss")==QString("00:00:00") ? "dd.MM.yyyy" : "dd.MM.yyyy hh:mm:ss";
    cursor.insertText(info.date.toString(formatDate),textTitleFormat);

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
    QVector<QTextLength> v;
    v.push_back(QTextLength(QTextLength::PercentageLength,6));
    v.push_back(QTextLength(QTextLength::VariableLength,24));
    v.push_back(QTextLength(QTextLength::VariableLength,15));
    v.push_back(QTextLength(QTextLength::PercentageLength,20));
    v.push_back(QTextLength(QTextLength::VariableLength,20));
    v.push_back(QTextLength(QTextLength::VariableLength,15));
    tableFormat.setColumnWidthConstraints(v);

    QTextTable *table = cursor.insertTable(datainfo.size()+2,6,tableFormat);

    cursor=table->cellAt(0,0).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("№"),textTitleFormat);

    cursor=table->cellAt(0,1).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Наименование\n товара"),textTitleFormat);

    cursor=table->cellAt(0,2).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Партия"),textTitleFormat);

    cursor=table->cellAt(0,3).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Рецептура/\nплавка"),textTitleFormat);

    cursor=table->cellAt(0,4).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Поддон"),textTitleFormat);

    cursor=table->cellAt(0,5).firstCursorPosition();
    cursor.setBlockFormat(formatCenter);
    cursor.insertText(QString("Колич.,\n кг"),textTitleFormat);
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

    cursor.insertText(QString("Сдал  ________________  _________________    Принял ________________  _________________\n"),textNormalFormat);
    cursor.insertText(QString("                        (подпись)             (расшифровка подписи)                                (подпись)              (расшифровка подписи)\n"),textNormalSmallFormat);
    cursor.insertBlock();
    cursor.insertText(QString("Водитель  _________________  _________________\n"),textNormalFormat);
    cursor.insertText(QString("                                      (подпись)           (расшифровка подписи)"),textNormalSmallFormat);
}
