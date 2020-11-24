#include "app.h"

void
emit_update_superscript(void)
{
	double d, dx, dy, h, w, y;

	save();

	p2 = pop(); // exponent
	p1 = pop(); // base

	h = HEIGHT(p2);
	d = DEPTH(p2);
	w = WIDTH(p2);

	// y is height of base

	y = HEIGHT(p1);

	// adjust

	y -= (h + d) / 2.0;

	if (emit_level == 0)
		y = fmax(y, get_xheight(ROMAN_FONT));
	else
		y = fmax(y, get_xheight(SMALL_ROMAN_FONT));

	dx = 0.0;
	dy = -(y + d);

	h = y + h + d;
	d = 0.0;

	if (OPCODE(p1) == EMIT_SUBSCRIPT) {
		dx = -WIDTH(p1);
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
