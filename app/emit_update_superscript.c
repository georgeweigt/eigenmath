#include "app.h"

void
emit_update_superscript(void)
{
	int font_num;
	double d, dx, dy, h, t, w, y;

	save();

	p2 = pop(); // exponent
	p1 = pop(); // base

	if (emit_level == 0)
		font_num = ROMAN_FONT;
	else
		font_num = SMALL_ROMAN_FONT;

	t = get_char_width(font_num, 'n') / 6.0;

	h = HEIGHT(p2);
	d = DEPTH(p2);
	w = t + WIDTH(p2);

	// y is height of base

	y = HEIGHT(p1);

	// adjust

	y -= (h + d) / 2.0;

	y = fmax(y, get_xheight(font_num));

	dx = t;
	dy = -(y + d);

	h = y + h + d;
	d = 0.0;

	if (OPCODE(p1) == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1) + t;
		w = fmax(0.0, w - WIDTH(p1));
	}

	push(p1); // base

	push_double(EMIT_SUPERSCRIPT);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dx);
	push_double(dy);
	push(p2);

	list(7);

	restore();
}
