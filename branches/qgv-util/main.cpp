/*
@version: 0.5
@author: Sudheer K. <scifi1947 at gmail.com>
@license: GNU General Public License
*/

#include "telepathyutility.h"
#include "databaseutility.h"
#include "vicarprofiletypes.h"
#include <QDebug>
#include <QDBusMetaType>

int main(int argc, char *argv[])
{
    qDBusRegisterMetaType<org::freedesktop::Telepathy::SimplePresence>();
    //qDBusRegisterMetaType<org::maemo::vicar::Profile>();
    //qDBusRegisterMetaType<org::maemo::vicar::ProfileList>();

    TelepathyUtility *tpUtility = new TelepathyUtility();
    DatabaseUtility *databaseUtility = new DatabaseUtility();

    if (argc > 1 && argv[1]){
        QString instruction = QString(argv[1]);
        if (instruction == "INSTALL"){
            //Check if Account already exists
            if (!tpUtility->accountExists()){
                qDebug() << "VICaR account not found. Creating ..";
                bool result = tpUtility->createAccount();
                if (!result) exit(1);
            }
            else{
                qDebug() << "VICaR account found.";
            }
        }
        else if (instruction == "REMOVE"){
            bool result = tpUtility->deleteAccount();
            if (!result) exit(2);
        }

        else if (instruction == "CREATEDB"){
            bool result = databaseUtility->openDatabase();
            if (!result){
                qDebug() <<"Error creating profiles database. "
                        <<databaseUtility->lastError();
                exit(3);
            }
            qDebug() << "VICaR profiles database opened.";

            if (!databaseUtility->tableExists("profiles")){
                qDebug() << "Creating VICaR profiles table..";
                result = databaseUtility->createProfilesTable();
                if (!result){
                    qDebug() <<"Error creating profiles table. "
                            <<databaseUtility->lastError();
                    exit(4);
                }
            }
            else{
                qDebug()<<"Profiles table exists";
            }
            databaseUtility->closeDatabase();
        }
        else if (instruction == "DROPDB"){
            bool result = databaseUtility->deleteDatabase();
            if (!result){
                qDebug() <<"Error deleting profiles database. "
                        <<databaseUtility->lastError();
                exit(5);
            }
            qDebug() << "VICaR profiles database deleted.";
        }
        else if (instruction == "ACCOUNTSTATUS"){
            QString status = tpUtility->getAccountStatus();
            qDebug() << "Account Status is "<< status;
        }
    }

    delete (tpUtility);
    delete (databaseUtility);
    tpUtility = 0;
    databaseUtility = 0;
}
