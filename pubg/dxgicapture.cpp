#include "dxgicapture.h"
#include <QDebug>
#include <QThread>
#include "cvutils.h"

#define SAFE_RELEASE(p) \
    if(p != nullptr) { p->Release(); p = nullptr; }

QString HresultToQString(HRESULT hr) {
    LPSTR messageBuffer = nullptr;
    int size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr,
                                 hr,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR)&messageBuffer,
                                 0,
                                 nullptr);
    if (size == 0) {
        return "Failed to convert HRESULT to string.";
    }

    QString message = QString::fromLocal8Bit(messageBuffer, size);
    message = "0x" + QString::number(hr, 16) + ": " + message;
    LocalFree(messageBuffer);
    return message;
}

DXGICapture::DXGICapture()
{
#if DXGI_LOG == 1
    f = new QFile("./dxgilog.txt");
    f->open(QIODevice::WriteOnly|QIODevice::Text);
#endif
    this->success = this->init();
}

DXGICapture::~DXGICapture()
{
    this->deinit();
#if DXGI_LOG == 1
    if(f) {
        f->close();
        delete f;
    }
#endif
}

bool DXGICapture::init()
{
    // Driver types supported
    D3D_DRIVER_TYPE gDriverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE
    };
    UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

    // Feature levels supported
    D3D_FEATURE_LEVEL gFeatureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };

    UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);

    HRESULT hr = E_FAIL;
    D3D_FEATURE_LEVEL lFeatureLevel;
    // Create device
    for (UINT DriverTypeIndex = 0; DriverTypeIndex < gNumDriverTypes; ++DriverTypeIndex) {
        pDevice = nullptr;
        pDeviceContext = nullptr;
        hr = D3D11CreateDevice(
            nullptr,
            gDriverTypes[DriverTypeIndex],
            nullptr,
            0,
            gFeatureLevels,
            gNumFeatureLevels,
            D3D11_SDK_VERSION,
            &this->pDevice,
            &lFeatureLevel,
            &this->pDeviceContext);

        if (SUCCEEDED(hr)) {
            // Device creation success, no need to loop anymore
            break;
        }

        SAFE_RELEASE(pDevice);
        SAFE_RELEASE(pDeviceContext);
    }

    if (FAILED(hr)) {
        result = HresultToQString(hr);
        return false;
    }
    if (pDevice == nullptr) {
        result = "pDevice == nullptr";
        return false;
    }


    // 创建DXGI设备
    IDXGIDevice* lDxgiDevice = nullptr;
    hr = pDevice->QueryInterface(__uuidof(IDXGIDevice),
                                 reinterpret_cast<void**>(&lDxgiDevice));
    if (FAILED(hr)) {
        result = HresultToQString(hr);
        return false;
    }

    IDXGIAdapter* lDxgiAdapter = nullptr;
    hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter),
                                reinterpret_cast<void**>(&lDxgiAdapter));
    if (FAILED(hr)) {
        result = HresultToQString(hr);
        return false;
    }

    SAFE_RELEASE(lDxgiDevice);


    UINT Output = 0;

    // 枚举适配器(视频卡)输出
    IDXGIOutput* lDxgiOutput;
    hr = lDxgiAdapter->EnumOutputs(Output,
                                   &lDxgiOutput);
    if (FAILED(hr)) {
        result = HresultToQString(hr);
        return false;
    }

    hr = lDxgiOutput->GetDesc(&this->lOutputDesc);

    if (FAILED(hr)) {
        result = HresultToQString(hr);
        return false;
    }

    // Get size
    this->width = this->lOutputDesc.DesktopCoordinates.right;
    this->height = this->lOutputDesc.DesktopCoordinates.bottom;

    // 创建拷贝缓冲区
    this->screen = cv::Mat(height, width, CV_8UC4);

    qDebug() << "DXGI:" << this->width << this->height;

    // QI for Output 1
    IDXGIOutput1* lDxgiOutput1 = 0;
    hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1),
                                     reinterpret_cast<void**>(&lDxgiOutput1));
    if (FAILED(hr)) {
        result = HresultToQString(hr);
        qDebug() << "QueryInterface:" << result;
        return false;
    }

    SAFE_RELEASE(lDxgiOutput);

    // Create desktop duplication
    hr = lDxgiOutput1->DuplicateOutput(this->pDevice,
                                       &this->pDeskDupl);

    if (FAILED(hr)) {
        if(hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) { // 达到使用上限
            qDebug() << "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
        }
        result = HresultToQString(hr);
        qDebug() << "DuplicateOutput:" << result;
        return false;
    }

    SAFE_RELEASE(lDxgiOutput1);
#if DXGI_LOG == 1
    f->write("DXGI Init Success!\n");
#endif
    return true;
}

