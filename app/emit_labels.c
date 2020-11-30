#include "app.h"

void
emit_labels(void)
{
	double h, w, x, y;

	// ymax

	p1 = pop();

	h = HEIGHT(p1);
	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = h;

	emit_draw(x, y, p1);

	// ymin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w - DRAW_LABEL_PAD;
	y = DRAW_HEIGHT;

	emit_draw(x, y, p1);

	// xmax

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_PAD + DRAW_WIDTH - w / 2.0; // center
	y = DRAW_HEIGHT + DRAW_LABEL_PAD + get_ascent(SMALL_ROMAN_FONT);

	emit_draw(x, y, p1);

	// xmin

	p1 = pop();

	w = WIDTH(p1);

	x = DRAW_LEFT_PAD - w / 2.0; // center
	y = DRAW_HEIGHT + DRAW_LABEL_PAD + get_ascent(SMALL_ROMAN_FONT);

	emit_draw(x, y, p1);
}
