QT += core gui script widgets webkitwidgets

TARGET = 05-contacts
TEMPLATE = app


SOURCES  += main.cpp \
            MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

RESOURCES += 05_contacts.qrc

include(../../trunk/api/o2/o2.pri)
