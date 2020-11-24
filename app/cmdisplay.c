#include "app.h"

void
cmdisplay(void)
{
	double d, h, w, x, y;

	save();

	p1 = pop();

	emit_level = 0;
	emit_index = 0;
	emit_count = 1; // for DRAW_END

	emit_list(p1);

	emit_display = malloc(sizeof (struct display) + emit_count * sizeof (float));

	if (emit_display == NULL)
		malloc_kaput();

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	x = 0.0;
	y = h;

	emit_draw(x, y, p1);

	emit_push(DRAW_END);

	emit_display->type = 2;

	emit_display->h = (int) ceil(h + d);
	emit_display->w = (int) ceil(w);

	shipout(emit_display);

	restore();
}
