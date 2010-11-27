QT       += dbus core
QT       -= gui

TARGET = qgv-tp
CONFIG   += qt debug console
CONFIG   -= app_bundle

TEMPLATE = app
VPATH += src
MOC_DIR = mocs
OBJECTS_DIR = objs

SOURCES  += src/main.cpp \
            src/connectiontypes.cpp \
            src/connectionmanagertypes.cpp \
            src/connectionmanageradaptor.cpp \
            src/connectionmanager.cpp \
            src/connectionadaptor.cpp \
            src/connection.cpp \
            src/connectioninterfacerequeststypes.cpp \
            src/connectioninterfacerequestsadaptor.cpp \
            src/vicarcallrouterproxy.cpp \
            src/connectioninterfacecapabilitiestypes.cpp \
            src/connectioninterfacecapabilitiesadaptor.cpp

HEADERS  += src/names.h \
            src/connectiontypes.h \
            src/connectionmanagertypes.h \
            src/connectionmanageradaptor.h \
            src/connectionmanager.h \
            src/connectionadaptor.h \
            src/connection.h \
            src/basetypes.h \
            src/connectioninterfacerequeststypes.h \
            src/connectioninterfacerequestsadaptor.h \
            src/vicarcallrouterproxy.h \
            src/connectioninterfacecapabilitiestypes.h \
            src/connectioninterfacecapabilitiesadaptor.h

#MAKE INSTALL
INSTALLDIR = /../../debian/qgvdial

INSTALLS += target

target.path =$$INSTALLDIR/opt/qgvdial/bin

