#include "app.h"

void
display(void)
{
	double d, h, w;
	struct atom *p1;

	p1 = pop();

	emit_level = 0;
	emit_index = 0;
	emit_count = 1; // for DRAW_END

	if (isstr(p1))
		emit_text_mode = 1;
	else
		emit_text_mode = 0;

	emit_list(p1);

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	emit_display = malloc(sizeof (struct display) + emit_count * sizeof (double));

	if (emit_display == NULL)
		exit(1);

	emit_formula(0.0, 0.0, p1);

	emit_push(DRAW_END);

	emit_display->type = 1;
	emit_display->color = BLACK;

	emit_display->height = VPAD + h + d + VPAD;
	emit_display->width = HPAD + w + HPAD;

	emit_display->dx = HPAD;
	emit_display->dy = VPAD + h;

	shipout(emit_display);
}
