#include "app.h"

void
emit_update_fraction(void)
{
	double opcode, h, d, w;

	save();

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = HEIGHT(p1) + DEPTH(p1);
	d = HEIGHT(p2) + DEPTH(p2);
	w = fmax(WIDTH(p1), WIDTH(p2));

	if (emit_level == 0) {
		opcode = EMIT_FRACTION;
		h += FRAC_VSPACE + MINUS_HEIGHT;
		d += FRAC_VSPACE - MINUS_HEIGHT;
		w += get_width(TIMES_FONT, 'n') / 2.0;
	} else {
		opcode = EMIT_SMALL_FRACTION;
		h += SMALL_FRAC_VSPACE + SMALL_MINUS_HEIGHT;
		d += SMALL_FRAC_VSPACE - SMALL_MINUS_HEIGHT;
		w += get_width(SMALL_TIMES_FONT, 'n') / 2.0;
	}

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);
	push(p2);

	list(6);

	emit_count += 6; // alloc 6 floats for drawing horizontal line

	restore();
}
