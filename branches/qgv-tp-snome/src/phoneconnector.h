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

#ifndef _SNOM_PHONECONNECTOR_H_
#define _SNOM_PHONECONNECTOR_H_

#include <QtNetwork/QHostAddress>

class PhoneConnectorPrivate;
class Connection;

static const QString key_hangup("ONHOOK");
static const QString key_pickup("OFFHOOK");
static const QString key_disconnect("DISCONNECT");
static const QString key_cancel("CANCEL");
static const QString key_dnd("F_DND");
static const QString key_digit_0("0");
static const QString key_digit_1("1");
static const QString key_digit_2("2");
static const QString key_digit_3("3");
static const QString key_digit_4("4");
static const QString key_digit_5("5");
static const QString key_digit_6("6");
static const QString key_digit_7("7");
static const QString key_digit_8("8");
static const QString key_digit_9("9");
static const QString key_asterisk("*");
static const QString key_hash("#");
static const QString key_speaker("SPEAKER");

class PhoneConnector : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PhoneConnector)

public:
    PhoneConnector(const QString & server,
                   const uint port,
                   const uint listen_port,
                   QObject * parent = 0);
    ~PhoneConnector();

    void startListening();
    void stopListening();

    bool isConnected() const;
    bool knowsAccount(const QString &) const;

    QString name() const;

    bool blockingGet(const QString &, QIODevice * dev = 0) const;
    bool sendKey(const QString & key) const;
    bool dial(const QString & target, const QString & uri) const;

    void addConnection(Connection *);
    void rmConnection(Connection *);

    QHostAddress phoneAddress() const;

signals:
    void phoneConnectorIsEmpty(PhoneConnector * const);

private slots:
    void doHandleNotification();
    void doReadNotification();
    void doDeleteNotificationSocket();

private:
    PhoneConnector(const PhoneConnector &); // no impl.

    PhoneConnectorPrivate * const d;
};

#endif
