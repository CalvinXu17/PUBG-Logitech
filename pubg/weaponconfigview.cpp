#include "weaponconfigview.h"
#include "ui_weaponconfigview.h"
#include "recognizer.h"
#include "mainwindow.h"
#include <QPair>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPixmap>
#include <QIcon>
#include <QDir>

WeaponConfigView::WeaponConfigView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeaponConfigView)
{
    ui->setupUi(this);
    QDoubleValidator *dvalid = new QDoubleValidator(0.0, 100.0, 10, this);
    QIntValidator *ivalid = new QIntValidator(0, 999, this);
    this->ui->lineEdit_scope->setValidator(dvalid);
    this->ui->lineEdit_attach1->setValidator(dvalid);
    this->ui->lineEdit_attach2->setValidator(dvalid);
    this->ui->lineEdit_speed->setValidator(ivalid);
    this->ui->lineEdit_max->setValidator(ivalid);
    this->ui->lineEdit_pose->setValidator(dvalid);

    Recognizer *recog = Recognizer::getInstance();


    QList<RecognizeObject> *weapons = &recog->weapons;

    int scope_n = 0;
    int attach1_n = 0;
    int attach2_n = 0;
    int attach4_n = 0;
    for(int i=0; i < recog->scopes.size(); i++)
        if(recog->scopes.at(i).name.indexOf('_') == -1) scope_n++;
    for(int i=0; i < recog->attachment1s.size(); i++)
        if(recog->attachment1s.at(i).name.indexOf('_') == -1) attach1_n++;
    for(int i=0; i < recog->attachment2s.size(); i++)
        if(recog->attachment2s.at(i).name.indexOf('_') == -1) attach2_n++;
    for(int i=0; i < recog->attachment4s.size(); i++)
        if(recog->attachment4s.at(i).name.indexOf('_') == -1) attach4_n++;

    // init none weapon
    // none武器不压枪
    WeaponConfig none;
    none.name = "none";
    none.pose.append(QPair<QString, QString>({"0", "1.0"}));
    none.pose.append(QPair<QString, QString>({"1", "1.0"}));
    none.pose.append(QPair<QString, QString>({"2", "1.0"}));

    for(int i=0; i < scope_n; i++)
        none.scope.append(QPair<QString, QString>({QString::number(i), "1.0"}));
    for(int i=0; i < attach1_n; i++)
        none.attach1.append(QPair<QString, QString>({QString::number(i), "1.0"}));
    for(int i=0; i < attach2_n; i++)
        none.attach2.append(QPair<QString, QString>({QString::number(i), "1.0"}));
    for(int i=0; i < attach4_n; i++)
        none.attach4.append(QPair<QString, QString>({QString::number(i), "1.0"}));

    none.speed = "100";
    none.max = "40";
    none.ballistic = "{1,0},\n{40,0}";
    this->weaponConfigs.append(none);

    // read json saved config file
    bool json_ok = false;

    QString name = "weapon";
    QFile jsonf(QApplication::applicationDirPath() + "/resource/" + name + ".json");
    QJsonDocument document;
    QJsonObject jsonObj;
    if(jsonf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString jsonstr = jsonf.readAll();
        jsonf.close();
        QJsonParseError err;
        document = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
        if(err.error == QJsonParseError::NoError) {
            jsonObj = document.object();
            json_ok = true;
        }
    }

    if(recog->init_success) {
        // init exist weapon
        for(int i=0; i < weapons->size(); i++) {
            WeaponConfig w;
            w.name = weapons->at(i).name;
            w.img = weapons->at(i).gray_img;

            bool pose_init = false;
            bool scope_init = false;
            bool attach1_init = false;
            bool attach2_init = false;
            bool attach4_init = false;

            if(json_ok && jsonObj.contains(w.name)) { // 保存的json配置存在改weapon则读取保存的配置
                QJsonValue v = jsonObj.value(w.name);
                if(v.isObject()) { // 要求值为新的jsonObject
                    QJsonObject weaponobj = v.toObject();
                    QJsonValue vv;
                    if(weaponobj.contains("pose_sensitive")) {
                        vv = weaponobj.value("pose_sensitive");
                        if(vv.isArray()) {
                            QJsonArray arr = vv.toArray();
                            int n_min = MIN(arr.size(), 3); // pose最多3种
                            int j;
                            // ignored pose null 0
                            for(j=0; j<n_min; j++) {
                                w.pose.append(QPair<QString, QString>({QString::number(j+1), arr.at(j).toString()}));
                            }
                            for(; j<3; j++) // 不满默认填1.0
                                w.pose.append(QPair<QString, QString>({QString::number(j+1), "1.0"}));
                            pose_init = true;
                        }
                    }

                    if(weaponobj.contains("scope_sensitive")) {
                        vv = weaponobj.value("scope_sensitive");
                        if(vv.isArray()) {
                            QJsonArray arr = vv.toArray();
                            int n_min = MIN(arr.size(), scope_n);
                            int j;
                            for(j=0; j<n_min; j++) {
                                w.scope.append(QPair<QString, QString>({QString::number(j), arr.at(j).toString()}));
                            }
                            for(; j<scope_n; j++)
                                w.scope.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                            scope_init = true;
                        }
                    }

                    if(weaponobj.contains("a1_sensitive")) {
                        vv = weaponobj.value("a1_sensitive");
                        if(vv.isArray()) {
                            QJsonArray arr = vv.toArray();
                            int n_min = MIN(arr.size(), attach2_n);
                            int j;
                            for(j=0; j<n_min; j++) {
                                w.attach1.append(QPair<QString, QString>({QString::number(j), arr.at(j).toString()}));
                            }
                            for(; j<attach1_n; j++)
                                w.attach1.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                            attach1_init = true;
                        }
                    }

                    if(weaponobj.contains("a2_sensitive")) {
                        vv = weaponobj.value("a2_sensitive");
                        if(vv.isArray()) {
                            QJsonArray arr = vv.toArray();
                            int n_min = MIN(arr.size(), attach2_n);
                            int j;
                            for(j=0; j<n_min; j++) {
                                w.attach2.append(QPair<QString, QString>({QString::number(j), arr.at(j).toString()}));
                            }
                            for(; j<attach2_n; j++)
                                w.attach2.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                            attach2_init = true;
                        }
                    }

                    if(weaponobj.contains("a4_sensitive")) {
                        vv = weaponobj.value("a4_sensitive");
                        if(vv.isArray()) {
                            QJsonArray arr = vv.toArray();
                            int n_min = MIN(arr.size(), attach4_n);
                            int j;
                            for(j=0; j<n_min; j++) {
                                w.attach4.append(QPair<QString, QString>({QString::number(j), arr.at(j).toString()}));
                            }
                            for(; j<attach4_n; j++)
                                w.attach4.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                            attach4_init = true;
                        }
                    }

                    if(weaponobj.contains("speed")) {
                        vv = weaponobj.value("speed");
                        w.speed = vv.toString("");
                    }

                    if(weaponobj.contains("max")) {
                        vv = weaponobj.value("max");
                        w.max = vv.toString("");
                    }

                    if(weaponobj.contains("ballistic")) {
                        vv = weaponobj.value("ballistic");
                        w.ballistic += vv.toString("");
                    }
                }
            }
            if(!pose_init) {
                for(int j=0; j<3; j++) {
                    w.pose.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                }
            }
            if(!scope_init) {
                for(int j=0; j<scope_n; j++) {
                    w.scope.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                }
            }
            if(!attach1_init) {
                for(int j=0; j<attach1_n; j++) {
                    w.attach1.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                }
            }
            if(!attach2_init) {
                for(int j=0; j<attach2_n; j++) {
                    w.attach2.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                }
            }
            if(!attach4_init) {
                for(int j=0; j<attach4_n; j++) {
                    w.attach4.append(QPair<QString, QString>({QString::number(j), "1.0"}));
                }
            }
            this->weaponConfigs.append(w);
        }

        this->ui->comboBox_weapon->setIconSize(QSize(70,20));
        // ignore weapon none
        for(int i=1;i<this->weaponConfigs.size(); i++) {
            this->ui->comboBox_weapon->addItem(QIcon(QPixmap::fromImage(this->weaponConfigs.at(i).img)), this->weaponConfigs.at(i).name);
        }

        this->ui->comboBox_scope->setIconSize(QSize(30,30));
        for(int i=0;i<recog->scopes.size(); i++) {
            this->ui->comboBox_scope->addItem(QIcon(QPixmap::fromImage(recog->scopes.at(i).gray_img)), recog->scopes.at(i).name);
        }

        this->ui->comboBox_attach1->setIconSize(QSize(30,30));
        for(int i=0;i<recog->attachment1s.size(); i++) {
            if(recog->attachment1s.at(i).name.indexOf('_') == -1)
                this->ui->comboBox_attach1->addItem(QIcon(QPixmap::fromImage(recog->attachment1s.at(i).gray_img)), recog->attachment1s.at(i).name);
        }

        this->ui->comboBox_attach2->setIconSize(QSize(30,30));
        for(int i=0;i<recog->attachment2s.size(); i++) {
            if(recog->attachment2s.at(i).name.indexOf('_') == -1)
                this->ui->comboBox_attach2->addItem(QIcon(QPixmap::fromImage(recog->attachment2s.at(i).gray_img)), recog->attachment2s.at(i).name);
        }

        this->ui->comboBox_attach4->setIconSize(QSize(30,30));
        for(int i=0;i<recog->attachment4s.size(); i++) {
            if(recog->attachment4s.at(i).name.indexOf('_') == -1)
                this->ui->comboBox_attach4->addItem(QIcon(QPixmap::fromImage(recog->attachment4s.at(i).gray_img)), recog->attachment4s.at(i).name);
        }

        this->ui->comboBox_pose->setIconSize(QSize(30,30));
        // ignore pose none
        for(int i=1;i<=3; i++) {
            this->ui->comboBox_pose->addItem(QIcon(QPixmap::fromImage(recog->poses.at(i).gray_img)), recog->poses.at(i).name);
        }
        if(this->weaponConfigs.size() > 1) {
            this->weapon_index = 1;
        } else
            this->weapon_index = 0;


        this->setInfoByIndex(this->weapon_index);
        this->saveWeaponLua();

        this->init_success = true;
    }


}

