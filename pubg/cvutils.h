#ifndef CVUTILS_H
#define CVUTILS_H

#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>

#define BPIX 4
class CVUtils
{
public:

    static int screenshot_mode;
    CVUtils();
    static cv::Mat screenShotGDI(HWND hwnd);
    static cv::Mat screenShot();
    static cv::Mat to_black_white(cv::Mat &img, bool clone);
    static cv::Mat to_black_white_above200(cv::Mat &img, bool clone);
    static cv::Mat to_black_white_avg(cv::Mat &img, bool clone);
    static cv::Mat to_black_white_pose(cv::Mat &img, bool clone);
    static QString pHash(cv::Mat &img);
    static QString pHashN(cv::Mat &img, int n);
    static int HammingDistance(QString hash1, QString hash2);

    static cv::Mat cropMat(cv::Mat &inmat, int x, int y, int width, int height);

    static void getRGBMean(cv::Mat &img, float &r, float &g, float &b);

    static QImage cvMatToQImage(cv::Mat &mat);
    static cv::Mat QImageToCvMat(const QImage &img, bool inCloneImageData);
    static cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData);

};

#endif // CVUTILS_H
