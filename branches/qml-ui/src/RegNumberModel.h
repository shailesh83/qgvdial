#ifndef REGNUMBERMODEL_H
#define REGNUMBERMODEL_H

#include <QtCore>
#include <QAbstractListModel>

enum RegNumType {
    RNT_Callback,
    RNT_Callout,
};

struct RegNumData {
    RegNumType  type;
    QString     strName;
    QString     strDesc;
};
typedef QList<RegNumData> RegNumDataList;

class RegNumberModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum RegNumberRoles {
        TypeRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
    };

    explicit RegNumberModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool insertRow (RegNumType type,
                    const QString &strName,
                    const QString &strDesc);

private:
    RegNumDataList listRegNums;
};

#endif // REGNUMBERMODEL_H
