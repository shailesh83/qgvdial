TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT += dbus

# Input
HEADERS += cm_adapter.h             \
           connection_adapter.h     \
           protocol_adapter.h       \
           shared_data_types.h

SOURCES += cm_adapter.cpp           \
           connection_adapter.cpp   \
           main.cpp                 \
           protocol_adapter.cpp     \

exists(not-really) {
HEADERS += cm_proxy.h               \
           connection_proxy.h       \
           protocol_proxy.h

SOURCES += cm_proxy.cpp             \
           connection_proxy.cpp     \
           protocol_proxy.cpp
}
