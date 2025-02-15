#include "weaponconfig.h"
#include <QJsonArray>
#include <QFile>

WeaponConfig::WeaponConfig()
{
    QFile f(":/weapon.lua");
    if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        this->luaCode = f.readAll();
        f.close();
    }
}

QString WeaponConfig::getListString(QList<QPair<QString, QString> > &l)
{
    QString s = "";
    for(int i = 0; i < l.size(); i++) {
        QString key = QString::number(i);
        int j;
        for(j = 0; j < l.size(); j++) {
            if(l.at(j).first == key) {
                s += l.at(j).second + ",";
                break;
            }
        }
        if(j == l.size())
            return "";
    }
    return s;
}

QString WeaponConfig::getListStringAddOne(QList<QPair<QString, QString> > &l)
{
    QString s = "";
    for(int i = 0; i < l.size(); i++) {
        QString key = QString::number(i+1);
        int j;
        for(j = 0; j < l.size(); j++) {
            if(l.at(j).first == key) {
                s += l.at(j).second + ",";
                break;
            }
        }
        if(j == l.size())
            return "";
    }
    return s;
}

QString WeaponConfig::getStringByName(QString name, QList<QPair<QString, QString> > &l)
{
    for(int i = 0; i < l.size(); i++) {
        if(l.at(i).first == name)
            return l.at(i).second;
    }
    return "";
}

void WeaponConfig::setStringByName(QString name, QString value, QList<QPair<QString, QString> > &l)
{
    for(int i = 0; i < l.size(); i++) {
        if(l.at(i).first == name) {
            l[i].second = value;
            return;
        }
    }
}

QString WeaponConfig::toLuaCode()
{
    if(this->name == "" || this->pose.size() == 0 ||
            this->scope.size() == 0 || this->attach1.size() == 0 ||
            this->attach2.size() == 0 || this->attach4.size() == 0  || this->speed == "" ||
            this->max == "" || this->ballistic == "") return "";

    return this->luaCode.arg(this->name,
                             getListStringAddOne(this->pose),
                             getListString(this->scope),
                             getListString(this->attach1),
                             getListString(this->attach2),
                             getListString(this->attach4),
                             this->speed,
                             this->max)
                        .arg(this->ballistic);
}

QJsonObject WeaponConfig::toJsonObject()
{
    QJsonObject jsonObj;
    QJsonArray pose_arr;
    QJsonArray scope_arr;
    QJsonArray attach1_arr;
    QJsonArray attach2_arr;
    QJsonArray attach4_arr;

    for(int i = 0; i < pose.size(); i++) {
        QString key = QString::number(i+1);
        int j;
        for(j = 0; j < pose.size(); j++) {
            if(pose[j].first == key) {
                pose_arr.append(QJsonValue(pose[j].second));
                break;
            }
        }
        if(j == pose.size())
            break;
    }

    for(int i = 0; i < scope.size(); i++) {
        QString key = QString::number(i);
        int j;
        for(j = 0; j < scope.size(); j++) {
            if(scope[j].first == key) {
                scope_arr.append(QJsonValue(scope[j].second));
                break;
            }
        }
        if(j == scope.size()) break;
    }

    for(int i = 0; i < attach1.size(); i++) {
        QString key = QString::number(i);
        int j;
        for(j = 0; j < attach1.size(); j++) {
            if(attach1[j].first == key) {
                attach1_arr.append(QJsonValue(attach1[j].second));
                break;
            }
        }
        if(j == attach1.size()) break;
    }

    for(int i = 0; i < attach2.size(); i++) {
        QString key = QString::number(i);
        int j;
        for(j=0; j<attach2.size(); j++) {
            if(attach2[j].first == key) {
                attach2_arr.append(QJsonValue(attach2[j].second));
                break;
            }
        }
        if(j == attach2.size())
            break;
    }

    for(int i = 0; i < attach4.size(); i++) {
        QString key = QString::number(i);
        int j;
        for(j=0; j<attach4.size(); j++) {
            if(attach4[j].first == key) {
                attach4_arr.append(QJsonValue(attach4[j].second));
                break;
            }
        }
        if(j == attach4.size())
            break;
    }

    jsonObj.insert("pose_sensitive", pose_arr);
    jsonObj.insert("scope_sensitive", scope_arr);
    jsonObj.insert("a1_sensitive", attach1_arr);
    jsonObj.insert("a2_sensitive", attach2_arr);
    jsonObj.insert("a4_sensitive", attach4_arr);

    jsonObj.insert("speed", QJsonValue(this->speed));
    jsonObj.insert("max", QJsonValue(this->max));

    jsonObj.insert("ballistic", QJsonValue(this->ballistic));

    return jsonObj;
}
