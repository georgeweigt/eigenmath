#include "app.h"

void
emit_right_delim(double x, double y, double h, double d, double w, double stroke_width)
{
	double x1, x2, y1, y2;

	x1 = x + 0.75 * w;
	x2 = x + 0.25 * w;

	y1 = y - h + stroke_width / 2.0;
	y2 = y + d - stroke_width / 2.0;

	// stem stroke

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x1);
	emit_push(y2);
	emit_push(stroke_width);

	// top stroke

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y1);
	emit_push(stroke_width);

	// bottom stroke

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y2);
	emit_push(x2);
	emit_push(y2);
	emit_push(stroke_width);
}
