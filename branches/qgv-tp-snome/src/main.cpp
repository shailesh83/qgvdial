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

#include <QtCore>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>

#include "names.h"
#include "connectionmanager.h"
#include "basetypes.h"
#include "connectionmanagertypes.h"
#include "connectiontypes.h"
#include "channelstreamedmediatypes.h"

#include <iostream>
#include <fstream>
using namespace std;

/**
 * @mainpage QGVDial Telepathy connection manager.
 *
 * Welcome to QGVDial Telepathy
 */


ofstream fLogfile;
void
qDebugHandler (QtMsgType type, const char *msg)
{
    int level = -1;
    switch (type) {
    case QtDebugMsg:
        level = 3;
        break;
    case QtWarningMsg:
        level = 2;
        break;
    case QtCriticalMsg:
        level = 1;
        break;
    case QtFatalMsg:
        level = 0;
    }

    QDateTime dt = QDateTime::currentDateTime ();
    QString strLog = QString("%1 : %2 : %3")
                     .arg(dt.toString ("yyyy-MM-dd hh:mm:ss.zzz"))
                     .arg(level)
                     .arg(msg);

    fLogfile << strLog.toAscii().data();
    if (QtFatalMsg == type) {
        abort ();
    }
}//qDebugHandler

int
main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    fLogfile.open ("/var/log/qgv-tp.log", ios::app);
    qInstallMsgHandler(qDebugHandler);

    // register types:
    qDBusRegisterMetaType<org::freedesktop::Telepathy::ParameterDefinition>();
    qDBusRegisterMetaType<org::freedesktop::Telepathy::ParameterDefinitionList>();
    qDBusRegisterMetaType<org::freedesktop::Telepathy::ChannelInfo>();
    qDBusRegisterMetaType<org::freedesktop::Telepathy::ChannelInfoList>();
    qDBusRegisterMetaType<org::freedesktop::Telepathy::StreamInfo>();
    qDBusRegisterMetaType<org::freedesktop::Telepathy::StreamInfoList>();

    // register on D-BUS:
    QDBusConnection::sessionBus().registerService(cm_service_name);
    qDebug() << QString ("Service %1 registered with session bus.")
                        .arg(cm_service_name);

    ConnectionManager connection_mgr(&app);
    QDBusConnection::sessionBus().registerObject(cm_object_path,
                                                 &connection_mgr);

    qDebug("Enternig main loop.");

    return app.exec();
}
