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

#include "connectionmanager.h"
#include "phoneconnector.h"

#include "connectionmanageradaptor.h"
#include "connection.h"

#include <QtCore/QDebug>
#include <QtNetwork/QHostAddress>

namespace
{
static const QString protocol_name("snom");
}

class ConnectionManagerPrivate
{
public:
    ConnectionManagerPrivate(ConnectionManager * parent) :
        adaptor(new ConnectionManagerAdaptor(parent))
    { Q_ASSERT(0 != adaptor); }

    ~ConnectionManagerPrivate() { }

    ConnectionManagerAdaptor * const adaptor;

    QList<PhoneConnector *> connectors;
};

// ---------------------------------------------------------------------------

ConnectionManager::ConnectionManager(QObject * parent) :
    QObject(parent),
    d(new ConnectionManagerPrivate(this))
{ Q_ASSERT(0 != d); }

ConnectionManager::~ConnectionManager()
{ delete(d); }

org::freedesktop::Telepathy::ParameterDefinitionList
ConnectionManager::GetParameters(const QString &proto)
{
    qDebug() << "ConnectionManager::GetParameters(const QString &prot)";
    org::freedesktop::Telepathy::ParameterDefinitionList result;
    org::freedesktop::Telepathy::ParameterDefinition param;

    // Attention! Default constructed QDBusVariants cause havok on the D-Bus!
    param.name = "account";
    param.flags = Required;
    param.signature = "s";
    param.defaultValue = QDBusVariant(QString());
    result.append(param);

    param.name = "server";
    param.flags = Required;
    param.signature = "s";
    param.defaultValue = QDBusVariant(QString());
    result.append(param);

    param.name = "port";
    param.flags = Required|hasDefault;
    param.signature = "q";
    param.defaultValue = QDBusVariant(80);
    result.append(param);

    param.name = "listen_port";
    param.flags = Required|hasDefault;
    param.signature = "q";
    param.defaultValue = QDBusVariant(22222);
    result.append(param);

    return result;
}

QStringList ConnectionManager::ListProtocols()
{
    qDebug() << "QStringList ConnectionManager::ListProtocols()";
    return QStringList(protocol_name);
}

QString ConnectionManager::RequestConnection(const QString & proto,
                                             QVariantMap parameters,
                                             QDBusObjectPath & object_path)
{
    qDebug() << "CM: Request Connection...";
    QString connection_service;
    object_path = QDBusObjectPath();

    if (proto != protocol_name)
    {
        // FIXME: return org.freedesktop.Telepathy.Error.NotImplemented
        qDebug() << "CM::RequestConnection: proto mismatch.";
        return connection_service;
    }

    QString account;
    QString server;
    uint port(80);
    uint listen_port(22222);

    // read parameters:
    QString param;
    foreach (param, parameters.keys())
    {
        if ("account" == param)
        { account = parameters[param].toString(); }
        else if ("server" == param)
        { server = parameters[param].toString(); }
        else if ("port" == param)
        { port = parameters[param].toInt(); }
        else if ("listen_port" == param)
        { listen_port = parameters[param].toInt(); }
        else
        {
            // FIXME: return org.freedesktop.Telepathy.Error.InvalidArgument
            qDebug() << "CM::RequestConnection: invalid parameter" << param << "found.";
            return connection_service;
        }
    }

    // Check required parameters:
    if (server.isEmpty())
    {
        // FIXME: return org.freedesktop.Telepathy.Error.InvalidArgument
        qDebug() << "CM::RequestConnection: proto missing.";
        return connection_service;
    }

    qDebug() << "CM: looking for phoneconnector...";

    const QHostAddress phone_address(server);

    PhoneConnector * connector(0);
    foreach (connector, d->connectors)
    {
        if (phone_address == connector->phoneAddress())
        { break; }
    }
    if (0 == connector || phone_address != connector->phoneAddress())
    {
        qDebug() << "CM: No phoneconnector found... creating a new one.";
        connector = new PhoneConnector(server, port, listen_port);
        connect(connector, SIGNAL(phoneConnectorIsEmpty(PhoneConnector * const)),
                this, SLOT(onPhoneConnectorIsEmpty(PhoneConnector * const)));
        Q_ASSERT(0 != connector);
        d->connectors.append(connector);
    }

    qDebug() << "CM: Connector found...";

    if (!connector->knowsAccount(account))
    {
        // FIXME: return org.freedesktop.Telepathy.Error.InvalidArgument
        qDebug() << "CM::RequestConnection: account not known on phone.";
        return QString();
    }
    Q_ASSERT(connector->isConnected()); // Must be true if the account is known.

    qDebug() << "CM: Account is known on phone...";


    Connection * new_connection = new Connection(connector, account, this);
    Q_ASSERT(0 != new_connection);

    if (!new_connection->registerObject())
    {
        qDebug() << "CM: Not a phone or registration failure.";
        delete new_connection;
        return QString();
    }

    qDebug() << "CM: Phone found... all is well.";

    object_path = new_connection->objectPath();

    emit NewConnection(connection_service, object_path, QString("snom"));

    return new_connection->serviceName();
}

void ConnectionManager::onPhoneConnectorIsEmpty(PhoneConnector * const connector)
{ d->connectors.removeAll(connector); }