WeaponConfigView::~WeaponConfigView()
{
    delete ui;
}

void WeaponConfigView::setInfoByIndex(int index)
{
    WeaponConfig curw = this->weaponConfigs.at(index);
    this->ui->lineEdit_scope->setText(curw.getStringByName(this->ui->comboBox_scope->currentText(), curw.scope));
    this->ui->lineEdit_attach1->setText(curw.getStringByName(this->ui->comboBox_attach1->currentText(), curw.attach1));
    this->ui->lineEdit_attach2->setText(curw.getStringByName(this->ui->comboBox_attach2->currentText(), curw.attach2));
    this->ui->lineEdit_attach4->setText(curw.getStringByName(this->ui->comboBox_attach4->currentText(), curw.attach4));
    this->ui->lineEdit_pose->setText(curw.getStringByName(this->ui->comboBox_pose->currentText(), curw.pose));

    this->ui->lineEdit_speed->setText(curw.speed);
    this->ui->lineEdit_max->setText(curw.max);

    this->ui->textEdit_ballistic->setText(curw.ballistic);
}

void WeaponConfigView::saveInfoByIndex(int index)
{
    auto &curw = this->weaponConfigs[index];
    curw.setStringByName(this->ui->comboBox_scope->currentText(), this->ui->lineEdit_scope->text(), curw.scope);
    curw.setStringByName(this->ui->comboBox_attach1->currentText(), this->ui->lineEdit_attach1->text(), curw.attach1);
    curw.setStringByName(this->ui->comboBox_attach2->currentText(), this->ui->lineEdit_attach2->text(), curw.attach2);
    curw.setStringByName(this->ui->comboBox_attach4->currentText(), this->ui->lineEdit_attach4->text(), curw.attach4);
    curw.setStringByName(this->ui->comboBox_pose->currentText(), this->ui->lineEdit_pose->text(), curw.pose);
    curw.speed = this->ui->lineEdit_speed->text();
    curw.max = this->ui->lineEdit_max->text();
    curw.ballistic = this->ui->textEdit_ballistic->toPlainText();
}

