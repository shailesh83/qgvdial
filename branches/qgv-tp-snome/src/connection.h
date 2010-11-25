/*
 * Telepathy SNOM VoIP phone connection manager
 * Copyright (C) 2006 by basyskom GmbH
 *  @author Tobias Hunger <info@basyskom.de>
 *
 * This library is free software; you can redisQObject::tribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is disQObject::tributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin SQObject::treet, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SNOM_CONNECTION_H_
#define _SNOM_CONNECTION_H_

#include "connectiontypes.h"

#include <QtDBus/QDBusContext>

class ConnectionPrivate;

class PhoneConnector;

class Connection : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit Connection(PhoneConnector * const,
                        const QString & account,
                        QObject * parent = 0);
    ~Connection();

    QString name() const;
    QString serviceName() const;
    QDBusObjectPath objectPath() const;

    bool registerObject();
    void unregisterObject();

    bool handlesAccount(const QString &) const;

    void incomingCall(const uint local_handle, const uint remote_handle);
    void outgoingCall(const uint local_handle, const uint remote_handle);
    void setConnected(const uint local_handle, const uint remote_handle,
                      const bool);
    void setDND(const bool);

    uint findHandle(const QString &);

    PhoneConnector * connector() const;

    enum Status
    {
        Connected = 0,
        Connecting = 1,
        Disconnected = 2
    };

    enum Reason
    {
        ReasonNone = 0,
        ReasonRequested = 1,
        ReasonNetworkError = 2,
        ReasonAuthenticationFailed = 3,
        ReasonEncryptionError = 4,
        ReasonNameInUse = 5,
        ReasonCertNotProvided = 6,
        ReasonCertUntrusted = 7,
        ReasonCertExpired = 8,
        ReasonCertNotActivated = 9,
        ReasonCertHostnameMismatch = 10,
        ReasonCertFingerprintMismatch = 11,
        ReasonCertSelfSigned = 12,
        ReasonCertOtherError = 13
    };

    enum Handle
    {
        HandleNone = 0,
        HandleContact = 1,
        HandleRoom = 2,
        HandleRoomList = 3,
        HandleGroup = 4
    };

public slots:
    void Connect();
    void Disconnect();

    QStringList GetInterfaces();
    QString GetProtocol();
    uint GetStatus();

    uint GetSelfHandle();
    QList<uint> RequestHandles(const uint handle_type, const QStringList &names);
    void HoldHandles(const uint handle_type, const QList<uint> &handles);
    QStringList InspectHandles(const uint handle_type, const QList<uint> &handles);
    void ReleaseHandles(const uint handle_type, const QList<uint> &handles);

    org::freedesktop::Telepathy::ChannelInfoList ListChannels();
    QDBusObjectPath RequestChannel(const QString &type,
                                   uint handle_type, uint handle,
                                   bool suppress_handler);

    // Connection.Interface.Privacy:
    QString GetPrivacyMode();
    QStringList GetPrivacyModes();
    void SetPrivacyMode(const QString &mode);

signals:
    void NewChannel(const QDBusObjectPath & object_path,
                    const QString & channel_type,
                    uint handle_type, uint handle,
                    bool suppress_handler);
    void StatusChanged(uint status, uint reason);

    // Connection.Interface.Privacy:
    void PrivacyModeChanged(const QString &mode);

private slots:
    void doChannelClosed();

private:
    QString getPrivacyModeString() const;

    uint findHandleInternal(const QString &) const;

    Connection(const Connection &); // no impl.

    ConnectionPrivate * const d;
};

#endif
