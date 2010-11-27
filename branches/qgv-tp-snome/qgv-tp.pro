TEMPLATE = app
TARGET = qgv-tp
DEPENDPATH += src
INCLUDEPATH += src

QT *= core network dbus
CONFIG *= console

MOC_DIR = mocs
OBJECTS_DIR = objs

# Input
HEADERS += src/basetypes.h \
           src/channel.h \
           src/channeladaptor.h \
           src/channelgroupinterfacetypes.h \
           src/channelinterfacedtmfadaptor.h \
           src/channelinterfacedtmftypes.h \
           src/channelinterfacegroupadaptor.h \
           src/channelstreamedmediaadaptor.h \
           src/channelstreamedmediatypes.h \
           src/connection.h \
           src/connectionadaptor.h \
           src/connectioninterfaceprivacyadaptor.h \
           src/connectionmanager.h \
           src/connectionmanageradaptor.h \
           src/connectionmanagertypes.h \
           src/connectiontypes.h \
           src/names.h \
           src/phoneconnector.h

SOURCES += src/channel.cpp \
           src/channeladaptor.cpp \
           src/channelinterfacedtmfadaptor.cpp \
           src/channelinterfacegroupadaptor.cpp \
           src/channelstreamedmediaadaptor.cpp \
           src/channelstreamedmediatypes.cpp \
           src/connection.cpp \
           src/connectionadaptor.cpp \
           src/connectioninterfaceprivacyadaptor.cpp \
           src/connectionmanager.cpp \
           src/connectionmanageradaptor.cpp \
           src/connectionmanagertypes.cpp \
           src/connectiontypes.cpp \
           src/main.cpp \
           src/phoneconnector.cpp

