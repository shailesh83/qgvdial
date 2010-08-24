#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

// Required for Symbian (QSystemTrayIcon)
#include "OsDependent.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setStatus (const QString &strText, int timeout = 0);

private slots:
    //! The Singleton Application class invokes this function
    void on_actionE_xit_triggered();
    void messageReceived (const QString &message);
    //! Invoked when the user clicks Login
    void on_action_Login_triggered();
    //! Invoked when the system ray is clicked
    void systray_activated (QSystemTrayIcon::ActivationReason reason);

    void charClicked (QChar ch);
    void charDeleted ();

private:
    Ui::MainWindow *ui;

    // Tray and all icons
    QIcon           icoGoogle;
    QSystemTrayIcon *pSystray;
};

#endif // MAINWINDOW_H
