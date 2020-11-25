#include "app.h"

void
draw_formula(float x, float y, float *p)
{
	int k;

	for (;;) {

		k = p[0];

		switch (k) {

		case DRAW_CHAR:
			draw_char(x + p[1], y + p[2], (int) p[3], (int) p[4]);
			p += 5;
			break;

		case DRAW_STROKE:
			draw_stroke(x + p[1], y + p[2], x + p[3], y + p[4], p[5]);
			p += 6;
			break;

		case DRAW_POINT:
			draw_point(x + p[1], y + p[2]);
			p += 3;
			break;

		default:
			return;
		}
	}
}
