#include "gen/cm_adapter.h"
#include "QGVConnectionManager.h"

int
main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QGVConnectionManager *cm = new QGVConnectionManager;
    new ConnectionManagerAdaptor(cm);

/*
    MyDemo* demo = new MyDemo;
    new DemoIfAdaptor(demo);

    QDBusConnection connection = QDBusConnection::sessionBus();
    bool ret = connection.registerService("net.yuvraaj.qgvdial.PhoneIntegration");
    ret = connection.registerObject("/", demo);
*/

    return a.exec();
}
