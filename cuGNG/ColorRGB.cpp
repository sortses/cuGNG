#include "ColorRGB.h"

//ColorRGB Class
//
ColorRGB::ColorRGB(){}
ColorRGB::ColorRGB(float r,float g,float b){
	this->r=r;
	this->b=b;
	this->g=g;
}

float ColorRGB::getR(){ return r; }
float ColorRGB::getG(){ return g; }
float ColorRGB::getB(){ return b; }

void ColorRGB::setR(float R){ this->r=R;}
void ColorRGB::setG(float G){ this->g=G;}
void ColorRGB::setB(float B){ this->b=B;}