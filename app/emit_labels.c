#include "app.h"

void
emit_labels(void)
{
	double h, w, x, y;

	// ymax

	p1 = pop();

	h = HEIGHT(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w - DRAW_YLABEL_MARGIN;
	y = h;

	emit_draw(x, y, p1);

	// ymin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w - DRAW_YLABEL_MARGIN;
	y = DRAW_HEIGHT;

	emit_draw(x, y, p1);

	// xmax

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN + DRAW_WIDTH - w / 2.0; // center horizontally
	y = DRAW_HEIGHT + DRAW_XLABEL_MARGIN;

	emit_draw(x, y, p1);

	// xmin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_MARGIN - w / 2.0; // center horizontally
	y = DRAW_HEIGHT + DRAW_XLABEL_MARGIN;

	emit_draw(x, y, p1);
}
