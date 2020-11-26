#include "app.h"

// returns height of x axis labels

double
emit_labels(void)
{
	double d, h, t, w, x, y;

	// ymax

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = h;

	emit_draw(x, y, p1);

	// ymin

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_HEIGHT;

	emit_draw(x, y, p1);

	// xmax

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD + DRAW_WIDTH - w / 2.0; // center
	y = DRAW_HEIGHT + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);

	t = h;

	// xmin

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w / 2.0; // center
	y = DRAW_HEIGHT + DRAW_LABEL_PAD + h;

	emit_draw(x, y, p1);

	return fmax(t, h);
}
