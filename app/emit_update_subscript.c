#include "app.h"

void
emit_update_subscript(void)
{
	int font_num;
	double d, dx, dy, h, w;

	save();

	p1 = pop();

	d = DEPTH(p1);
	w = WIDTH(p1);

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	h = get_cap_height(font_num);

	dx = 0.0;
	dy = h / 2.0;

	d += dy;

	push_double(EMIT_SUBSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p1);

	list(7);

	restore();
}
