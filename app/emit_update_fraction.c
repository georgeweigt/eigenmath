#include "app.h"

void
emit_update_fraction(void)
{
	int font_num, opcode;
	double d, dy, h, w;

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

	dy = get_operator_height(font_num); // approximate height of '-'

	h += dy + get_underline_position(font_num);
	d -= dy;
	w += get_width(font_num, 'n') / 2.0;

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(dy);
	push(p1);
	push(p2);

	list(7);

	emit_count += 6; // alloc 6 floats for drawing horizontal line

	restore();
}
