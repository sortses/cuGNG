#ifndef __utils__h__
#define __utils__h__

#include <opencv/cv.h>
#include <iostream>

struct hsv_color {
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
};

#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))

hsv_color rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b);

bool isSkinColorRGB(unsigned char r,unsigned char g,unsigned char b);

void segmentarPatronesEntradaSCR(std::vector<cv::Point3f> &pcl, std::vector<cv::Vec3b> &color);

#endif