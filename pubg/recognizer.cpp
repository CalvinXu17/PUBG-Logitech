#include "recognizer.h"
#include "cvutils.h"
#include "globalstatic.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <windows.h>
#include <QApplication>

extern "C" {
    __declspec(dllexport) struct RecognizerData RecogData;
    __declspec(dllexport) int RecogFPS = 10;
}

Recognizer::Recognizer(QObject *parent) : QObject(parent)
{
    this->init_success = false;
    this->needupdate = true;

    QString pwd = QApplication::applicationDirPath();

    HWND hwnd = GetDesktopWindow();
    cv::Mat screen = CVUtils::screenShotGDI(hwnd);
    int width = screen.cols;
    int height = screen.rows;
    this->screen_width = width;
    this->screen_height = height;
//    width = 3440;
//    height = 1440;
    QString resource_path = "/resource/%1_%2";
    resource_path = pwd + resource_path.arg(width).arg(height);
    QDir resource_dir(resource_path);
    if (!resource_dir.exists()) {
        QMessageBox msg;
        msg.setText(QString("暂不支持分辨率%1x%2!").arg(width).arg(height));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QString resolution_file_path =  "/resource/resolution/%1_%2.json";
    resolution_file_path = resolution_file_path.arg(width).arg(height);
    QFile jsonf(pwd + resolution_file_path);
    if(!jsonf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox msg;
        msg.setText(QString("暂不支持分辨率%1x%2!").arg(width).arg(height));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QString jsonstr = jsonf.readAll();
    jsonf.close();
    QJsonParseError err;
    QJsonDocument document = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
    if(err.error != QJsonParseError::NoError) {
        qWarning() << "parse Json failed!";
        return;
    }

    QJsonObject jsonObj = document.object();

    if(jsonObj.contains("weapon2")) {
        QJsonValue v = jsonObj.value("weapon2");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->weapon2_x = arr.at(0).toInt(0);
            this->weapon2_y = arr.at(1).toInt(0);
            this->weapon2_width = arr.at(2).toInt(0);
            this->weapon2_height = arr.at(3).toInt(0);
        } else {
            qWarning() << "weapon2 is not array!";
            return;
        }
    } else {
        qWarning() << "Json has no key: weapon2";
        return;
    }

    if(jsonObj.contains("weapon1")) {
        QJsonValue v = jsonObj.value("weapon1");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->weapon1_x = arr.at(0).toInt(0);
            this->weapon1_y = arr.at(1).toInt(0);
            this->weapon1_width = arr.at(2).toInt(0);
            this->weapon1_height = arr.at(3).toInt(0);
        } else {
            qWarning() << "weapon1 is not array!";
            return;
        }
    } else {
        qWarning() << "Json has no key: weapon1";
        return;
    }


    if(jsonObj.contains("pose")) {
        QJsonValue v = jsonObj.value("pose");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->pose_x = arr.at(0).toInt(0);
            this->pose_y = arr.at(1).toInt(0);
            this->pose_width = arr.at(2).toInt(0);
            this->pose_height = arr.at(3).toInt(0);
        } else {
            qWarning() << "pose is not array";
            return;
        }
    } else {
        qWarning() << "Json has no key: pose";
        return;
    }


    if(jsonObj.contains("bp1")) {
        QJsonValue v = jsonObj.value("bp1");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->bp1_x = arr.at(0).toInt(0);
            this->bp1_y = arr.at(1).toInt(0);
            this->bp_width = arr.at(2).toInt(0);
            this->bp_height = arr.at(3).toInt(0);
        } else return;
    } else return;

    if(jsonObj.contains("bp2")) {
        QJsonValue v = jsonObj.value("bp2");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->bp2_x = arr.at(0).toInt(0);
            this->bp2_y = arr.at(1).toInt(0);
            this->bp_width = arr.at(2).toInt(0);
            this->bp_height = arr.at(3).toInt(0);
        } else return;
    } else return;

    if(jsonObj.contains("w1")) {
        QJsonValue v = jsonObj.value("w1");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1_x = arr.at(0).toInt(0);
            this->w1_y = arr.at(1).toInt(0);
            this->w1_width = arr.at(2).toInt(0);
            this->w1_height = arr.at(3).toInt(0);
        } else return;
    } else return;

    if(jsonObj.contains("w2")) {
        QJsonValue v = jsonObj.value("w2");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2_x = arr.at(0).toInt(0);
            this->w2_y = arr.at(1).toInt(0);
            this->w2_width = arr.at(2).toInt(0);
            this->w2_height = arr.at(3).toInt(0);
        } else return;
    } else return;

    if(jsonObj.contains("w1scope")) {
        QJsonValue v = jsonObj.value("w1scope");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1scope_x = arr.at(0).toInt();
            this->w1scope_y = arr.at(1).toInt();
            this->w1scope_width = arr.at(2).toInt(0);
            this->w1scope_height = arr.at(3).toInt(0);
        } else return;
    } else return;

    if(jsonObj.contains("w1a1")) {
        QJsonValue v = jsonObj.value("w1a1");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1a1_x = arr.at(0).toInt(0);
            this->w1a1_y = arr.at(1).toInt(0);
            this->w1a1_width = arr.at(2).toInt(0);;
            this->w1a1_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w1a2")) {
        QJsonValue v = jsonObj.value("w1a2");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1a2_x = arr.at(0).toInt(0);
            this->w1a2_y = arr.at(1).toInt(0);
            this->w1a2_width = arr.at(2).toInt(0);;
            this->w1a2_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w1a3")) {
        QJsonValue v = jsonObj.value("w1a3");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1a3_x = arr.at(0).toInt(0);
            this->w1a3_y = arr.at(1).toInt(0);
            this->w1a3_width = arr.at(2).toInt(0);;
            this->w1a3_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w1a4")) {
        QJsonValue v = jsonObj.value("w1a4");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w1a4_x = arr.at(0).toInt(0);
            this->w1a4_y = arr.at(1).toInt(0);
            this->w1a4_width = arr.at(2).toInt(0);;
            this->w1a4_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w2scope")) {
        QJsonValue v = jsonObj.value("w2scope");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2scope_x = arr.at(0).toInt(0);
            this->w2scope_y = arr.at(1).toInt(0);
            this->w2scope_width = arr.at(2).toInt(0);;
            this->w2scope_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w2a1")) {
        QJsonValue v = jsonObj.value("w2a1");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2a1_x = arr.at(0).toInt(0);
            this->w2a1_y = arr.at(1).toInt(0);
            this->w2a1_width = arr.at(2).toInt(0);;
            this->w2a1_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w2a2")) {
        QJsonValue v = jsonObj.value("w2a2");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2a2_x = arr.at(0).toInt(0);
            this->w2a2_y = arr.at(1).toInt(0);
            this->w2a2_width = arr.at(2).toInt(0);;
            this->w2a2_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w2a3")) {
        QJsonValue v = jsonObj.value("w2a3");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2a3_x = arr.at(0).toInt(0);
            this->w2a3_y = arr.at(1).toInt(0);
            this->w2a3_width = arr.at(2).toInt(0);;
            this->w2a3_height = arr.at(3).toInt(0);;
        } else return;
    } else return;

    if(jsonObj.contains("w2a4")) {
        QJsonValue v = jsonObj.value("w2a4");
        if(v.isArray()) {
            QJsonArray arr = v.toArray();
            if(arr.size() < 4) return;
            this->w2a4_x = arr.at(0).toInt(0);
            this->w2a4_y = arr.at(1).toInt(0);
            this->w2a4_width = arr.at(2).toInt(0);;
            this->w2a4_height = arr.at(3).toInt(0);;
        } else return;
    } else return;


    qDebug() << "Parse Json success!";

    this->currentweapon = 1;

    this->weapon1 = "none";
    this->w1scope = 0;
    this->w1a1 = 0; // no a1
    this->w1a2 = 0; // no a2

    this->weapon2 = "none";
    this->w2scope = 0;
    this->w2a1 = 0; // no a1
    this->w2a2 = 0; // no a2


    ///////////////////////
    this->rweapon = "none";
    this->rpose = 1; // 默认蹲姿
    this->rx = 0;
    this->ry = 0;
    this->rscope = 0;
    this->rattachment1 = 0;
    this->rattachment2 = 0;
    this->rattachment3 = 0;
    this->rattachment4 = 0;

    this->fps = 16;

    QDir gun_dir(resource_path + "/gun");
    if(gun_dir.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList guns = gun_dir.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < guns.size(); i++) {
            QString filename = guns[i];
            QString gun_name;
            int index = filename.indexOf('.');
            if(index == -1) gun_name = filename;
            else {
                gun_name = filename.left(index);
            }
            cv::Mat p_img = cv::imread((gun_dir.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            // this->weapons.append(RecognizeObject(gun_name, p_img, 0, 16));
            // 由原来枪械图片识别改为背包枪械名字识别
            this->weapons.append(RecognizeObject(gun_name, p_img, 0)); // guns don't need id
        }
    } else return;

    QDir pose_dir(resource_path + "/pose");
    if(pose_dir.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList p = pose_dir.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < p.size(); i++) {
            QString filename = p[i];
            QString p_name;
            int index = filename.indexOf('.');
            if(index == -1) p_name = filename;
            else {
                p_name = filename.left(index);
            }
            cv::Mat p_img = cv::imread((pose_dir.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            this->poses.append(RecognizeObject(p_name, p_img, p_name.toInt()));
        }
    } else return;

    qDebug() << "Pose init success!";

    QDir scope_dir(resource_path + "/scope");
    if(scope_dir.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList p = scope_dir.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < p.size(); i++) {
            QString filename = p[i];
            QString p_name;
            int index = filename.indexOf('.');
            if(index == -1) p_name = filename;
            else {
                p_name = filename.left(index);
            }

            int id = 0;
            if(p_name.indexOf('_') == -1) {
                id = p_name.toInt();
            } else {
                id = p_name.left(p_name.indexOf('_')).toInt();
            }
            cv::Mat p_img = cv::imread((scope_dir.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            this->scopes.append(RecognizeObject(p_name, p_img, id)); // index 0 is ignored when saving to lua
        }
    } else return;

    qDebug() << "Scope init success!";

    QDir attachs1(resource_path + "/attachments/1");
    if(attachs1.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList p = attachs1.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < p.size(); i++) {
            QString filename = p[i];
            QString p_name;
            int index = filename.indexOf('.');
            if(index == -1) p_name = filename;
            else {
                p_name = filename.left(index);
            }

            int id = 0;
            if(p_name.indexOf('_') == -1) {
                id = p_name.toInt();
            } else {
                id = p_name.left(p_name.indexOf('_')).toInt();
            }
            cv::Mat p_img = cv::imread((attachs1.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            this->attachment1s.append(RecognizeObject(p_name, p_img, id)); // index 0 is needed, in lua the first item's index is 1
        }
    } else return;

    qDebug() << "a1 init success!";

    QDir attachs2(resource_path + "/attachments/2");
    if(attachs2.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList p = attachs2.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < p.size(); i++) {
            QString filename = p[i];
            QString p_name;
            int index = filename.indexOf('.');
            if(index == -1) p_name = filename;
            else {
                p_name = filename.left(index);
            }

            int id = 0;
            if(p_name.indexOf('_') == -1)
            {
                id = p_name.toInt();
            } else {
                id = p_name.left(p_name.indexOf('_')).toInt();
            }
            cv::Mat p_img = cv::imread((attachs2.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            this->attachment2s.append(RecognizeObject(p_name, p_img, id)); // index 0 is needed, in lua the first item's index is 1
        }
    } else return;

    qDebug() << "a2 init success!";

    QDir attachs4(resource_path + "/attachments/4");
    if(attachs4.exists()) {
        QStringList filters;
        filters << "*.png";
        QStringList p = attachs4.entryList(filters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i=0; i < p.size(); i++) {
            QString filename = p[i];
            QString p_name;
            int index = filename.indexOf('.');
            if(index == -1) p_name = filename;
            else {
                p_name = filename.left(index);
            }

            int id = 0;
            if(p_name.indexOf('_') == -1)
            {
                id = p_name.toInt();
            } else {
                id = p_name.left(p_name.indexOf('_')).toInt();
            }
            cv::Mat p_img = cv::imread((attachs4.path() + "/" + filename).toLocal8Bit().toStdString());
            if(p_img.empty()) return;
#if BPIX == 4
            if (p_img.channels() == 3)
                cv::cvtColor(p_img, p_img, cv::COLOR_BGR2BGRA);
#endif
            this->attachment4s.append(RecognizeObject(p_name, p_img, id)); // index 0 is needed, in lua the first item's index is 1
        }
    } else return;

    qDebug() << "a4 init success!";

    // 背包数字根据height分辨率不同选择不同的图标
    QString bpp = resource_path + "/bp/bp1.png";
    cv::Mat bpp_img = cv::imread(bpp.toLocal8Bit().toStdString());
    if(bpp_img.empty()) return;
    this->bps.append(RecognizeObject("bp1", bpp_img, 1));

    bpp = resource_path + "/bp/bp2.png";
    bpp_img = cv::imread(bpp.toLocal8Bit().toStdString());
    if(bpp_img.empty()) return;
    this->bps.append(RecognizeObject("bp2", bpp_img, 2));

    QFile f(":/rec.lua");
    if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        this->luaCode = f.readAll();
        f.close();
    }

    this->isrunning = false;
    this->bloodrecog = new BloodRecognize(0);
    this->init_success = true;
}

Recognizer *Recognizer::getInstance()
{
    static Recognizer recogInstance(NULL);
    return &recogInstance;
}

Recognizer::~Recognizer()
{
    this->stop();
    if(this->pthread) {
        if(this->pthread->isRunning()) {
            this->pthread->quit();
            this->pthread->wait();
        }
        disconnect(this->pthread, SIGNAL(started()), this, SLOT(recognizeLoop()));
        delete this->pthread;
        this->pthread = NULL;
    }
    delete bloodrecog;
}

void Recognizer::start()
{
    if(this->pthread && this->isrunning)
        return;

    if(this->pthread == NULL) {
        this->pthread = new QThread();
        this->moveToThread(this->pthread);
        connect(this->pthread, SIGNAL(started()), this, SLOT(recognizeLoop()));
    }
    this->isrunning = true;
    this->pthread->start();
}

void Recognizer::stop()
{
    if(this->pthread && this->isrunning) {
        this->isrunning = false;
        this->pthread->wait(10);
        this->pthread->quit();
    }
}


void Recognizer::setFPS(int fps)
{
    this->fps = fps;
    RecogFPS = fps;
}

bool Recognizer::isBagOpen(cv::Mat &screen)
{
    if(this->bps.size() < 2) return false;

    cv::Mat bp1 = CVUtils::cropMat(screen, this->bp1_x, this->bp1_y, this->bp_width, this->bp_height);
    cv::Mat bp2 = CVUtils::cropMat(screen, this->bp2_x, this->bp2_y, this->bp_width, this->bp_height);

    int d1 = CVUtils::HammingDistance(CVUtils::pHash(bp1), this->bps.at(0).phash);
    int d2 = CVUtils::HammingDistance(CVUtils::pHash(bp2), this->bps.at(1).phash);
    if (screen.cols != 1920) {
        this->bag1 = d1 < 10;
        this->bag2 = d2 < 10;
    } else {
        this->bag1 = d1 < 6;
        this->bag2 = d2 < 6;
    }
    return this->bag1 || this->bag2;
}

void Recognizer::poseRecognize(cv::Mat &screen)
{
    cv::Mat pose = CVUtils::cropMat(screen, this->pose_x, this->pose_y, this->pose_width, this->pose_height);

    CVUtils::to_black_white_pose(pose, false);

    QString hash = CVUtils::pHash(pose);
    int len = this->poses.size();
    for (int i = 0; i < len; i++)
    {
        if(CVUtils::HammingDistance(this->poses.at(i).phash, hash) < 15) {
            int id = this->poses.at(i).id;
            // 长按开镜不能返回0
            if(!this->scope_mode && !id) continue;

            if(this->rpose != id) {
                this->rpose = id;
                this->needupdate = true;
            }
            return;
        }
    }

    // 单击开镜
    if(this->scope_mode) {
        if(this->rpose != 0) {
            this->rpose = 0;
            this->needupdate = true;
        }
    } else {
        // 长按开镜
        if(this->rscope != 1 && this->rpose != 1) {
            this->rpose = 1;
            this->needupdate = true;
        }
    }
}

// deprecated
bool Recognizer::isWeaponRed(cv::Mat &img)
{
    int sum = 0;
    for (int row = 0; row < img.rows; row++) {
        uchar* ptr = img.ptr<uchar>(row);
        for (int col = 0; col < img.cols*BPIX; col+=BPIX) {
            uchar b = ptr[col+0];
            uchar g = ptr[col+1];
            uchar r = ptr[col+2];
            if(r > 175 && g < 50 && b < 50) {
                ptr[col+0] = ptr[col+1] = ptr[col+2] = 255;
                sum++;
            } else {
                ptr[col+0] = ptr[col+1] = ptr[col+2] = 0;
            }
        }
    }
    return sum >= 100;
}

// deprecated
bool Recognizer::isWeaponWhite(float r, float g, float b)
{
    return (abs(r-g) <= 20 && abs(r-b) <= 20 && abs(g-b) <= 20 && (r + g + b) / 3 > 150.0);
}

// deprecated
bool Recognizer::isWeaponActivated(float r, float g, float b)
{
    return (r+g+b) / 3 >= 215.0;
}

// deprecated
void Recognizer::weaponRecognize(cv::Mat &screen)
{

    cv::Mat w1 = CVUtils::cropMat(screen, this->weapon1_x, this->weapon1_y, this->weapon1_width, this->weapon1_height);
    cv::Mat w2 = CVUtils::cropMat(screen, this->weapon2_x, this->weapon2_y, this->weapon2_width, this->weapon2_height);

    CVUtils::to_black_white_avg(w1, false);
    CVUtils::to_black_white_avg(w2, false);

    QString hash1 = CVUtils::pHashN(w1, 16);
    QString hash2 = CVUtils::pHashN(w2, 16);

    bool b1 = false;
    bool b2 = false;

    int d1 = 80;
    int d2 = 80;

    int len = this->weapons.size();
    for(int i=0; i<len; i++) {
        QString phash = this->weapons.at(i).phash;
        int d = CVUtils::HammingDistance(phash, hash1);
        // qDebug() << this->weapons.at(i).name << d;
        if(d < d1) {
            this->weapon1 = this->weapons.at(i).name;
            b1 = true;
            d1 = d;
        }
        d = CVUtils::HammingDistance(phash, hash2);
        if(d < d2) {
            this->weapon2 = this->weapons.at(i).name;
            b2 = true;
            d2 = d;
        }
       // if(b1 && b2) break;
    }

    if(b1 && !b2) {
        this->currentweapon = 1;
    } else if(b2 && !b1) {
        this->currentweapon = 2;
    } else if(b1 && b2) {
        if(d1 < d2) {
            this->currentweapon = 1;
        } else {
            this->currentweapon = 2;
        }
    } else {
        if(this->scope_mode && this->rweapon != "none") {
            this->rweapon = "none";
            this->needupdate = true;
        }
    }
    if (this->currentweapon == 1) {
        if(this->rweapon != this->weapon1) {
            this->rweapon = this->weapon1;
            this->needupdate = true;
        }
        if(this->rscope != this->w1scope) {
            this->rscope = this->w1scope;
            this->needupdate = true;
        }
        if(this->rattachment1 != this->w1a1) {
            this->rattachment1 = this->w1a1;
            this->needupdate = true;
        }
        if(this->rattachment2 != this->w1a2) {
            this->rattachment2 = this->w1a2;
            this->needupdate = true;
        }
        if(this->rattachment4 != this->w1a4) {
            this->rattachment4 = this->w1a4;
            this->needupdate = true;
        }
    } else {
        if(this->rweapon != this->weapon2)
        {
            this->rweapon = this->weapon2;
            this->needupdate = true;
        }
        if(this->rscope != this->w2scope)
        {
            this->rscope = this->w2scope;
            this->needupdate = true;
        }
        if(this->rattachment1 != this->w2a1)
        {
            this->rattachment1 = this->w2a1;
            this->needupdate = true;
        }
        if(this->rattachment2 != this->w2a2)
        {
            this->rattachment2 = this->w2a2;
            this->needupdate = true;
        }
        if(this->rattachment4 != this->w2a4)
        {
            this->rattachment4 = this->w2a4;
            this->needupdate = true;
        }
    }
}

int Recognizer::weaponChoose(cv::Mat &screen)
{

    cv::Mat w1 = CVUtils::cropMat(screen, this->weapon1_x, this->weapon1_y, this->weapon1_width, this->weapon1_height);
    cv::Mat w2 = CVUtils::cropMat(screen, this->weapon2_x, this->weapon2_y, this->weapon2_width, this->weapon2_height);

    CVUtils::to_black_white_avg(w1, false);
    CVUtils::to_black_white_avg(w2, false);

    int n1 = 0;
    int n2 = 0;

    for (int row = 0; row < w1.rows; row++) {
        uchar* ptr = w1.ptr<uchar>(row);
        for (int col = 0; col < w1.cols*BPIX; col+=BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            if (b == 255 && g == 255 && r == 255)
                n1++;
        }
    }

    for (int row = 0; row < w2.rows; row++) {
        uchar* ptr = w2.ptr<uchar>(row);
        for (int col = 0; col < w2.cols*BPIX; col+=BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            if (b == 255 && g == 255 && r == 255)
                n2++;
        }
    }
    if (n1 > n2 && n1 - n2 > 100)
        return 1;
    else if(n1 < n2 && n2 - n1 > 100)
        return 2;
    else
        return 0;
}

int Recognizer::weaponNameRecognize(cv::Mat &screen, int x, int y, int width, int height)
{

    cv::Mat w = CVUtils::cropMat(screen, x, y, width, height);

    CVUtils::to_black_white_above200(w, false);

    QString hash1 = CVUtils::pHash(w);

    int windex = -1;

    int d1 = 8;

    int len = this->weapons.size();
    for(int i=0; i<len; i++) {
        QString phash = this->weapons.at(i).phash;
        int d = CVUtils::HammingDistance(phash, hash1);
        if(d < d1) {
            windex = i;
            d1 = d;
        }
    }
    return windex;
}

void Recognizer::scopeRecognize(cv::Mat &screen)
{
    cv::Mat w1scope = CVUtils::cropMat(screen, this->w1scope_x, this->w1scope_y, this->w1scope_width, this->w1scope_height);
    cv::Mat w2scope = CVUtils::cropMat(screen, this->w2scope_x, this->w2scope_y, this->w2scope_width, this->w2scope_height);

    CVUtils::to_black_white(w1scope, false);
    CVUtils::to_black_white(w2scope, false);

    QString hash1 = CVUtils::pHash(w1scope);
    QString hash2 = CVUtils::pHash(w2scope);

    int len = this->scopes.size();

    bool b1 = false;
    bool b2 = false;

    int d1 = 20;
    int d2 = 20;

    for(int i=0; i<len; i++) {
        int d;

        d = CVUtils::HammingDistance(hash1, this->scopes.at(i).phash);
        if(d < 15 && d <= d1) {
            b1 = true;
            d1 = d;
            this->w1scope = this->scopes.at(i).id;
        }

        d = CVUtils::HammingDistance(hash2, this->scopes.at(i).phash);
        if(d < 15 && d <= d2) {
            b2 = true;
            d2 = d;
            this->w2scope = this->scopes.at(i).id;
        }

//        if(b1 && b2)
//            break;
    }

    int s;
    if(this->currentweapon == 1) {
        s = this->w1scope;
    } else {
        s = this->w2scope;
    }

    if(this->rscope != s) {
        this->rscope = s;
        this->needupdate = true;
    }
}

void Recognizer::attachRecognize(cv::Mat &screen)
{
    cv::Mat a11 = CVUtils::cropMat(screen, this->w1a1_x, this->w1a1_y, this->w1a1_width, this->w1a1_height);
    CVUtils::to_black_white(a11, false);
    QString hash11 = CVUtils::pHash(a11);

    cv::Mat a21 = CVUtils::cropMat(screen, this->w2a1_x, this->w2a1_y, this->w2a1_width, this->w2a1_height);
    CVUtils::to_black_white(a21, false);
    QString hash21 = CVUtils::pHash(a21);

    cv::Mat a12 = CVUtils::cropMat(screen, this->w1a2_x, this->w1a2_y, this->w1a2_width, this->w1a2_height);
    CVUtils::to_black_white(a12, false);
    QString hash12 = CVUtils::pHash(a12);

    cv::Mat a22 = CVUtils::cropMat(screen, this->w2a2_x, this->w2a2_y, this->w2a2_width, this->w2a2_height);
    CVUtils::to_black_white(a22, false);
    QString hash22 = CVUtils::pHash(a22);

    cv::Mat a14 = CVUtils::cropMat(screen, this->w1a4_x, this->w1a4_y, this->w1a4_width, this->w1a4_height);
    CVUtils::to_black_white(a14, false);
    QString hash14 = CVUtils::pHash(a14);

    cv::Mat a24 = CVUtils::cropMat(screen, this->w2a4_x, this->w2a4_y, this->w2a4_width, this->w2a4_height);
    CVUtils::to_black_white(a24, false);
    QString hash24 = CVUtils::pHash(a24);

    int d1 = 20;
    int d2 = 20;

    for(int i=0; i<this->attachment1s.size(); i++) {
        int d = CVUtils::HammingDistance(hash11, this->attachment1s.at(i).phash);

        if(d < d1) {
            d1 = d;
            this->w1a1 = this->attachment1s.at(i).id;
        }
        d = CVUtils::HammingDistance(hash21, this->attachment1s.at(i).phash);
        if(d < d2) {
            d2 = d;
            this->w2a1 = this->attachment1s.at(i).id;
        }
    }

    d1 = 15;
    d2 = 15;

    for(int i=0; i<this->attachment2s.size(); i++) {
        int d = CVUtils::HammingDistance(hash12, this->attachment2s.at(i).phash);
        if(d < d1) {
            d1 = d;
            this->w1a2 = this->attachment2s.at(i).id;
        }

        d = CVUtils::HammingDistance(hash22, this->attachment2s.at(i).phash);
        if(d < d2) {
            d2 = d;
            this->w2a2 = this->attachment2s.at(i).id;
        }
    }

    d1 = 15;
    d2 = 15;

    for(int i=0; i<this->attachment4s.size(); i++) {
        int d = CVUtils::HammingDistance(hash14, this->attachment4s.at(i).phash);
        if(d < d1) {
            d1 = d;
            this->w1a4 = this->attachment4s.at(i).id;
        }

        d = CVUtils::HammingDistance(hash24, this->attachment4s.at(i).phash);
        if(d < d2) {
            d2 = d;
            this->w2a4 = this->attachment4s.at(i).id;
        }
    }

    if(this->currentweapon == 1) {
        if(this->rattachment1 != this->w1a1) {
            this->rattachment1 = this->w1a1;
            this->needupdate = true;
        }
        if(this->rattachment2 != this->w1a2) {
            this->rattachment2 = this->w1a2;
            this->needupdate = true;
        }
        if(this->rattachment4 != this->w1a4) {
            this->rattachment4 = this->w1a4;
            this->needupdate = true;
        }
    } else {
        if(this->rattachment1 != this->w2a1) {
            this->rattachment1 = this->w2a1;
            this->needupdate = true;
        }
        if(this->rattachment2 != this->w2a2) {
            this->rattachment2 = this->w2a2;
            this->needupdate = true;
        }
        if(this->rattachment4 != this->w2a4) {
            this->rattachment4 = this->w2a4;
            this->needupdate = true;
        }
    }
}

void Recognizer::saveLua()
{
    QFile f(QDir::temp().absolutePath() + "/" + GlobalStatic::rec_name);
    if(f.open(QIODevice::WriteOnly)) {
        f.write((this->luaCode.arg(this->rweapon)
                 .arg(this->rpose)
                 .arg(this->rscope)
                 .arg(this->rattachment1)
                 .arg(this->rattachment2)
                 .arg(this->rattachment3)
                 .arg(this->rattachment4))
                 .arg(this->rx)
                 .arg(this->ry).toLocal8Bit());
        f.close();
    }
}

void Recognizer::printLua()
{
    qDebug() << (this->luaCode.arg(this->rweapon)
                .arg(this->rpose)
                .arg(this->rx)
                .arg(this->ry)
                .arg(this->rscope)
                .arg(this->rattachment1)
                .arg(this->rattachment2)
                .arg(this->rattachment3)
                .arg(this->rattachment4));
}

void Recognizer::doRecognize(cv::Mat &screen)
{
    if(this->isBagOpen(screen)) {
        scopeRecognize(screen);
        attachRecognize(screen);

        if (this->bag1) {
            int w1 = weaponNameRecognize(screen, this->w1_x, this->w1_y, this->w1_width, this->w1_height);
            if (w1 != -1) {
                this->w1_img = this->weapons.at(w1).gray_img;
                this->weapon1 = this->weapons.at(w1).name;
            }
        }

        if (this->bag2) {
            int w2 = weaponNameRecognize(screen, this->w2_x, this->w2_y, this->w2_width, this->w2_height);
            if (w2 != -1) {
                this->w2_img = this->weapons.at(w2).gray_img;
                this->weapon2 = this->weapons.at(w2).name;
            }
        }

        if (this->currentweapon == 1 && this->rweapon != this->weapon1) {
            this->rweapon = this->weapon1;
            this->needupdate = true;
        } else if (this->currentweapon == 2 && this->rweapon != this->weapon2) {
            this->rweapon = this->weapon2;
            this->needupdate = true;
        }

        if(this->scope_mode && this->rpose != 0) { // 打开背包关闭姿态
            this->rpose = 0;
            this->needupdate = true;
        }
    } else {
        poseRecognize(screen);
        // weaponRecognize(screen);
        int choose = weaponChoose(screen);
        if (choose != 0 && choose != this->currentweapon) {
            this->currentweapon = choose;
            this->needupdate = true;
            if (currentweapon == 1) {
                if(this->rweapon != this->weapon1) {
                    this->rweapon = this->weapon1;
                    this->needupdate = true;
                }
                if(this->rscope != this->w1scope) {
                    this->rscope = this->w1scope;
                    this->needupdate = true;
                }
                if(this->rattachment1 != this->w1a1) {
                    this->rattachment1 = this->w1a1;
                    this->needupdate = true;
                }
                if(this->rattachment2 != this->w1a2) {
                    this->rattachment2 = this->w1a2;
                    this->needupdate = true;
                }
                if(this->rattachment4 != this->w1a4) {
                    this->rattachment4 = this->w1a4;
                    this->needupdate = true;
                }
            } else {
                if(this->rweapon != this->weapon2) {
                    this->rweapon = this->weapon2;
                    this->needupdate = true;
                }
                if(this->rscope != this->w2scope) {
                    this->rscope = this->w2scope;
                    this->needupdate = true;
                }
                if(this->rattachment1 != this->w2a1) {
                    this->rattachment1 = this->w2a1;
                    this->needupdate = true;
                }
                if(this->rattachment2 != this->w2a2) {
                    this->rattachment2 = this->w2a2;
                    this->needupdate = true;
                }
                if(this->rattachment4 != this->w2a4) {
                    this->rattachment4 = this->w2a4;
                    this->needupdate = true;
                }
            }
        }

        if(this->blood_enable) {
            if(this->bloodrecog->doBloodRecognize(screen, this->rx, this->ry))
                this->needupdate = true;
        }
    }
}

void Recognizer::setScopeMode(int mode)
{
    this->scope_mode = mode;
}

void Recognizer::recognizeLoop()
{
    while(this->isrunning) {
        qint64 start = QDateTime::currentDateTime().toMSecsSinceEpoch();

        if (CVUtils::screenshot_mode == 2) { // DMA Mode, DMA software will change the data in this program.
            struct RecognizerData tempData = RecogData;
            this->currentweapon = tempData.Index;

            this->weapon1 = QString::fromLocal8Bit(tempData.Data[0].Name);
            for (auto &w : this->weapons) {
                if (w.name == this->weapon1) {
                    this->w1_img =w.gray_img;
                    break;
                }
            }
            this->w1scope = tempData.Data[0].Scope;
            this->w1a1 = tempData.Data[0].A1;
            this->w1a2 = tempData.Data[0].A2;
            this->w1a4 = tempData.Data[0].A4;

            this->weapon2 = QString::fromLocal8Bit(tempData.Data[1].Name);
            for (auto &w : this->weapons) {
                if (w.name == this->weapon2) {
                    this->w2_img =w.gray_img;
                    break;
                }
            }
            this->w2scope = tempData.Data[1].Scope;
            this->w2a1 = tempData.Data[1].A1;
            this->w2a2 = tempData.Data[1].A2;
            this->w2a4 = tempData.Data[1].A4;

            if (currentweapon == 0) {
                this->rweapon = "none";
            } else if (currentweapon == 1){
                this->rweapon = this->weapon1;
                this->rscope = this->w1scope;
                this->rattachment1 = this->w1a1;
                this->rattachment2 = this->w1a2;
                this->rattachment4 = this->w1a4;
            } else {
                this->rweapon = this->weapon2;
                this->rscope = this->w2scope;
                this->rattachment1 = this->w2a1;
                this->rattachment2 = this->w2a2;
                this->rattachment4 = this->w2a4;
            }
            this->rpose = tempData.Pose;
            this->rx = 0;
            this->ry = 0;

            this->needupdate = true;
        } else {
            cv::Mat screen = CVUtils::screenShot();
            if(screen.empty() || this->screen_width != screen.cols || this->screen_height != screen.rows) {
                QThread::msleep(10);
                continue;
            }

            this->needupdate = false;
            this->doRecognize(screen);
        }

        if(this->needupdate) {
            this->saveLua();
        }

        emit updateView(this->w1_img, this->w1scope, this->w1a1, this->w1a2, this->w1a4,
                        this->w2_img, this->w2scope, this->w2a1, this->w2a2, this->w2a4,
                        this->rpose, this->currentweapon);

        qint64 end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qint64 sleeptime = 1000 / (this->fps > 0 ? this->fps : 1);

        sleeptime -= (end - start);
        if(sleeptime > 0)
            QThread::msleep(sleeptime);
    }

}
