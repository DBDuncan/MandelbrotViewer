#include "DoLine.h"



DoLine::DoLine(uint32_t(*imageNew)[HEIGHT][WIDTH],
	double left,
	double right,
	double top,
	double bottom,
	int yNew,
	int colourNew,
	double yOffsetNew,
	double xOffsetNew,
	double zoomNew,
	int maxIterationsNew)
{
	image = imageNew;
	boundsData.left = left;
	boundsData.right = right;
	boundsData.top = top;
	boundsData.bottom = bottom;
	y = yNew;
	colour = colourNew;
	yOffset = yOffsetNew;
	xOffset = xOffsetNew;
	zoom = zoomNew;
	maxIterations = maxIterationsNew;
}


DoLine::~DoLine()
{
}

void DoLine::run()
{
	for (int x{ 0 }; x < WIDTH; ++x) {
		std::complex<double> c(((boundsData.left + ((120+x)*(boundsData.right - boundsData.left) / WIDTH)) * zoom + xOffset),
			((boundsData.top + (y*(boundsData.bottom - boundsData.top) / HEIGHT)) * zoom + yOffset));
		std::complex<double> z(0.0, 0.0);
		int iterations{ 0 };

		//c = std::pow(c, 2);
		//doing this means no square rooting!
		double zrsqr = z.real() * z.real();
		double zisqr = z.imag() * z.imag();
		

		while (zrsqr + zisqr < 4.0 && iterations < maxIterations) {
			z = (z*z) + c;
			++iterations;

			zrsqr = z.real() * z.real();
			zisqr = z.imag() * z.imag();
		}

		if (iterations == maxIterations)
		{
			(*image)[y][x] = 0x000000;
		}
		else
		{
			if (colour == 0)
			{
				(*image)[y][x] = 255 << 24 | iterations; // blue
			}
			else if (colour == 1)
			{
				(*image)[y][x] = 255 << 24 | iterations << 16; //red
			}
			else if (colour == 2)
			{
				(*image)[y][x] = 255 << 24 | iterations << 8; //green
			}
			else if (colour == 3)
			{
				//white
				(*image)[y][x] = 255 << 24 | iterations << 16 | iterations << 8 | iterations;
			}	
			else
			{
				//let there be colour!
				(*image)[y][x] = 255 << 24 | ((7 * iterations) % 255) << 16 | ((iterations * 3) % 255) << 8 | (iterations % 255);
			}
		}
	}
}
