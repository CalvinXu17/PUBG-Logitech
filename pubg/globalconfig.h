#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QString>

#define GREEN_BLOOD     0
#define RED_BLOOD       1

#define DXGI_CAPTURE    0
#define GDI_CAPTURE     1

class GlobalConfig
{
public:
    float xrate = 0;
    float yrate = 0;
    int bloodcolor = GREEN_BLOOD;
    int fps = 8;
    int capturemode = DXGI_CAPTURE;

    int isdebug = 0;
    int xmove = 10;
    int autorecognize = 1;
    int autoshift = 0;
    int loopdelay = 8;


    void loadJson();
    void saveJson();

    static GlobalConfig* getInstance();
    ~GlobalConfig();
private:
    explicit GlobalConfig();
};

#endif // GLOBALCONFIG_H
