#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

typedef QPair<QString,QString> QStringPair;
typedef QList<QStringPair> QStringPairList;

///////////////////////// Google contacts API related //////////////////////////
enum PhoneType {
    PType_Unknown = 0,
    PType_Mobile,
    PType_Home,
    PType_Other,
};

struct PhoneInfo
{
    PhoneType   Type;
    QString     strNumber;

    PhoneInfo() { init(); }

    void init () {
        Type = PType_Unknown;
        strNumber.clear ();
    }

    static PhoneType charToType(const char ch) {
        switch (ch) {
        case 'M':
            return PType_Mobile;
        case 'H':
            return PType_Home;
        case 'O':
            return PType_Other;
        default:
            return PType_Unknown;
        }
    }
    static char typeToChar (PhoneType type) {
        switch(type) {
        case PType_Mobile:
            return 'M';
        case PType_Home:
            return 'H';
        case PType_Other:
            return 'O';
        default:
            return '?';
        }
    }
    static const char * typeToString (PhoneType type) {
        switch(type) {
        case PType_Mobile:
            return "Mobile";
        case PType_Home:
            return "Home";
        case PType_Other:
            return "Other";
        default:
            return "Unknown";
        }
    }
};
typedef QVector<PhoneInfo> PhoneInfoArray;

struct ContactInfo
{
    //! Unique ID for this contact
    QString         strId;
    //! Contact title a.k.a. the contact name
    QString         strTitle;

    //! Array of phones for this contact
    PhoneInfoArray  arrPhones;
    // Which number is selected
    int             selected;

    //! The notes that the user may have added to the contact
    QString         strNotes;

    //! Is this contact deleted?
    bool            bDeleted;

    ContactInfo() { init(); }

    void init () {
        strId.clear ();
        strTitle.clear ();
        arrPhones.clear ();
        strNotes.clear ();
        bDeleted = false;
        selected = 0;
    }
};
Q_DECLARE_METATYPE(ContactInfo)
////////////////////////////////////////////////////////////////////////////////

#endif //_GLOBAL_H_
