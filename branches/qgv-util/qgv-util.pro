QT       += dbus core
QT       -= gui

TEMPLATE = app
TARGET = qgv-util
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS  += telepathyutility.h      \
            accountproxy.h          \
            accountmanagerproxy.h   \
            accountcompatproxy.h

SOURCES  += main.cpp                \
            telepathyutility.cpp    \
            accountproxy.cpp        \
            accountmanagerproxy.cpp \
            accountcompatproxy.cpp

