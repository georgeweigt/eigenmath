#include "app.h"

void
draw_formula(float x, float y, float *p)
{
	int k;

	CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 0.0, 1.0); // black
	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0); // black

	for (;;) {

		k = (int) p[0];

		if (k == DRAW_CHAR) {
			draw_char(x + p[1], y + p[2], (int) p[3], (int) p[4]);
			p += 5;
			continue;
		}

		if (k == DRAW_STROKE) {
			draw_stroke(x + p[1], y + p[2], x + p[3], y + p[4], p[5]);
			p += 6;
			continue;
		}

		break;
	}
}
