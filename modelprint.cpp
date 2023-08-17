#include "modelprint.h"

ModelPrint::ModelPrint(QObject *parent) : TableModel(parent)
{
    QStringList head;
    head<<"Имя"<<"URL"<<"DPI"<<"Яркость";
    setHeader(head);
    load();
}

Qt::ItemFlags ModelPrint::flags(const QModelIndex &/*index*/) const
{
    return Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

bool ModelPrint::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()==3 && role==Qt::EditRole){
        int d = value.toInt();
        if (d<1 || d>15){
            return false;
        }
    }
    return TableModel::setData(index,value,role);
}

void ModelPrint::load()
{
    QFile file(fileName);
    if ( file.open( QIODevice::ReadOnly ) ) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array=doc.array();
        QVector<QVector<QVariant>> data;
        for (QJsonValue v : array){
            QVector<QVariant> row;
            QJsonObject o = v.toObject();
            row.push_back(o.value("name").toString());
            row.push_back(o.value("url").toString());
            row.push_back(o.value("dpi").toInt());
            int density = o.value("density").toInt();
            if (density<1 || density>15){
                density=defaultDensity;
            }
            row.push_back(density);
            data.push_back(row);
        }
        file.close();
        setModelData(data);
    }
}

void ModelPrint::save()
{
    QFile file(fileName);
    if ( file.open( QIODevice::WriteOnly ) ) {
        QJsonArray array;
        for (int i=0; i<rowCount(); i++){
            QJsonObject o;
            o.insert("name",data(index(i,0),Qt::EditRole).toString());
            o.insert("url",data(index(i,1),Qt::EditRole).toString());
            o.insert("dpi",data(index(i,2),Qt::EditRole).toInt());
            o.insert("density",data(index(i,3),Qt::EditRole).toInt());
            array.append(o);
        }
        QJsonDocument doc;
        doc.setArray(array);
        file.write(doc.toJson());
        file.close();
    }
}

void ModelPrint::newPrint(QString adr)
{
    QString nam="new_Printer";
    if (rowCount()){
        nam=data(index(rowCount()-1,0),Qt::EditRole).toString()+"-1";
    }
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    QVector<QVariant> row;
    row.push_back(nam);
    row.push_back(adr);
    row.push_back(200);
    row.push_back(defaultDensity);
    p_d.push_back(row);
    endInsertRows();
}
