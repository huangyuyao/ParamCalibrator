//When subclassing QAbstractTableModel, you must implement rowCount(), columnCount(), and data(). Default implementations of the index() and parent() functions are provided by QAbstractTableModel. Well behaved models will also implement headerData().
//Editable models need to implement setData(), and implement flags() to return a value containing Qt::ItemIsEditable.

#include <UserPanel.h>
#include <QAbstractTableModel>
#include <QMap>
#include <QString>

class ParamModel : public QAbstractTableModel
{
public:
    ParamModel(vector<string> keys, QObject * parent = 0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void flush(ParamSync& container);
    void update(ParamSync& data);
private:
    vector<double> valueTable;
    vector<QString> typeTable;
    vector<QString> nameTable;
    QMap<QString, int> rowMap;
    ParamSync modifyBuffer;
    bool modifyBufferLock = true;
};
ParamModel::ParamModel(vector<string> keys, QObject *parent) : QAbstractTableModel(parent)
{
    int row = 0;
    nameTable.reserve(keys.size());
    typeTable.reserve(keys.size());
    valueTable.reserve(keys.size());
    for(string& key : keys){
        QStringList list = QString::fromStdString(key).split(QChar('|'));
        QString varname = list.at(0);
        QString vartype = list.at(1);
        nameTable.push_back(varname);
        typeTable.push_back(vartype);
        valueTable.push_back(0);
        rowMap[varname] = row++;
    }
    modifyBufferLock = false;
}

int ParamModel::rowCount(const QModelIndex &parent) const
{
    return valueTable.size();
}

int ParamModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant ParamModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole){
        if(index.column() == 0)
            return QVariant(typeTable[index.row()]);
        else if(index.column() == 1)
            return QVariant(nameTable[index.row()]);
        else if(index.column() == 2)
            return QVariant(valueTable[index.row()]);
    }
    return QVariant();
}

bool ParamModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (value.toString().length()==0) return false;
    if (role == Qt::EditRole){
        while (modifyBufferLock) {}
        modifyBufferLock = true;
        modifyBuffer[nameTable[index.row()].toStdString()] = value.toDouble();
        modifyBufferLock = false;
        return true;
    }
    return false;
}

Qt::ItemFlags ParamModel::flags(const QModelIndex &index) const
{
    if (index.column() == 2)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ParamModel::flush(ParamSync &container)
{
    while(modifyBufferLock){}
    modifyBufferLock = true;
    container.clear();
    container.swap(modifyBuffer);
    modifyBufferLock = false;
}

void ParamModel::update(ParamSync &data)
{
    for(ParamSync::iterator item = data.begin(); item != data.end(); ++item){
        valueTable[rowMap[QString::fromStdString(item->first)]] = item->second;
    }
}

#include <QApplication>
#include <QTableView>

ParamModel * model = NULL;

UserPanel::UserPanel(){}
UserPanel::~UserPanel(){}

void UserPanel::getModifiedBuffer(ParamSync &data)
{
    if(model) model->flush(data);
}

void UserPanel::setParams(ParamSync &data)
{
    if(model) model->update(data);
}

void UserPanel::exec(vector<string> keys)
{
    int argc = 0;
    QApplication a(argc, NULL);
    model = new ParamModel(keys);
    QTableView view;
    view.setModel(model);
    view.show();
    a.exec();
    delete model;
    model = NULL;
}
