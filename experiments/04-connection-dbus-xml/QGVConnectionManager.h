#ifndef _QGVCONNECTIONMANAGER_H_
#define _QGVCONNECTIONMANAGER_H_

#include "global.h"
#include "QGVConnection.h"

typedef QList<QGVConnection *> QGVConnectionList;

class QGVConnectionManager: public QObject
{
    Q_OBJECT

public:
    QGVConnectionManager(QObject *parent = NULL);
    virtual ~QGVConnectionManager();
    
    bool registerObject();
    void unregisterObject();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    Qt_Type_a_susv GetParameters(const QString &Protocol);
    QStringList ListProtocols();
    QString RequestConnection(const QString &Protocol, const QVariantMap &Parameters, QDBusObjectPath &Object_Path);
Q_SIGNALS: // SIGNALS
    void NewConnection(const QString &in0, const QDBusObjectPath &in1, const QString &in2);
    
private:
    QGVConnectionList m_connectionList;
    uint m_connectionHandleCounter;
};

#endif//_QGVCONNECTIONMANAGER_H_
