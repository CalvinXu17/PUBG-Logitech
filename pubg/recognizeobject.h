#ifndef RECOGNIZEOBJECT_H
#define RECOGNIZEOBJECT_H

#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>

class RecognizeObject
{
public:
    QString name;
    QString phash;
    QImage gray_img;
    int id;
    RecognizeObject(QString name, cv::Mat &gray, int id);
    RecognizeObject(QString name, cv::Mat &gray, int id, int n);
};

#endif // RECOGNIZEOBJECT_H
