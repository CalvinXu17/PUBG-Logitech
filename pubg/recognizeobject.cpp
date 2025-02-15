#include "recognizeobject.h"
#include "cvutils.h"

RecognizeObject::RecognizeObject(QString name, cv::Mat &gray, int id)
{
    this->name = name;
    this->phash = CVUtils::pHash(gray);
    this->gray_img = CVUtils::cvMatToQImage(gray);
    this->id = id;
}

RecognizeObject::RecognizeObject(QString name, cv::Mat &gray, int id, int n)
{
    this->name = name;
    this->phash = CVUtils::pHashN(gray, n);
    this->gray_img = CVUtils::cvMatToQImage(gray);
    this->id = id;
}
