#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QThread>
#include <QImage>
#include "recognizeobject.h"
#include "bloodrecognize.h"

struct WeaponData
{
    char Name[32];
    int Scope; // 倍镜 0 无 1 红点 2 全息 3 2倍 4 3倍 5 四倍 6 6倍
    int A1; // 枪口 0 无 1 补偿 2 消焰 3 消音 4 制退
    int A2; // 握把 0 无 1 垂直 2 三角 3 轻型 4 激光 5 拇指 6 半截
    int A3; // 弹夹 0 无 1 快速 2 扩容 3 快扩
    int A4; // 枪托 0 无 1 枪托 2 重型
};

struct RecognizerData
{
    int Index; // 当前武器位 0 未持武器 或 未在游戏 1 1号武器 2 2号武器
    int Pose; // 姿态 0 未知 1 蹲 2  站 3 趴
    struct WeaponData Data[2];
};

class Recognizer : public QObject
{
    Q_OBJECT
public:

    int screen_width;
    int screen_height;
    int weapon1_x;
    int weapon1_y;
    int weapon1_width;
    int weapon1_height;

    int weapon2_x;
    int weapon2_y;
    int weapon2_width;
    int weapon2_height;

    int pose_x;
    int pose_y;
    int pose_width;
    int pose_height;

    int bp1_x;
    int bp1_y;
    int bp2_x;
    int bp2_y;
    int bp_width;
    int bp_height;

    int w1_x;
    int w1_y;
    int w2_x;
    int w2_y;
    int w1_width;
    int w1_height;
    int w2_width;
    int w2_height;

    int w1scope_x;
    int w1scope_y;
    int w1scope_width;
    int w1scope_height;

    int w1a1_x;
    int w1a1_y;
    int w1a1_width;
    int w1a1_height;

    int w1a2_x;
    int w1a2_y;
    int w1a2_width;
    int w1a2_height;

    int w1a3_x;
    int w1a3_y;
    int w1a3_width;
    int w1a3_height;

    int w1a4_x;
    int w1a4_y;
    int w1a4_width;
    int w1a4_height;


    int w2scope_x;
    int w2scope_y;
    int w2scope_width;
    int w2scope_height;

    int w2a1_x;
    int w2a1_y;
    int w2a1_width;
    int w2a1_height;

    int w2a2_x;
    int w2a2_y;
    int w2a2_width;
    int w2a2_height;

    int w2a3_x;
    int w2a3_y;
    int w2a3_width;
    int w2a3_height;

    int w2a4_x;
    int w2a4_y;
    int w2a4_width;
    int w2a4_height;

    bool bag1 = false;
    bool bag2 = false;
    int currentweapon;
    QString weapon1;
    int w1scope;
    int w1a1;
    int w1a2;
    int w1a4;

    QString weapon2;
    int w2scope;
    int w2a1;
    int w2a2;
    int w2a4;


    QString rweapon;
    int rpose;
    int rx;
    int ry;
    int rscope;
    int rattachment1;
    int rattachment2;
    int rattachment3;
    int rattachment4;


    QImage w1_img;
    QImage w2_img;

    int fps;

    QList<RecognizeObject> weapons;
    QList<RecognizeObject> scopes;
    QList<RecognizeObject> attachment1s;
    QList<RecognizeObject> attachment2s;
    QList<RecognizeObject> attachment4s;
    QList<RecognizeObject> poses;
    QList<RecognizeObject> bps;

    bool isrunning;
    QThread *pthread = NULL;

    bool needupdate;
    bool init_success;

    int scope_mode = 0;

    QString luaCode;

    BloodRecognize *bloodrecog = NULL;
    bool blood_enable = false;
    static Recognizer* getInstance();
    ~Recognizer();

    void start();
    void stop();
    void setFPS(int fps);

    bool isBagOpen(cv::Mat &screen);

    void poseRecognize(cv::Mat &screen);

    bool isWeaponRed(cv::Mat &img);
    bool isWeaponWhite(float r, float g, float b);
    bool isWeaponActivated(float r, float g, float b);
    void weaponRecognize(cv::Mat &screen);
    int weaponChoose(cv::Mat &screen);
    int weaponNameRecognize(cv::Mat &screen, int x, int y, int width, int height);
    void scopeRecognize(cv::Mat &screen);
    void attachRecognize(cv::Mat &screen);

    void saveLua();
    void printLua();

    void doRecognize(cv::Mat &screen);

    void setScopeMode(int mode);

signals:
    void updateView(const QImage &w1, int w1scope, int w1a1, int w1a2, int w1a4,
                    const QImage &w2, int w2scope, int w2a1, int w2a2, int w2a4,
                    int pos, int currentw);

public slots:
    void recognizeLoop();

private:
    explicit Recognizer(QObject *parent = nullptr);

};

#endif // RECOGNIZER_H
