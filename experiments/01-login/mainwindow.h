#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "global.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);
    void showExpanded();

    void log(const QString &strLog);

private:
    QString isMovedTemporarily(const QString &strResponse);

    bool doLogin2(QString strUrl);

private slots:
    void on_actionE_xit_triggered();
    void on_actionDo_it_triggered();

    void onLogin1(bool success,const QByteArray &response);
    void onLogin2(bool success,const QByteArray &response);

private:
    Ui::MainWindow *ui;
    QString strUser, strPass;

    QNetworkAccessManager nwMgr;
    CookieJar jar;
};

#endif // MAINWINDOW_H
