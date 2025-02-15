#ifndef WEAPONCONFIG_H
#define WEAPONCONFIG_H

#include <QString>
#include <QImage>
#include <QList>
#include <QPair>
#include <QMap>
#include <QJsonObject>

class WeaponConfig
{
public:
    QString name="";
    QList<QPair<QString, QString>> scope;
    QList<QPair<QString, QString>> attach1;
    QList<QPair<QString, QString>> attach2;
    QList<QPair<QString, QString>> attach4;
    QList<QPair<QString, QString>> pose;
    QString speed="";
    QString max="";
    QString ballistic="";
    QImage img;
    QString luaCode;
    WeaponConfig();
    QString getListString(QList<QPair<QString, QString>> &l);
    QString getListStringAddOne(QList<QPair<QString, QString>> &l);
    QString getStringByName(QString name, QList<QPair<QString, QString>> &l);
    void setStringByName(QString name, QString value, QList<QPair<QString, QString> > &l);
    QString toLuaCode();
    QJsonObject toJsonObject();
};

#endif // WEAPONCONFIG_H
