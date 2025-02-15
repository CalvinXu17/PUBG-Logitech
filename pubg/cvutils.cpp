#include "cvutils.h"
#include <windows.h>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QWindow>
#include <QDebug>
#include "dxgicapture.h"

BITMAPINFOHEADER createBitmapHeader(int width, int height)
{
     BITMAPINFOHEADER  bi;

     // create a bitmap
     bi.biSize = sizeof(BITMAPINFOHEADER);
     bi.biWidth = width;
     bi.biHeight = -height;  //this is the line that makes it draw upside down or not
     bi.biPlanes = 1;
     bi.biBitCount = 8*BPIX;
     bi.biCompression = BI_RGB;
     bi.biSizeImage = width*height*3;
     bi.biXPelsPerMeter = 0;
     bi.biYPelsPerMeter = 0;
     bi.biClrUsed = 0;
     bi.biClrImportant = 0;

     return bi;
}

int CVUtils::screenshot_mode = 0;

CVUtils::CVUtils()
{

}

cv::Mat CVUtils::screenShotGDI(HWND hwnd)
{
    HDC deviceContext = GetDC(hwnd);
    HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);

    int height = windowRect.bottom;
    int width = windowRect.right;

    HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);

    SelectObject(memoryDeviceContext, bitmap);

    BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);


    BITMAPINFOHEADER bi = createBitmapHeader(width, height);

#if BPIX == 4
     cv::Mat shot(height, width, CV_8UC4);
#else
     cv::Mat shot(height, width, CV_8UC3);
#endif

    GetDIBits(memoryDeviceContext, bitmap, 0, height, shot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

    DeleteObject(bitmap);
    DeleteDC(memoryDeviceContext);
    ReleaseDC(hwnd, deviceContext);
    return shot;
}

cv::Mat CVUtils::screenShot()
{
//    cv::Mat m = cv::imread("F:\\python\\pubg\\2.png");
//    if (m.channels() == 3)
//        cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);
//    return m;
    if(!screenshot_mode)
    {
        DXGICapture *cap = DXGICapture::getInstance();
        if(cap->success)
            return cap->screenShot();

    }
    HWND hwnd = GetDesktopWindow();
    return screenShotGDI(hwnd);
//    QScreen *screen = QGuiApplication::primaryScreen();
//    QPixmap img = screen->grabWindow(QApplication::desktop()->winId());
//    return QPixmapToCvMat(img, true);
}

cv::Mat CVUtils::to_black_white(cv::Mat &img, bool clone)
{
    cv::Mat gray = clone ? img.clone() : img;

    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        //每一次跳跃三列，也就是跳跃一个像素，三列也就是B,G,R
        for (int col = 0; col < gray.cols*BPIX; col+=BPIX) {
            uchar b = ptr[col+0] < 30 ? 255 : 0;
            uchar g = ptr[col+1] < 30 ? 255 : 0;
            uchar r = ptr[col+2] < 30 ? 255 : 0;
            ptr[col+0] = ptr[col+1] = ptr[col+2] = b & g & r;
        }

    }
    return gray;
}


cv::Mat CVUtils::to_black_white_above200(cv::Mat &img, bool clone)
{
    cv::Mat gray = clone ? img.clone() : img;

    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        //每一次跳跃三列，也就是跳跃一个像素，三列也就是B,G,R
        for (int col = 0; col < gray.cols*BPIX; col+=BPIX) {
            uchar b = ptr[col+0] > 200 ? 255 : 0;
            uchar g = ptr[col+1] > 200 ? 255 : 0;
            uchar r = ptr[col+2] > 200 ? 255 : 0;
            ptr[col+0] = ptr[col+1] = ptr[col+2] = b & g & r;
        }

    }
    return gray;
}

cv::Mat CVUtils::to_black_white_avg(cv::Mat &img, bool clone)
{
    cv::Mat gray = clone ? img.clone() : img;
    float avg = 0.0;
    int n = 0;
    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        for (int col = 0; col < gray.cols*BPIX; col+=BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            avg += ((float)(b + g + r)) / 3;
            n++;
        }
    }
    avg = avg / n;

    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        for (int col = 0; col < gray.cols*BPIX; col+=BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            float a = ((float)(b+g+r)) / 3;
            if(fabs(b - g) <= 20 && fabs(b-r) <= 20 && fabs(g-r) <= 20 && b > 150 && g > 150 && r > 150 && a - avg > 20) {
                if((a - avg < 40 && a > 220) || (a-avg > 80 && a > 200) || (a > 230))
                    ptr[col+0] = ptr[col+1] = ptr[col+2] = 255;
                else
                    ptr[col+0] = ptr[col+1] = ptr[col+2] = 0;
            } else
                ptr[col+0] = ptr[col+1] = ptr[col+2] = 0;
        }
    }
    return gray;
}

cv::Mat CVUtils::to_black_white_pose(cv::Mat &img, bool clone)
{
    cv::Mat gray = clone ? img.clone() : img;
    float avg = 0.0;
    int n = 0;
    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        for (int col = 0; col < gray.cols * BPIX; col += BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            avg += ((float)(b + g + r)) / 3;
            n++;
        }
    }
    avg = avg / n;

    for (int row = 0; row < gray.rows; row++) {
        uchar* ptr = gray.ptr<uchar>(row);
        for (int col = 0; col < gray.cols * BPIX; col += BPIX) {
            int b = ptr[col+0];
            int g = ptr[col+1];
            int r = ptr[col+2];
            float a = ((float)(b+g+r)) / 3;
            if(fabs(b-g) <= 20 && fabs(b-r) <= 20 && fabs(g-r) <= 20 && b > 150 && g > 150 && r > 150 && a - avg > 20) {
                if((a-avg < 40 && a > 220) || (a - avg > 80 && a > 180) || (a > 220))
                    ptr[col+0] = ptr[col+1] = ptr[col+2] = 255;
                else
                    ptr[col+0] = ptr[col+1] = ptr[col+2] = 0;
            } else
                ptr[col+0] = ptr[col+1] = ptr[col+2] = 0;
        }
    }
    return gray;
}

