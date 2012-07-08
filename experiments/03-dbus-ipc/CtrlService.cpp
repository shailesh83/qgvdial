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
    emit RequestClientInfo("getUser");
}//CtrlService::requestUserInfo

