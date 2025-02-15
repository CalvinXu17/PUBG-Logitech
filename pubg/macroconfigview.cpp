#include "macroconfigview.h"
#include "ui_macroconfigview.h"
#include "utils.h"
#include "globalstatic.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QDebug>

MacroConfigView::MacroConfigView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MacroConfigView)
{
    ui->setupUi(this);

    QFile f(":/macro_ghub.lua");

    if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        this->macro_ghub = f.readAll();
        f.close();
    }

    f.setFileName(":/configc_ghub.lua");
    if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        this->macroConfig_ghub = f.readAll();
        f.close();
    }

    QIntValidator *ivalid = new QIntValidator(0, 999, this);
    this->ui->lineEdit_offset->setValidator(ivalid);
    this->ui->lineEdit_sleeptime->setValidator(ivalid);

    QDoubleValidator *dvalid = new QDoubleValidator(0, 2.0, 2, this);
    this->ui->lineEdit_ysensitive->setValidator(dvalid);

    dvalid = new QDoubleValidator(0, 100.0, 1, this);
    this->ui->lineEdit_scopesensitive->setValidator(dvalid);

    this->loadJson();

    this->saveLua();

    this->logiNet = new LogiNet();
    ivalid = new QIntValidator(1000, 65535, this);
    this->ui->lineEdit_loginet->setValidator(ivalid);
}

MacroConfigView::~MacroConfigView()
{
    this->saveJson();
    delete ui;

    if (this->logiNet) {
        delete this->logiNet;
    }
}

void MacroConfigView::loadJson()
{
    int adsmode = 0;
    bool debug = false;
    int xbase = 0;
    double scope_sensitive = 50.0;
    double y_sensitive = 1.0;
    int sleep = 10;

    QString pwd = QApplication::applicationDirPath();
    QString config_path =  "/resource/config.json";
    QFile jsonf(pwd + config_path);
    if(jsonf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString jsonstr = jsonf.readAll();
        jsonf.close();
        QJsonParseError err;
        QJsonDocument document = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
        if(err.error == QJsonParseError::NoError) {
            QJsonObject jsonObj = document.object();

            if(jsonObj.contains("adsmode")) {
                QJsonValue v = jsonObj.value("adsmode");
                adsmode = v.toInt();
            }
            if(jsonObj.contains("debug")) {
                QJsonValue v = jsonObj.value("debug");
                debug = v.toBool();
            }
            if(jsonObj.contains("xbase")) {
                QJsonValue v = jsonObj.value("xbase");
                xbase = v.toInt();
            }
            if(jsonObj.contains("scope_sensitive")) {
                QJsonValue v = jsonObj.value("scope_sensitive");
                scope_sensitive = v.toDouble(35.0);
            }
            if(jsonObj.contains("y_sensitive")) {
                QJsonValue v = jsonObj.value("y_sensitive");
                y_sensitive = v.toDouble(1.0);
            }
            if(jsonObj.contains("sleep")) {
                QJsonValue v = jsonObj.value("sleep");
                sleep = v.toInt(10);
            }
        }
    }

    this->ui->comboBox_adsmode->setCurrentIndex(adsmode);
    this->ui->checkBox_debug->setChecked(debug);
    this->ui->lineEdit_offset->setText(QString::number(xbase));
    this->ui->lineEdit_scopesensitive->setText(QString::number(scope_sensitive));
    this->ui->lineEdit_ysensitive->setText(QString::number(y_sensitive));
    this->ui->lineEdit_sleeptime->setText(QString::number(sleep));

}

void MacroConfigView::saveJson()
{
    QJsonObject jobj;

    jobj.insert("adsmode", QJsonValue(this->ui->comboBox_adsmode->currentIndex()));
    jobj.insert("debug", QJsonValue(this->ui->checkBox_debug->isChecked()));
    jobj.insert("xbase", QJsonValue(this->ui->lineEdit_offset->text().toInt()));
    jobj.insert("scope_sensitive", QJsonValue(this->ui->lineEdit_scopesensitive->text().toDouble()));
    jobj.insert("y_sensitive", QJsonValue(this->ui->lineEdit_ysensitive->text().toDouble()));
    jobj.insert("sleep", QJsonValue(this->ui->lineEdit_sleeptime->text().toInt()));
    QJsonDocument doc;
    doc.setObject(jobj);

    QFile jsonf(QApplication::applicationDirPath() + "/resource/config.json");
    if(jsonf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        jsonf.write(doc.toJson());
        jsonf.close();
    }
}

void MacroConfigView::saveLua()
{
    QString tmp = QDir::temp().absolutePath();
    QString configc_path = tmp + "/" + GlobalStatic::configc_name;
    QString rec_path = tmp + "/" + GlobalStatic::rec_name;
    QString weapon_path = tmp + "/" + GlobalStatic::weapon_name;
    configc_path = Utils::toLuaUnicode(configc_path);
    rec_path = Utils::toLuaUnicode(rec_path);
    weapon_path = Utils::toLuaUnicode(weapon_path);
    QFile f(tmp + "/" + GlobalStatic::configc_name);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if(this->ui->comboBox_driver->currentIndex() == 0) {
            f.write(this->macroConfig_ghub.arg(configc_path, rec_path, weapon_path,
                                               this->ui->lineEdit_sleeptime->text(),
                                               this->ui->comboBox_adsmode->currentText(),
                                               this->ui->lineEdit_scopesensitive->text() == "" ? "50.0" : this->ui->lineEdit_scopesensitive->text(),
                                               this->ui->lineEdit_ysensitive->text() == "" ? "1.0" : this->ui->lineEdit_ysensitive->text(),
                                               this->ui->checkBox_debug->isChecked()?"true":"false",
                                               this->ui->lineEdit_offset->text()
                                               ).toUtf8());
        }
        f.close();
    }
}

void MacroConfigView::on_pushButton_downloadmacro_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存脚本", "", "*.lua");

    if(!filename.isEmpty()) {
        QFile f(filename);
        if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QString luapath = QDir::temp().absolutePath() + "/" + GlobalStatic::configc_name;
            luapath = Utils::toLuaUnicode(luapath);
            if(this->ui->comboBox_driver->currentIndex() == 0) {
                f.write(this->macro_ghub.arg(luapath).toUtf8());
            }
            f.close();
        }
    }
}

void MacroConfigView::on_pushButton_apply_clicked()
{
    this->saveLua();
    this->saveJson();
}


void MacroConfigView::on_checkBox_loginet_clicked(bool checked)
{
    if (checked) {
        this->ui->lineEdit_loginet->setDisabled(true);
        this->logiNet->start(this->ui->lineEdit_loginet->text().toInt());
    } else {
        this->logiNet->stop();
        this->ui->lineEdit_loginet->setDisabled(false);
    }
}

