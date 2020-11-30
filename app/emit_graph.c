#include "app.h"

void
emit_graph(void)
{
	int i;
	double x, y;

	emit_level = 1; // small font
	emit_index = 0;
	emit_count = 3 * draw_count + 37; // 37 = 1 for DRAW_END and 6 for each stroke (6 strokes)

	push_double(xmin);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(xmax);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(ymin);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	push_double(ymax);
	p1 = pop();
	emit_list(p1); // advance emit_count, leave result on stack

	emit_display = malloc(sizeof (struct display) + emit_count * sizeof (float));

	if (emit_display == NULL)
		malloc_kaput();

	emit_labels(); // uses the above results on stack

	emit_box();

	emit_xaxis();
	emit_yaxis();

	for (i = 0; i < draw_count; i++) {

		x = draw_buf[i].x;
		y = draw_buf[i].y;

		if (x < 0 || x > DRAW_WIDTH || y < 0 || y > DRAW_HEIGHT)
			continue;

		x += DRAW_LEFT_MARGIN;
		y = DRAW_HEIGHT - y;

		emit_push(DRAW_POINT);
		emit_push(x);
		emit_push(y);
	}

	emit_push(DRAW_END);

	emit_display->type = 1;
	emit_display->color = BLACK;

	emit_display->height = VPAD + DRAW_HEIGHT + DRAW_XLABEL_MARGIN + VPAD;
	emit_display->width = DRAW_LEFT_MARGIN + DRAW_WIDTH + DRAW_RIGHT_MARGIN;

	emit_display->dx = 0.0;
	emit_display->dy = VPAD;

	shipout(emit_display);
}