void DXGICapture::deinit()
{
    SAFE_RELEASE(this->pDevice);
    SAFE_RELEASE(this->pDeviceContext);
    SAFE_RELEASE(this->pDeskDupl);
}

bool DXGICapture::CaptureFrame(cv::Mat &out)
{
    HRESULT hr = E_FAIL;

    IDXGIResource* pDesktopResource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

    hr = pDeskDupl->AcquireNextFrame(1, &lFrameInfo, &pDesktopResource);

    if (FAILED(hr)) { // 桌面没有发生变化的情况
        if(hr == DXGI_ERROR_WAIT_TIMEOUT)
            return false;
        else if (hr == DXGI_ERROR_ACCESS_LOST || hr == DXGI_ERROR_DEVICE_REMOVED) {
#if DXGI_LOG == 1
            f->write("AcquireNextFrame Failed DXGI_ERROR_ACCESS_LOST!\n");
#endif
            this->deinit();
            this->init();
        }
        return false;
    }

    if (lFrameInfo.LastPresentTime.HighPart == 0) { // 忽略鼠标的更新 which can happen much faster than 60fps if you really shake the mouse
        SAFE_RELEASE(pDesktopResource);
        this->pDeskDupl->ReleaseFrame();
#if DXGI_LOG == 1
        f->write("Ignore Mouse Update!\n");
#endif
        return true;
    }

    ID3D11Texture2D *pAcquiredDesktopImage=nullptr;
    // Query next frame staging buffer
    hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D),
                                          reinterpret_cast<void**>(&pAcquiredDesktopImage));

    SAFE_RELEASE(pDesktopResource);

    if (FAILED(hr)) {
#if DXGI_LOG == 1
        f->write("QueryInterface pAcquiredDesktopImage Failed!\n");
#endif
        this->pDeskDupl->ReleaseFrame();
        return false;
    }

    // Create GUI drawing texture
    pDeskDupl->GetDesc(&this->lOutputDuplDesc);

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = lOutputDuplDesc.ModeDesc.Width;
    desc.Height = lOutputDuplDesc.ModeDesc.Height;
    desc.Format = lOutputDuplDesc.ModeDesc.Format;
    desc.ArraySize = 1;
    desc.BindFlags = 0; // D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.MipLevels = 1;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;

    ID3D11Texture2D* currTexture=nullptr;
    hr = pDevice->CreateTexture2D(&desc, NULL, &currTexture);

    if (FAILED(hr)) {
#if DXGI_LOG == 1
        f->write("CreateTexture2D currTexture Failed!\n");
#endif
        SAFE_RELEASE(pAcquiredDesktopImage);
        this->pDeskDupl->ReleaseFrame();
        return false;
    }

    // Copy image into CPU access texture
    pDeviceContext->CopyResource(currTexture, pAcquiredDesktopImage);
    // 拷贝完即可释放
    SAFE_RELEASE(pAcquiredDesktopImage);
    pDeskDupl->ReleaseFrame();

    // create staging buffer for map bits
    IDXGISurface* pStagingSurf = nullptr;
    hr = currTexture->QueryInterface(__uuidof(IDXGISurface), (void**)(&pStagingSurf));
    SAFE_RELEASE(currTexture);

    if(FAILED(hr)) {
#if DXGI_LOG == 1
        f->write("QueryInterface pStagingSurf Failed!\n");
#endif
        return false;
    }

    // copy bits to user space
    DXGI_MAPPED_RECT mappedRect;
    hr = pStagingSurf->Map(&mappedRect, DXGI_MAP_READ);

    if(FAILED(hr)) {
#if DXGI_LOG == 1
        f->write("Map Failed!\n");
#endif
        pStagingSurf->Unmap();
        SAFE_RELEASE(pStagingSurf);
        return false;
    }

    UINT nImageSize = this->width * this->height * 4;
    memcpy(out.data, mappedRect.pBits, nImageSize);
    pStagingSurf->Unmap();
    SAFE_RELEASE(pStagingSurf);

    return true;
}

cv::Mat DXGICapture::screenShot()
{

#if DXGI_LOG == 1
    qint64 s = QDateTime::currentMSecsSinceEpoch();
#endif
    bool b = false;
    do {
        b = this->CaptureFrame(screen);
    } while(!b);

#if DXGI_LOG == 1
    qint64 e = QDateTime::currentMSecsSinceEpoch();
    this->f->write(("ScreenShot Time: " + QString::number(e-s) + "ms\n").toUtf8());
#endif

#if BPIX == 4
    return screen;
#else
    cv::Mat ret(this->height, this->width, CV_8UC3);
    cv::cvtColor(screen, ret, cv::COLOR_BGRA2BGR);
    return ret;
#endif
}

DXGICapture *DXGICapture::getInstance()
{
    static DXGICapture f;
    return &f;
}
