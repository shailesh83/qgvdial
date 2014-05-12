#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QmlMainWindow.h"

class MainWindow : public QmlMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = 0);

protected:
    QString getMainQmlPath();
};

QCoreApplication *
createAppObject(int &argc, char **argv);

#endif // MAINWINDOW_H