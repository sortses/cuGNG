#ifndef _COLOR_RGB_
#define _COLOR_RGB_

class ColorRGB
{
	private:
		float r,g,b;

	public:
		ColorRGB();
		ColorRGB(float r,float g,float b);

		float getR();
		float getG();
		float getB();

		void setR(float R);
		void setG(float G);
		void setB(float B);
};

#endif