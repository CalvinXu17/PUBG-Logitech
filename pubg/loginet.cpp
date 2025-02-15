#include "loginet.h"
#include "logitech_driver.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <QMessageBox>

LogiNet::LogiNet(QObject *parent) : QObject(parent)
{
    this->isMouseOpen = mouse_open();
    qDebug() << "mouseOpen " << this->isMouseOpen;
}

LogiNet::~LogiNet()
{
    this->stop();
    if (this->pThread) {
        if(this->pThread->isRunning()) {
            this->pThread->quit();
            this->pThread->wait();
        }
        disconnect(this->pThread, SIGNAL(started()), this, SLOT(mouseLoop()));
        delete this->pThread;
        this->pThread = NULL;
    }
    mouse_close();
}

void LogiNet::start(int port)
{
    if (!this->isMouseOpen) {
        QMessageBox msg;
        msg.setText(QString("罗技驱动打开失败!"));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    if(this->pThread && this->isRunning)
        return;

    if (port > 1000)
        this->port = port;

    if(this->pThread == NULL) {
        this->pThread = new QThread();
        this->moveToThread(this->pThread);
        connect(this->pThread, SIGNAL(started()), this, SLOT(mouseLoop()));
    }
    this->isRunning = true;
    this->pThread->start();
}

void LogiNet::stop()
{
    if(this->pThread && this->isRunning) {
        this->isRunning = false;
        this->pThread->wait(10);
        this->pThread->quit();
    }
}

void LogiNet::mouseLoop()
{
    auto socket = new QUdpSocket();
    if (!socket->bind(QHostAddress::Any, this->port)) {
        return;
    }

    while(this->isRunning) {
        // 阻塞直到有数据可读
        if (socket->waitForReadyRead()) {
            // 每次只读取一个报文
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            // 读取一个完整的UDP报文
            socket->readDatagram(datagram.data(), datagram.size());
            if (datagram.size() == sizeof(struct MouseData)) {
                struct MouseData *data = (struct MouseData *)datagram.data();
                uint32_t deadbeaf = 0xdeadbeaf;
                data->randxor ^= deadbeaf;
                uint32_t *p = (uint32_t *)data;
                for (int i=0; p + i < &data->randxor; i++) {
                    p[i] = p[i] ^ data->randxor;
                }
                if (data->magic == 0x6C6F6769) {
                    if (data->cmd == 0x1 && this->isMouseOpen) {
                        moveR(data->x, data->y);
                    }
                }
            }
        }
    }

    socket->close();
    delete socket;
}
