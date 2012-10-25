#ifndef _QGVCONNECTIONMANAGER_H_
#define _QGVCONNECTIONMANAGER_H_

#include "global.h"

class QGVConnectionManager: public QObject
{
    Q_OBJECT

public:
    QGVConnectionManager(QObject *parent = NULL);
    virtual ~QGVConnectionManager();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    Qt_Type_a_susv GetParameters(const QString &Protocol);
    QStringList ListProtocols();
    QString RequestConnection(const QString &Protocol, const QVariantMap &Parameters, QDBusObjectPath &Object_Path);
Q_SIGNALS: // SIGNALS
    void NewConnection(const QString &in0, const QDBusObjectPath &in1, const QString &in2);
};

#endif//_QGVCONNECTIONMANAGER_H_
