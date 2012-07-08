#include "CtrlService.h"

CtrlService::CtrlService(QObject *parent)
: QObject(parent)
{
}//CtrlService::CtrlService

void
CtrlService::ReportUser(const QString &user)
{
    Q_DEBUG("Reporting user: ") << user;
}//CtrlService::ReportUser

void
CtrlService::requestUserInfo()
{
    emit CommandForClient("getUser");
}//CtrlService::requestUserInfo

void
CtrlService::requestAllQuit()
{
    emit CommandForClient("quitAll");
}//CtrlService::requestAllQuit

