#ifndef BLOODRECOGNIZE_H
#define BLOODRECOGNIZE_H

#include <opencv2/opencv.hpp>

#define BLOOD_GREEN 0
#define BLOOD_RED   1

class BloodRecognize
{
public:
    int hmin, hmax;
    int smin = 43;
    int smax = 255;
    int vmin = 46;
    int vmax = 255;

    // green blood
    int hgmin = 35;
    int hgmax = 77;
    // red blood
    int hrmin = 156;
    int hrmax = 180;

    float xrate = 1.0;
    float yrate = 1.0;

    explicit BloodRecognize(int color=0);
    ~BloodRecognize();

    void setBloodColor(int color);
    void setRate(float x, float y);
    bool doBloodRecognize(cv::Mat &screen, int &x, int &y);

};

#endif // BLOODRECOGNIZE_H
