#include "app.h"

void
emit_box(void)
{
	double x1, x2, y1, y2;

	x1 = DRAW_LEFT_MARGIN;
	x2 = DRAW_LEFT_MARGIN + DRAW_WIDTH;

	y1 = 0.0;
	y2 = DRAW_HEIGHT;

	// left

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x1);
	emit_push(y2);
	emit_push(VERY_THIN_STROKE);

	// right

	emit_push(DRAW_STROKE);
	emit_push(x2);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(VERY_THIN_STROKE);

	// top

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y1);
	emit_push(VERY_THIN_STROKE);

	// bottom

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y2);
	emit_push(x2);
	emit_push(y2);
	emit_push(VERY_THIN_STROKE);
}