void WeaponConfigView::saveWeaponLua(void)
{
    // save lua file to Temp path
    QFile luaf(QDir::temp().absolutePath() + "/weapon.lua");
    if(luaf.open(QIODevice::WriteOnly)) {
        for(int i=0; i<this->weaponConfigs.size(); i++) {
            luaf.write(this->weaponConfigs[i].toLuaCode().toLocal8Bit());
        }
        luaf.close();
    }
}

void WeaponConfigView::on_comboBox_weapon_currentIndexChanged(int index)
{
    if(init_success) {
        this->saveInfoByIndex(this->weapon_index);
        setInfoByIndex(index+1); // 切换武器信息
    }
    this->weapon_index = index + 1; // 0为none武器，none武器未添加进comboBox
}


void WeaponConfigView::on_comboBox_scope_currentIndexChanged(int index)
{
    if(init_success) {
        auto &curw = this->weaponConfigs[this->weapon_index];
        curw.setStringByName(this->ui->comboBox_scope->itemText(this->scope_index), this->ui->lineEdit_scope->text(), curw.scope);
        this->ui->lineEdit_scope->setText(curw.getStringByName(this->ui->comboBox_scope->itemText(index), curw.scope));
    }
    this->scope_index = index;

}


void WeaponConfigView::on_comboBox_attach1_currentIndexChanged(int index)
{
    if(init_success) {
        auto &curw = this->weaponConfigs[this->weapon_index];
        curw.setStringByName(this->ui->comboBox_attach1->itemText(this->attach1_index), this->ui->lineEdit_attach1->text(), curw.attach1);
        this->ui->lineEdit_attach1->setText(curw.getStringByName(this->ui->comboBox_attach1->itemText(index), curw.attach1));
    }
    this->attach1_index = index;
}


