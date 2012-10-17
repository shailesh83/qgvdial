TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT += dbus

# Input
HEADERS += cm_adapter.h             \
           cm_proxy.h               \
           connection_adapter.h     \
           connection_proxy.h       \
           protocol_adapter.h       \
           protocol_proxy.h         \
           shared_data_types.h

SOURCES += cm_adapter.cpp           \
           cm_proxy.cpp             \
           connection_adapter.cpp   \
           connection_proxy.cpp     \
           main.cpp                 \
           protocol_adapter.cpp     \
           protocol_proxy.cpp
