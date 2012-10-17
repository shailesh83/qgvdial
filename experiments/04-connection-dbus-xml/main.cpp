#include <QtCore>
#include "shared_data_types.h"

int
main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

/*
    MyDemo* demo = new MyDemo;
    new DemoIfAdaptor(demo);

    QDBusConnection connection = QDBusConnection::sessionBus();
    bool ret = connection.registerService("net.yuvraaj.qgvdial.PhoneIntegration");
    ret = connection.registerObject("/", demo);
*/

    return a.exec();
}
