#ifndef LOGINET_H
#define LOGINET_H

#include <QObject>
#include <QThread>

struct MouseData {
    uint32_t magic; // 0x84398439
    uint32_t time; // timestamp in ms
    uint32_t cmd; // 0x1 mouse
    uint32_t button; // byte 0: 0x1 left 0x2 middle 0x4 right byte1: left state
    int32_t x; // relative move x
    int32_t y; // relative move y
    int32_t wheel; // scroll
    uint32_t randxor;
};

class LogiNet: public QObject
{
    Q_OBJECT
public:
    bool isRunning = false;
    QThread *pThread = NULL;
    bool isMouseOpen = false;
    int port = 8787;
    explicit LogiNet(QObject *parent = nullptr);
    ~LogiNet();

    void start(int port);
    void stop();

public slots:
    void mouseLoop();
};

#endif // LOGINET_H
