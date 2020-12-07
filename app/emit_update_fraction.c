#include "app.h"

void
emit_update_fraction(void)
{
	int font_num, opcode;
	double d, h, m, v, w;

	save();

	p2 = pop(); // denominator
	p1 = pop(); // numerator

	h = HEIGHT(p1) + DEPTH(p1);
	d = HEIGHT(p2) + DEPTH(p2);
	w = fmax(WIDTH(p1), WIDTH(p2));

	if (emit_level == 0) {
		opcode = EMIT_FRACTION;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_FRACTION;
		font_num = SMALL_ROMAN_FONT;
	}

	m = get_operator_height(font_num);

	v = 0.75 * m; // extra vertical space

	h += v + m;
	d += v - m;

	w += get_char_width(font_num, 'n') / 2.0; // make horizontal line a bit wider

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);
	push(p2);

	list(6);

	emit_count += 6; // alloc 6 for drawing horizontal line

	restore();
}
