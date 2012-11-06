#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus

#include <QtCore>
#include "shared_data_types.h"

#define LOGS_SERVER "http://www.yuvraaj.net"

#if defined(Q_WS_X11)
#define __FULLFUNC__ __PRETTY_FUNCTION__
#else
#define __FULLFUNC__ __FUNCTION__
#endif

#define Q_DEBUG(_s) qDebug() << QString("%1(%2): %3").arg(__FULLFUNC__).arg(__LINE__).arg(_s)
#define Q_WARN(_s) qWarning() << QString("%1(%2): %3").arg(__FULLFUNC__).arg(__LINE__).arg(_s)
#define Q_CRIT(_s) qCritical() << QString("%1(%2): %3").arg(__FULLFUNC__).arg(__LINE__).arg(_s)

#define QGV_ProtocolName "qgv"

#define TP_OBJECT_PATH    "org/freedesktop/Telepathy"
// org/freedesktop/Telepathy/ConnectionManager
#define TP_CM_OBJECT_PATH TP_OBJECT_PATH "/ConnectionManager"
// org/freedesktop/Telepathy/Connection
#define TP_CONN_OBJECT_PATH TP_OBJECT_PATH "/Connection"

#define TP_SERVICE_PATH   "org.freedesktop.Telepathy"
// org.freedesktop.Telepathy.ConnectionManager
#define TP_CM_SERVICE_PATH TP_SERVICE_PATH ".ConnectionManager"
// org.freedesktop.Telepathy.Connection
#define TP_CONN_SERVICE_PATH TP_SERVICE_PATH ".Connection"

#define ofdT_Conn_Iface TP_CONN_SERVICE_PATH ".Interface"
// org.freedesktop.Telepathy.Connection.Interface.ContactBlocking
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".ContactBlocking"
// org.freedesktop.Telepathy.Connection.Interface.ContactGroups
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".ContactGroups"
// org.freedesktop.Telepathy.Connection.Interface.ContactList
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".ContactList"
// org.freedesktop.Telepathy.Connection.Interface.Contacts
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Contacts"
// org.freedesktop.Telepathy.Connection.Interface.Capabilities
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Capabilities"
// org.freedesktop.Telepathy.Connection.Interface.Avatars
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Avatars"
// org.freedesktop.Telepathy.Connection.Interface.Aliasing
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Aliasing"
// org.freedesktop.Telepathy.Connection.Interface.SimplePresence
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".SimplePresence"
// org.freedesktop.Telepathy.Connection.Interface.Presence
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Presence"
// org.freedesktop.Telepathy.Connection.Interface.Requests
#define ofdT_Conn_Iface_ContactBlocking ofdT_Conn_Iface ".Requests"

////////////////////////////////////////////////////////////////////////////////

#define QGV_CM_OBJECT_PATH  TP_CM_OBJECT_PATH  "/qgvtp"
#define QGV_CM_SERVICE_PATH TP_CM_SERVICE_PATH ".qgvtp"
#define QGV_CONN_OBJECT_PREFIX  TP_CONN_OBJECT_PATH  "/qgvtp/qgv/"
#define QGV_CONN_SERVICE_PREFIX TP_CONN_SERVICE_PATH ".qgvtp.qgv/"

#endif //__cplusplus
#endif //__GLOBAL_H__
