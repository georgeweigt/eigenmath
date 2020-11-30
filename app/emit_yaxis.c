#include "app.h"

extern double xmin, xmax;

void
emit_yaxis(void)
{
	double x, y1, y2;

	x = DRAW_WIDTH * (0.0 - xmin) / (xmax - xmin);

	if (x <= 0 || x >= DRAW_WIDTH)
		return;

	x += DRAW_LEFT_MARGIN;

	y1 = 0.0;
	y2 = DRAW_HEIGHT;

	emit_push(DRAW_STROKE);
	emit_push(x);
	emit_push(y1);
	emit_push(x);
	emit_push(y2);
	emit_push(VERY_THIN_STROKE);
}
