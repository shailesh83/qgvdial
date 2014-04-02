QT += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 05-contacts
TEMPLATE = app


SOURCES  += main.cpp \
            MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

RESOURCES += 05_contacts.qrc

include(../../trunk/qgvdial/features/o2/o2.pri)