QString CVUtils::pHash(cv::Mat &oimg)
{
    QString hash = "";
    cv::Mat img = oimg.clone();
    cv::resize(img, img, cv::Size(32, 32));

#if BPIX == 4
    cv::cvtColor(img, img, cv::COLOR_BGRA2GRAY);
#else
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
#endif

    img.convertTo(img, CV_32F);
    cv::Mat srcDCT;
    cv::dct(img, srcDCT);
    double sum = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            sum += srcDCT.at<float>(i,j);
    float average = sum / 64;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if(srcDCT.at<float>(i,j) >= average)
                hash.append('1');
            else
                hash.append('0');
    return hash;
}

QString CVUtils::pHashN(cv::Mat &oimg, int n)
{
    if(n > 32)
        n = 32;

    QString hash = "";
    cv::Mat img = oimg.clone();
    cv::resize(img, img, cv::Size(32, 32));
#if BPIX == 4
    cv::cvtColor(img, img, cv::COLOR_BGRA2GRAY);
#else
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
#endif

    img.convertTo(img, CV_32F);
    cv::Mat srcDCT;
    cv::dct(img, srcDCT);
    double sum = 0;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            sum += srcDCT.at<float>(i,j);
    float average = sum / 64;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if(srcDCT.at<float>(i,j) >= average)
                hash.append('1');
            else
                hash.append('0');
    return hash;
}

int CVUtils::HammingDistance(QString hash1, QString hash2)
{
    int len = MIN(hash1.length(), hash2.length());
    int ret = 0;
    for(int i=0; i < len; i++) {
        if(hash1[i] != hash2[i])
            ret++;
    }
    return ret;
}

cv::Mat CVUtils::cropMat(cv::Mat &inmat, int x, int y, int width, int height)
{
    cv::Rect rect(x, y, width, height);
    return inmat(rect).clone();
}

void CVUtils::getRGBMean(cv::Mat &img, float &r, float &g, float &b)
{
    uint rsum = 0;
    uint gsum = 0;
    uint bsum = 0;
    uint rn = 0;
    uint gn = 0;
    uint bn = 0;
    for (int row = 0; row < img.rows; row++) {
        uchar* ptr = img.ptr<uchar>(row);
        for (int col = 0; col < img.cols*BPIX; col+=BPIX) {
            uint r = ptr[col+0];
            uint g = ptr[col+1];
            uint b = ptr[col+2];
            if(r) {
                rsum += r;
                rn += 1;
            }
            if(g) {
                gsum += g;
                gn += 1;
            }
            if(b) {
                bsum += b;
                bn += 1;
            }
        }
    }
    r = (float)rsum / rn;
    g = (float)gsum / gn;
    b = (float)bsum / bn;
}

QImage CVUtils::cvMatToQImage(cv::Mat &inMat)
{
    switch (inMat.type()) {
         // 8-bit, 4 channel
         case CV_8UC4: {
            QImage image(inMat.data,
                         inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_ARGB32);
            return image.rgbSwapped();
         }

         // 8-bit, 3 channel
         case CV_8UC3: {
            QImage image(inMat.data,
                         inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_RGB888);
            return image.rgbSwapped();
         }

         // 8-bit, 1 channel
         case CV_8UC1: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data,
                         inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_Grayscale8);
#else
                static QVector<QRgb>  sColorTable;

                // only create our color table the first time
                if ( sColorTable.isEmpty() )
                {
                   sColorTable.resize( 256 );

                   for ( int i = 0; i < 256; ++i )
                   {
                      sColorTable[i] = qRgb( i, i, i );
                   }
                }

                QImage image( inMat.data,
                              inMat.cols, inMat.rows,
                              static_cast<int>(inMat.step),
                              QImage::Format_Indexed8 );

            image.setColorTable( sColorTable );
    #endif

            return image;
         }

         default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
      }
    return QImage();
}

cv::Mat CVUtils::QImageToCvMat(const QImage &img, bool inCloneImageData)
{
    switch (img.format()) {
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied: {
            cv::Mat mat(img.height(), img.width(),
                        CV_8UC4,
                        const_cast<uchar*>(img.bits()),
                        static_cast<size_t>(img.bytesPerLine()));
            return (inCloneImageData ? mat.clone() : mat);
        }
        case QImage::Format_RGB32:
        case QImage::Format_RGB888: {
            if (!inCloneImageData)
                qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";

            QImage swapped = img;
            if (img.format() == QImage::Format_RGB32)
                swapped = swapped.convertToFormat(QImage::Format_RGB888);

            swapped = swapped.rgbSwapped();

            return cv::Mat(swapped.height(), swapped.width(),
                           CV_8UC3,
                           const_cast<uchar*>(swapped.bits()),
                           static_cast<size_t>(swapped.bytesPerLine())
                           ).clone();
        }
        // 8-bit, 1 channel
        case QImage::Format_Indexed8: {
            cv::Mat  mat(img.height(), img.width(),
                         CV_8UC1,
                         const_cast<uchar*>(img.bits()),
                         static_cast<size_t>(img.bytesPerLine())
                         );
            return (inCloneImageData ? mat.clone() : mat);
        }
        default:
            qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << img.format();
            break;
    }
    return cv::Mat();
}

cv::Mat CVUtils::QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData)
{
    return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

