#include "bloodrecognize.h"
#include "cvutils.h"
#include "math.h"
#include <QDebug>
#include <vector>

BloodRecognize::BloodRecognize(int color)
{
    if(color == BLOOD_GREEN) {
        this->hmin = hgmin;
        this->hmax = hgmax;
    } else {
        this->hmin = hrmin;
        this->hmax = hrmax;
    }
}

BloodRecognize::~BloodRecognize()
{

}

void BloodRecognize::setBloodColor(int color)
{
    if(color == BLOOD_GREEN) {
        this->hmin = hgmin;
        this->hmax = hgmax;
    } else {
        this->hmin = hrmin;
        this->hmax = hrmax;
    }
}

void BloodRecognize::setRate(float x, float y)
{
    this->xrate = x;
    this->yrate = y;
}

bool BloodRecognize::doBloodRecognize(cv::Mat &screen, int &x, int &y)
{
    bool needupdate = false;
    int screen_width = screen.cols;
    int screen_height = screen.rows;

    int bx = (int)round((double)screen_width / 2 - round(0.0208333 * (double)screen_width));
    int by = (int)round((double)screen_height / 2 - round(0.0740741 * (double)screen_height));
    int bwidth = (int)round(0.0208333 * (double)screen_width) * 2;
    int bheight = (int)round(0.0740741 * (double)screen_height) * 2;

    int xcenter = bwidth / 2;
    int ycenter = bheight / 2;

    cv::Mat hsv = CVUtils::cropMat(screen, bx, by, bwidth, bheight);
    cv::cvtColor(hsv, hsv, cv::COLOR_BGR2HSV);

    cv::medianBlur(hsv, hsv, 9);

    std::vector<int> lower = {hmin, smin, vmin};
    std::vector<int> upper = {hmax, smax, vmax};
    cv::Mat mask;

    cv::inRange(hsv, lower, upper, mask);
    auto s = cv::sum(mask)[0];
    if (s > 30000) {
        int dot1x = bwidth + 1;
        int dot1y = bheight + 1;
        int dot2x = 0;
        int dot2y = 0;
        for(int i=0;i<mask.rows;i++)
            for(int j=0;j<mask.cols;j++) {
                if(mask.at<uchar>(i,j) == 255) {
                    if(j < dot1x)
                        dot1x = j;
                    if(i < dot1y)
                        dot1y = i;
                    if(j > dot2x)
                        dot2x = j;
                    if(i > dot2y)
                        dot2y = i;
                }
            }

        int blood_area = (dot2y-dot1y) * (dot2x-dot1x);
        int screen_area = bwidth * bheight;
        float blood_center_x = round((float)(dot2x - dot1x) / 2) + dot1x;
        float blood_center_y = round((float)(dot2y - dot1y) / 2) + dot1y;

        int movex = int((float)(blood_center_x - xcenter) * xrate * ((float)blood_area / (float)screen_area));
        int movey = -int((blood_center_y - ycenter) * yrate);

        if(x != movex) {
            x = movex;
            needupdate = true;
        }
        if(y != movey) {
            y = movey;
            needupdate = true;
        }
    } else {
        if(x != 0) {
            x = 0;
            needupdate = true;
        }
        if(y != 0) {
            y = 0;
            needupdate = true;
        }
    }
    return needupdate;
}
