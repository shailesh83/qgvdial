#ifndef _QGV_CONNECTION_H_
#define _QGV_CONNECTION_H_

#include "global.h"

class QGVConnection : public QObject
{
    Q_OBJECT
    
public:
    QGVConnection(QObject *parent = NULL);
    ~QGVConnection();
    
public Q_SLOTS: // METHODS
    void AddClientInterest(const QStringList &Tokens);
    void Connect();
    void Disconnect();
    QStringList GetInterfaces();
    QString GetProtocol();
    uint GetSelfHandle();
    uint GetStatus();
    void HoldHandles(uint Handle_Type, const Qt_Type_au &Handles);
    QStringList InspectHandles(uint Handle_Type, const Qt_Type_au &Handles);
    Qt_Type_a_osuu ListChannels();
    void ReleaseHandles(uint Handle_Type, const Qt_Type_au &Handles);
    void RemoveClientInterest(const QStringList &Tokens);
    QDBusObjectPath RequestChannel(const QString &Type, uint Handle_Type, uint Handle, bool Suppress_Handler);
    Qt_Type_au RequestHandles(uint Handle_Type, const QStringList &Identifiers);
Q_SIGNALS: // SIGNALS
    void ConnectionError(const QString &in0, const QVariantMap &in1);
    void NewChannel(const QDBusObjectPath &in0, const QString &in1, uint in2, uint in3, bool in4);
    void SelfHandleChanged(uint in0);
    void StatusChanged(uint in0, uint in1);
};

#endif//_QGV_CONNECTION_H_
