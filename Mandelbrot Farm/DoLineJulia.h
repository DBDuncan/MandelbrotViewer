#pragma once

#include <cstdint>
#include <complex>

#include "Task.h"
#include "Common.h"


class DoLineJulia : public Task
{
	uint32_t(*image)[HEIGHT][WIDTH];
	Bounds boundsData;
	int y;
	int colour;
	double yOffset;
	double xOffset;
	double zoom;
	int maxIterations;

public:
	DoLineJulia(uint32_t(*image)[HEIGHT][WIDTH], double left, double right, double top, double bottom, int y, int setColour, double yOffset, double xOffset, double zoom, int maxIterations);
	~DoLineJulia();

	void run();
};

