#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    Utils();

    static QString toLuaUnicode(QString &s);
};

#endif // UTILS_H
