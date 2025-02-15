#include "globalconfig.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QApplication>
#include <QDebug>

GlobalConfig::GlobalConfig()
{
    this->loadJson();
}

void GlobalConfig::loadJson()
{
    QString pwd = QApplication::applicationDirPath();
    QString fpath =  "/resource/config.json";
    QFile jsonf(pwd + fpath);
    if(jsonf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString jsonstr = jsonf.readAll();
        jsonf.close();
        QJsonParseError err;
        QJsonDocument document = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
        if(err.error == QJsonParseError::NoError) {
            QJsonObject jsonObj = document.object();

            if(jsonObj.contains("xrate")) {
                QJsonValue v = jsonObj.value("xrate");
                if(v.isDouble()) {
                    this->xrate = v.toDouble(1.0);
                }
            }
            if(jsonObj.contains("yrate")) {
                QJsonValue v = jsonObj.value("yrate");
                if(v.isDouble()) {
                    this->yrate = v.toDouble(0.0);
                }
            }

            if(jsonObj.contains("bloodcolor")) {
                QJsonValue v = jsonObj.value("bloodcolor");
                this->bloodcolor = v.toInt(GREEN_BLOOD);
            }


            if(jsonObj.contains("fps")) {
                QJsonValue v = jsonObj.value("fps");
                this->fps = v.toInt(16);
            }

            if(jsonObj.contains("capturemode")) {
                QJsonValue v = jsonObj.value("capturemode");
                this->capturemode = v.toInt(DXGI_CAPTURE);
            }


            if(jsonObj.contains("isdebug")) {
                QJsonValue v = jsonObj.value("isdebug");
                this->isdebug = v.toInt(0);
            }

            if(jsonObj.contains("xmove")) {
                QJsonValue v = jsonObj.value("xmove");
                this->xmove = v.toInt(10);
            }

            if(jsonObj.contains("autorecognize")) {
                QJsonValue v = jsonObj.value("autorecognize");
                this->autorecognize = v.toInt(1);
            }

            if(jsonObj.contains("autoshift")) {
                QJsonValue v = jsonObj.value("autoshift");
                this->autoshift = v.toInt(0);
            }

            if(jsonObj.contains("loopdelay")) {
                QJsonValue v = jsonObj.value("loopdelay");
                this->loopdelay = v.toInt(0);
            }
        }
    }
}

void GlobalConfig::saveJson()
{
    QJsonObject jobj;

    jobj.insert("xrate", this->xrate);
    jobj.insert("yrate", this->yrate);
    jobj.insert("bloodcolor", this->bloodcolor);
    jobj.insert("fps", this->fps);
    jobj.insert("capturemode", this->capturemode);

    jobj.insert("isdebug", this->isdebug);
    jobj.insert("xmove", this->xmove);
    jobj.insert("autorecognize", this->autorecognize);
    jobj.insert("autoshift", this->autoshift);
    jobj.insert("loopdelay", this->loopdelay);

    QJsonDocument doc;
    doc.setObject(jobj);
    QFile jsonf(QApplication::applicationDirPath() + "/resource/config.json");
    if(jsonf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        jsonf.write(doc.toJson());
        jsonf.close();
    }
}

GlobalConfig *GlobalConfig::getInstance()
{
    static GlobalConfig instance;
    return &instance;
}

GlobalConfig::~GlobalConfig()
{

}


