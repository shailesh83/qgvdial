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

#include "phoneconnector.h"

#include "connection.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtNetwork/QHttpResponseHeader>
#include <QtNetwork/QHttp>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

namespace
{
    bool isTrue(const QString & value)
    {
        return (value.compare(QString("on"), Qt::CaseInsensitive) == 0 ||
                value.compare(QString("true"), Qt::CaseInsensitive) == 0 ||
                value == "1");
    }

    class Functor
    {
    public:
        virtual ~Functor() { }
        virtual void operator()(Connection *) const = 0;
    };

    class DNDFunctor : public Functor
    {
    public:
        DNDFunctor(const bool s) :
            state(s)
        { }

        void operator()(Connection * conn) const
        { conn->setDND(state); }

    protected:
        const bool state;
    };

    class CallFunctor : public Functor
    {
    public:
        CallFunctor(const QString & l, const QString & r) :
            local(l), remote(r)
        { }
        virtual ~CallFunctor() { }

        uint getHandle(Connection * conn, const QString & s) const
        {
            qDebug() << "CallFunctor...";
            return conn->findHandle(s);
        }
    protected:
        const QString local;
        const QString remote;
    };

    class IncomingCallFunctor : public CallFunctor
    {
    public:
        IncomingCallFunctor(const QString & l, const QString & r) :
            CallFunctor(l, r)
        { }

        void operator()(Connection * conn) const
        { conn->incomingCall(getHandle(conn, local), getHandle(conn, remote)); }
    };

    class OutgoingCallFunctor : public CallFunctor
    {
    public:
        OutgoingCallFunctor(const QString & l, const QString & r) :
            CallFunctor(l, r)
        { }

        void operator()(Connection * conn) const
        { conn->outgoingCall(getHandle(conn, local), getHandle(conn, remote)); }
    };

    class ConnectedFunctor : public CallFunctor
    {
    public:
        ConnectedFunctor(const QString & l, const QString & r, const bool s) :
            CallFunctor(l, r),
            state(s)
        { }

        void operator()(Connection * conn) const
        {
            conn->setConnected(getHandle(conn, local), getHandle(conn, remote),
                               state);
        }

    protected:
        const bool state;
    };
}

class PhoneConnectorPrivate
{
public:
    PhoneConnectorPrivate(PhoneConnector * const p,
                          const QString & serv, const uint prt,
                          const uint listen_prt) :
        parent(p),
        server(serv), port(prt), listenPort(listen_prt),
        protocol(serv, port),
        notificationServer(new QTcpServer(p)),
        currentId(-1)
    {
        Q_ASSERT(0 != parent);
        Q_ASSERT(0 != notificationServer);
        QObject::connect(notificationServer, SIGNAL(newConnection()),
                         parent, SLOT(doHandleNotification()));
    }

    ~PhoneConnectorPrivate()
    { Q_ASSERT(notificationSockets.isEmpty()); }

    Connection * findConnection(const QString & local) const
    {
        Connection * conn;
        foreach (conn, connectionList)
        {
            if (conn->handlesAccount(local))
            { return conn; }
        }
        return 0;
    }

    void notifySingleConnection(const QString & account, const Functor & op)
    {
        Connection * conn = findConnection(account);
        if (conn == 0)
        {
            qDebug() << "PCP: call not associated with any connection:"
                     << account;
            return;
        }

        op(conn);
    }

    void notifyAllConnections(const Functor & op)
    {
        Connection * conn;
        foreach (conn, connectionList)
        { op(conn); }
    }

    void callDetected(const QHash<QString, QString>& data)
    {
        if (!data.contains("dir") ||
            !data.contains("remote") ||
            !data.contains("local"))
        { return; }

        QString dir = data.value("dir");

        QString local = data.value("local");
        local = local.replace("%40", "@");

        QString remote = data.value("remote");
        remote = remote.replace("%40", "@");

        if (dir.isEmpty() || local.isEmpty() || remote.isEmpty())
        { return; }

        if (dir == "incoming")
        { notifySingleConnection(local, IncomingCallFunctor(local, remote)); }
        else if (dir == "outgoing")
        { notifySingleConnection(local, OutgoingCallFunctor(local, remote)); }
        else
        {
            qWarning() << "Unknown direction" << dir
                    << "found when detecting a call.";
        }
    }

