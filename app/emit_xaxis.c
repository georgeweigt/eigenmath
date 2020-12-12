#include "app.h"

extern double ymin, ymax;

void
emit_xaxis(void)
{
	double x1, x2, y;

	y = DRAW_HEIGHT * (0.0 - ymin) / (ymax - ymin);
	y = DRAW_HEIGHT - y; // flip the y coordinate

	if (y <= 0 || y >= DRAW_HEIGHT)
		return;

	x1 = DRAW_LEFT_MARGIN;
	x2 = DRAW_LEFT_MARGIN + DRAW_WIDTH;

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y);
	emit_push(x2);
	emit_push(y);
	emit_push(DRAW_AXIS_STROKE);
}
