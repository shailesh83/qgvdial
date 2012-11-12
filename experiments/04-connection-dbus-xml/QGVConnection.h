#ifndef _QGV_CONNECTION_H_
#define _QGV_CONNECTION_H_

#include "global.h"

enum QGVConnectionStatus {
    QGVCS_Connected = 0,
    QGVCS_Connecting = 1,
    QGVCS_Disconnected = 2
};

class QGVConnection : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString m_dbusObjectPath
               READ getDBusObjectPath)
    Q_PROPERTY(QString m_dbusBusName
               READ getDBusBusName)

    Q_PROPERTY(int m_selfHandle
               READ getSelfHandle
               WRITE setSelfHandle)

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
    QDBusObjectPath RequestChannel(const QString &Type, uint Handle_Type,
                                   uint Handle, bool Suppress_Handler);
    Qt_Type_au RequestHandles(uint Handle_Type, const QStringList &Identifiers);

Q_SIGNALS: // SIGNALS
    void ConnectionError(const QString &in0, const QVariantMap &in1);
    void NewChannel(const QDBusObjectPath &in0, const QString &in1, uint in2,
                    uint in3, bool in4);
    void SelfHandleChanged(uint in0);
    void StatusChanged(uint in0, uint in1);

public:
    QGVConnection(const QString &u, const QString &p, QObject *parent = NULL);
    ~QGVConnection();

    void setSelfHandle(uint h);
    int getSelfHandle();

    bool registerObject();
    void unregisterObject();

    QString getDBusObjectPath();
    QString getDBusBusName();

private:
    uint    m_selfHandle;
    QString m_user, m_pass;
    QString m_dbusObjectPath;
    QString m_dbusBusName;

    QGVConnectionStatus m_connStatus;
};

#endif//_QGV_CONNECTION_H_
