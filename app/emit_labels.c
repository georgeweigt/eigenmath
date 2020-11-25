#include "app.h"

void
emit_labels(double xmin, double xmax, double ymin, double ymax)
{
	double d, h, w, x, y;

	// xmin

	push_double(xmin);
	p1 = pop();

	emit_level = 1; // small font
	emit_list(p1);
	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w / 2.0;
	y = DRAW_TOP_PAD + GDIM + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);

	// xmax

	push_double(xmax);
	p1 = pop();

	emit_level = 1; // small font
	emit_list(p1);
	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD + GDIM - w / 2.0;
	y = DRAW_TOP_PAD + GDIM + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);

	// ymin

	push_double(ymin);
	p1 = pop();

	emit_level = 1; // small font
	emit_list(p1);
	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_TOP_PAD + GDIM;

	emit_draw(x, y, p1);

	// ymax

	push_double(ymax);
	p1 = pop();

	emit_level = 1; // small font
	emit_list(p1);
	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_TOP_PAD + h;

	emit_draw(x, y, p1);
}