void WeaponConfigView::on_comboBox_attach2_currentIndexChanged(int index)
{
    if(init_success) {
        auto &curw = this->weaponConfigs[this->weapon_index];
        curw.setStringByName(this->ui->comboBox_attach2->itemText(this->attach2_index), this->ui->lineEdit_attach2->text(), curw.attach2);
        this->ui->lineEdit_attach2->setText(curw.getStringByName(this->ui->comboBox_attach2->itemText(index), curw.attach2));
        this->attach2_index = index;
    }
}

void WeaponConfigView::on_comboBox_attach4_currentIndexChanged(int index)
{
    if(init_success) {
        auto &curw = this->weaponConfigs[this->weapon_index];
        curw.setStringByName(this->ui->comboBox_attach4->itemText(this->attach4_index), this->ui->lineEdit_attach4->text(), curw.attach4);
        this->ui->lineEdit_attach4->setText(curw.getStringByName(this->ui->comboBox_attach4->itemText(index), curw.attach4));
        this->attach4_index = index;
    }
}

void WeaponConfigView::on_comboBox_pose_currentIndexChanged(int index)
{
    if(init_success) {
        auto &curw = this->weaponConfigs[this->weapon_index];
        curw.setStringByName(this->ui->comboBox_pose->itemText(this->pose_index), this->ui->lineEdit_pose->text(), curw.pose);
        this->ui->lineEdit_pose->setText(curw.getStringByName(this->ui->comboBox_pose->itemText(index), curw.pose));
        this->pose_index = index;
    }
}


void WeaponConfigView::on_pushButton_delete_clicked()
{
    this->ui->lineEdit_scope->setText("");
    this->ui->lineEdit_attach1->setText("");
    this->ui->lineEdit_attach2->setText("");
    this->ui->lineEdit_attach4->setText("");
    this->ui->lineEdit_speed->setText("");
    this->ui->lineEdit_max->setText("");
    this->ui->lineEdit_pose->setText("");
    this->ui->textEdit_ballistic->setText("");
}

void WeaponConfigView::on_pushButton_save_clicked()
{
    if(init_success) {
        this->saveInfoByIndex(this->weapon_index);

        // save json file to current path
        QJsonObject jobj;
        for(int i=1; i<this->weaponConfigs.size(); i++) {
            jobj.insert(this->weaponConfigs[i].name, this->weaponConfigs[i].toJsonObject());
        }
        QJsonDocument doc;
        doc.setObject(jobj);
        QFile jsonf(QApplication::applicationDirPath() + "/resource/weapon.json");
        if(jsonf.open(QIODevice::WriteOnly | QIODevice::Text)) {
            jsonf.write(doc.toJson());
            jsonf.close();
        }

        this->saveWeaponLua();
    }
}