    void settingChanged(const QHash<QString, QString>& data)
    {
        if (!data.contains("key") ||
            !data.contains("value"))
        { return; }

        QString key(data.value("key"));
        QString value(data.value("value"));
        QString local(data.value("local"));
        QString remote(data.value("remote"));

        qDebug() << "Setting: key:" << key
                 << "value:" << value
                 << "remote:" << remote
                 << "local:" << local;

        if (key == "connected")
        {
            if (local.isEmpty() || remote.isEmpty()) { return; }
            notifySingleConnection(local, ConnectedFunctor(local, remote,
                                   isTrue(value)));
        }
        else if (key == "dnd")
        { notifyAllConnections(DNDFunctor(isTrue(value))); }
        else
        {
            qWarning() << "Unknown key" << key
                    << "found when detecting a settings change.";
        }
    }

    PhoneConnector * const parent;

    const QHostAddress server;
    const uint port;
    const uint listenPort;

    QHttp protocol;

    QTcpServer * const notificationServer;
    QList<QTcpSocket *> notificationSockets;

    QList<Connection *> connectionList;

    QStringList ids;
    int currentId;
};

// ---------------------------------------------------------------------------

PhoneConnector::PhoneConnector(const QString & serv, const uint prt,
                               const uint listen_prt,
                               QObject * parent) :
    QObject(parent),
    d(new PhoneConnectorPrivate(this, serv, prt, listen_prt))
{
    Q_ASSERT(0 != d);

    QBuffer buf;
    if (blockingGet("/index.htm", &buf))
    {
        // parse IDs:
        bool id_list_upcoming = false;
        QStringList ids;
        QString current_id;
        QStringList lines(QString(buf.buffer()).split("\n"));
        QString line;
        foreach (line, lines)
        {
            if (id_list_upcoming)
            {
                if (line.contains("<option value="))
                {
                    int start_pos = line.indexOf('>');
                    int end_pos = line.indexOf('<', start_pos);
                    QString id(line.mid(start_pos + 1, end_pos - start_pos - 1));
                    qDebug() << "ID found:" << id;

                    if (line.contains("selected"))
                    {
                        current_id = id;
                        qDebug() << "    SELECTED.";
                    }
                    ids.append(id);
                }
                else if (line.contains("</select"))
                { break; }
            }
            else
            {
                if (line.contains("Outgoing Identity:"))
                { id_list_upcoming = true; }
            }
        }

        if (!current_id.isEmpty())
        {
            d->ids = ids;
            Q_ASSERT(ids.indexOf(current_id) >= 0);
            d->currentId = ids.indexOf(current_id);

            d->notificationServer->listen(QHostAddress::Any, d->listenPort);
        }
    }
}

PhoneConnector::~PhoneConnector()
{
    d->notificationServer->close();

    QTcpSocket * socket;
    foreach(socket, d->notificationSockets)
    { socket->disconnectFromHost(); }

    delete(d);
}

bool PhoneConnector::blockingGet(const QString & page, QIODevice * dev) const
{
    qDebug() << "blockingGetting:" << page;
    d->protocol.get(page, dev);

    // block:
    while (d->protocol.currentId() != 0 )
    { QCoreApplication::instance()->processEvents(); }

    if (d->protocol.error() != QHttp::NoError)
    {
        qDebug() << "Got an error during blockingGet:" << d->protocol.error();
        return false;
    }
    QHttpResponseHeader header = d->protocol.lastResponse();
    if (!header.isValid())
    {
        qDebug() << "Got a invalid header in blockingGet.";
        return false;
    }
    if (header.statusCode() != 200 && header.statusCode() != 302)
    {
        qDebug() << "Got an unexpected status code in blockingGet:" << header.statusCode();
        return false;
    }

    return true;
}

bool PhoneConnector::sendKey(const QString & key) const
{ return blockingGet("/command.htm?key=" + key); }

