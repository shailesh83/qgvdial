#ifndef __SHARED_DATA_TYPES__
#define __SHARED_DATA_TYPES__

#include <QtCore>
#include <QtDBus>

typedef QMap<QString, QVariant> Qt_Type_dict_sv;

struct Struct_susv {
    QString     s1;
    unsigned    u;
    QString     s2;
    QVariant    v;
};
Q_DECLARE_METATYPE(Struct_susv)

typedef QList<Struct_susv> Qt_Type_a_susv;
Q_DECLARE_METATYPE(Qt_Type_a_susv)

typedef QList<unsigned> Qt_Type_au;
Q_DECLARE_METATYPE(Qt_Type_au)

typedef QMap<unsigned, QString> Qt_Type_dict_us;
Q_DECLARE_METATYPE(Qt_Type_dict_us)

struct Struct_su {
    QString     s;
    unsigned    u;
};
Q_DECLARE_METATYPE(Struct_su)

typedef QList<Struct_su> Qt_Type_a_su;
Q_DECLARE_METATYPE(Qt_Type_a_su)

struct Struct_us {
    unsigned    u;
    QString     s;
};
Q_DECLARE_METATYPE(Struct_us)

typedef QList<Struct_us> Qt_Type_a_us;
Q_DECLARE_METATYPE(Qt_Type_a_us)

#endif//__SHARED_DATA_TYPES__
