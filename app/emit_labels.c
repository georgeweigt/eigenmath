#include "app.h"

void
emit_labels(void)
{
	double d, h, w, x, y;

	// ymax

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_TOP_PAD + h;

	emit_draw(x, y, p1);

	// ymin

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_TOP_PAD + GDIM;

	emit_draw(x, y, p1);

	// xmax

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD + GDIM - w / 2.0;
	y = DRAW_TOP_PAD + GDIM + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);

	// xmin

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w / 2.0;
	y = DRAW_TOP_PAD + GDIM + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);
}
