QT       += dbus core
QT       -= gui

TEMPLATE = app
TARGET = qgv-util
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += telepathyutility.h
SOURCES += main.cpp telepathyutility.cpp
