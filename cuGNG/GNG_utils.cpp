
#include "GNG_utils.h"
#include <iostream>

using namespace std;

hsv_color rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b)
{
	hsv_color hsv;
	unsigned char rgb_min, rgb_max;
	rgb_min = MIN3(r,g,b);
	rgb_max = MAX3(r,g,b);

	hsv.val = rgb_max;
	if (hsv.val == 0) {
		hsv.hue = hsv.sat = 0;
		return hsv;
	}

	hsv.sat = 255*long(rgb_max - rgb_min)/hsv.val;
	if (hsv.sat == 0) {
		hsv.hue = 0;
		return hsv;
	}

	/* Compute hue */
	if (rgb_max == r) {
		hsv.hue = 0 + 43*(g-b)/(rgb_max - rgb_min);
	} else if (rgb_max == g) {
		hsv.hue = 85 + 43*(b - r)/(rgb_max - rgb_min);
	} else /* rgb_max == rgb.b */ {
		hsv.hue = 171 + 43*(r - g)/(rgb_max - rgb_min);
	}
	return hsv;
}

bool isSkinColorRGB(unsigned char r,unsigned char g,unsigned char b)
{
	// reference h s values for skin color regions
	unsigned char h = 25;
	unsigned char s = 75;

	int threshold_h = 25;
	int threshold_s = 20;

	hsv_color hsv = rgb_to_hsv(r,g,b);
	//cout << (int)hsv.hue << ";" << (int)hsv.sat << endl;

	/*if( (r>(b_r-threshold) && r<(b_r+threshold)) && (g>(b_g-threshold) && g<(b_g+threshold)) 
		&& (b>(b_b-threshold) && b<(b_b+threshold)) )*/
	if( (hsv.hue>=0 && hsv.hue<=60) && (hsv.sat>=60 && hsv.sat<=110) )
		return true;
	else
		return false;
}

void segmentarPatronesEntradaSCR(std::vector<cv::Point3f> &pcl, std::vector<cv::Vec3b> &color)
{
	vector<cv::Point3f>::iterator it = pcl.begin();
	vector<cv::Vec3b>::iterator it_color = color.begin();

    for (unsigned int i=0;i<pcl.size();i++) {
        if( !isSkinColorRGB((unsigned char)(*it_color)[0],(unsigned char)(*it_color)[1],(unsigned char)(*it_color)[2]) )
		{
			it=pcl.erase(it);
			it_color=color.erase(it_color);
		}else
		{
			it++;
			it_color++;
		}
	}

	float avg=0.0;
	for (unsigned int i=0;i<pcl.size();i++) {
        avg+=pcl[i].z;
	}
	avg=avg/pcl.size();

	for (unsigned int i=0;i<pcl.size();i++) {
        if( avg-pcl[i].z > 0.3  )
			cout << "point.x = " << pcl[i].x << " point.y = " << pcl[i].y << " point.z = " << pcl[i].z << endl;
	}
}
