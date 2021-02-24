
#include <stdint.h>
#include <stddef.h>

#include "misc.h"
#include "display.h"

//Bresenham's Algorithm
//draws line between to points
static inline void _plothigh(size_t x1, size_t y1, size_t x2, size_t y2,
			     uint8_t value)
{
	size_t x, y;
	int d;
	int dx = x2 - x1;
	int dy = y2 - y1;

	int xi = 1;

	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}

	d = 2 * dx - dy;
	x = x1;

	for (y = y1; y < y2; y++) {
		drawpixel_il0373(x, y, value);

		if (d > 0) {
			x += xi;
			d += 2 * (dx - dy);
		} else {
			d += 2 * dx;
		}
	}
}

static inline void _plotlow(size_t x1, size_t y1, size_t x2, size_t y2,
			    uint8_t value)
{
	size_t y, x;
	int d;
	int dx = x2 - x1;
	int dy = y2 - y1;

	int yi = 1;

	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}

	d = 2 * dy - dx;
	y = y1;

	for (x = x1; x <= x2; x++) {
		drawpixel_il0373(x, y, value);

		if (d > 0) {
			y += yi;
			d += 2 * (dy - dx);
		} else {
			d += 2 * dy;
		}
	}
}

// bugs
// doenst recognise 90 deg turns
void drawline(size_t x1, size_t y1, size_t x2, size_t y2, uint8_t value)
{
	if ((y2 - y1) < (x2 - x1)) {
		if (x1 > x2) {
			_plotlow(x2, y2, x1, y1, value);
		} else {
			_plotlow(x1, y1, x2, y2, value);
		}
	} else {
		if (y1 > y2) {
			_plothigh(x2, y2, x1, y1, value);
		} else {
			_plothigh(x1, y1, x2, y2, value);
		}
	}
}