bool PhoneConnector::dial(const QString & target, const QString & uri) const
{
    if (target.isEmpty() || uri.isEmpty()) { return false; }

    return blockingGet(QString("/command.htm?number=") + target +
                       QString("&outgoing_uri=") + uri);
}

QString PhoneConnector::name() const
{
    // Make a cleanish connection name:
    QString phone_name(d->server.toString());
    phone_name += ':' + QString::number(d->port);

    phone_name = phone_name.replace('.', "_2e");
    phone_name = phone_name.replace(':', "_3a");

#if not defined (Q_NO_DEBUG)
    {
        QRegExp regexp("[^a-zA-Z0-9_-]");
        Q_ASSERT(-1 == phone_name.indexOf(regexp));
    }
#endif

    return phone_name;
}

bool PhoneConnector::isConnected() const
{ return (-1 != d->currentId); }

void PhoneConnector::doHandleNotification()
{
    qDebug() << "PC: New Notification socket detected!";
    // FIXME: Close socket if IP is not the one we expect.
    while (d->notificationServer->hasPendingConnections())
    {
        QTcpSocket * notification_socket =
            d->notificationServer->nextPendingConnection();
        connect(notification_socket, SIGNAL(disconnected()),
            this, SLOT(doDeleteNotificationSocket()));

        // Ignore sockets not from our phone:
        if (notification_socket->peerAddress() != d->server)
        {
            notification_socket->disconnectFromHost();
            continue;
        }

        connect(notification_socket, SIGNAL(readyRead()),
                this, SLOT(doReadNotification()));
        d->notificationSockets.append(notification_socket);
    }
}

void PhoneConnector::doReadNotification()
{
    qDebug() << "PC: New Notification received!";
    QTcpSocket * socket;
    foreach(socket, d->notificationSockets)
    {
        if (socket->bytesAvailable() == 0) { continue; }

        QByteArray inputData = socket->readAll();
        QString input(inputData);
        // qDebug() << "Input:" << input;

        if (input.startsWith("GET "))
        {
            QString url = input.mid(4, input.indexOf(' ', 4) - 4);
            qDebug() << "URL to parse:" << url;

            if (url.startsWith('/'))
            {
                int paramstart = url.indexOf('?');
                QString command = url.mid(1, paramstart - 1);
                QStringList params = url.mid(paramstart + 1).split('&');
                QHash<QString, QString> settings;

                qDebug() << "Command:" << command << "Params:" << params;
                QString param;
                foreach (param, params)
                {
                    QStringList tmp = param.split('=');
                    if (tmp.size() != 2) { continue; }
                    settings.insert(tmp[0], tmp[1].replace(QString("%40"), QString('@')));
                }

                if (command == "call") { d->callDetected(settings); }
                else if (command == "set") { d->settingChanged(settings); }
                else
                { qWarning() << "Ignoring" << command << "!"; }
            }
        }

        QHttpResponseHeader response(200);
        QString output(response.toString());
        // qDebug() << "Output:" << output;
        socket->write(output.toUtf8());
        socket->disconnectFromHost();
    }
}

void PhoneConnector::doDeleteNotificationSocket()
{
    QTcpSocket * socket;
    foreach(socket, d->notificationSockets)
    {
        if (socket->state() != QAbstractSocket::ConnectedState)
        {
            d->notificationSockets.removeAll(socket);
            socket->deleteLater();
        }
    }
}

void PhoneConnector::addConnection(Connection * conn)
{
    if (d->connectionList.contains(conn)) { return; }
    d->connectionList.append(conn);
}

void PhoneConnector::rmConnection(Connection * conn)
{
    d->connectionList.removeAll(conn);
    if (d->connectionList.isEmpty())
    {
        emit phoneConnectorIsEmpty(this);
        deleteLater();
    }
}

QHostAddress PhoneConnector::phoneAddress() const
{
    if (!isConnected()) { return QHostAddress(); }
    else
    { return d->notificationServer->serverAddress(); }
}

bool PhoneConnector::knowsAccount(const QString & account) const
{
    QString id;
    foreach (id, d->ids)
    {
        qDebug() << "    PC: matching" << id << "with" << "account.";
        if (id.startsWith(account)) { return true; }
    }
    return false;
}
