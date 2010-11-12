#include "RegNumberModel.h"

RegNumberModel::RegNumberModel (QObject *parent)
: QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    setRoleNames(roles);
}//RegNumberModel::RegNumberModel

int
RegNumberModel::rowCount(const QModelIndex & /*parent*/) const
{
    return (listRegNums.count ());
}//RegNumberModel::rowCount

QVariant
RegNumberModel::data(const QModelIndex &index, int role) const
{
    QVariant var;
    do { // Begin cleanup block (not a loop)
        int row = index.row();
        if ((row < 0) || (row > rowCount ())) {
            qWarning ("Requested an index out of range");
            break;
        }

        if (TypeRole == role) {
            var = listRegNums.at(row).type;
            break;
        }
        if (NameRole == role) {
            var = listRegNums.at(row).strName;
            break;
        }
        if (DescriptionRole == role) {
            var = listRegNums.at(row).strDesc;
            break;
        }
    } while (0); // End cleanup block (not a loop)
    return (var);
}//RegNumberModel::data

bool
RegNumberModel::insertRow (RegNumType type,
                           const QString &strName,
                           const QString &strDesc)
{
    int oldcount = listRegNums.count ();
    beginInsertRows (QModelIndex (), oldcount, oldcount);
    RegNumData data;
    data.type    = type;
    data.strName = strName;
    data.strDesc = strDesc;

    listRegNums.append (data);
    endInsertRows ();
    return (true);
}//RegNumberModel::insertRow
