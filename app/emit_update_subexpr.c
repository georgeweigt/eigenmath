#include "app.h"

void
emit_update_subexpr(void)
{
	int font_num, opcode;
	double d, h, m, w, y;

	save();

	p1 = pop();

	h = HEIGHT(p1);
	d = DEPTH(p1);
	w = WIDTH(p1);

	if (emit_level == 0) {
		opcode = EMIT_SUBEXPR;
		font_num = ROMAN_FONT;
	} else {
		opcode = EMIT_SMALL_SUBEXPR;
		font_num = SMALL_ROMAN_FONT;
	}

	h = fmax(h, get_cap_height(font_num));
	d = fmax(d, get_descent(font_num));

	// delimiters have vertical symmetry

	if (h > get_cap_height(font_num) || d > get_descent(font_num)) {
		m = get_operator_height(font_num);
		y = fmax(h - m, d + m);
		h = y + m;
		d = y - m;
		h += 0.25 * get_cap_height(font_num);
		d += 0.25 * get_cap_height(font_num);
	}

	w += 2.0 * get_char_width(font_num, '(');

	push_double(opcode);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);

	emit_count += 36; // alloc 36 for drawing delimiters

	restore();
}
