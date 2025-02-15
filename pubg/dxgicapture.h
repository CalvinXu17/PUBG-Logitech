#ifndef DXGICAPTURE_H
#define DXGICAPTURE_H

#include <windows.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <opencv2/opencv.hpp>
#include <QString>

#define DXGI_LOG 0

#if DXGI_LOG == 1
#include <QFile>
#include <QDateTime>
#endif

#define SAFE_RELEASE(p) \
    if(p != nullptr) { p->Release(); p = nullptr; }

class DXGICapture
{
public:
    ID3D11Device *pDevice = nullptr;;
    ID3D11DeviceContext *pDeviceContext = nullptr;;
    IDXGIOutputDuplication *pDeskDupl = nullptr;;

    DXGI_OUTPUT_DESC lOutputDesc;
    DXGI_OUTDUPL_DESC lOutputDuplDesc;

    int width = 0;
    int height = 0;

    QString result = "";
    bool success = false;

    cv::Mat screen;

#if DXGI_LOG == 1
    QFile *f = nullptr;
#endif


    DXGICapture();
    ~DXGICapture();

    bool init();
    void deinit();
    bool CaptureFrame(cv::Mat &out);
    cv::Mat screenShot();
    static DXGICapture* getInstance();
};

#endif // DXGICAPTURE_H
