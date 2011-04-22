QT      *= core gui webkit sql xml xmlpatterns script
TARGET   = qgvnotify
TEMPLATE = app

CONFIG  *= precompile_header mobility

# In Windows, add the mosquitto dll
win32 {
    CONFIG *= embed_manifest_exe
    LIBS *= -lmosquitto
}

unix:!symbian {
    QT *= dbus
    LIBS *= -lmosquitto

    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

PRECOMPILED_HEADER = global.h

SOURCES  += main.cpp                    \
            MainWindow.cpp

HEADERS  += global.h                    \
            MainWindow.h
