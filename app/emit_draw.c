#include "app.h"

void
emit_draw(double x, double y, struct atom *p)
{
	double d, dx, dy, h, k, stroke_width, w;

	k = OPCODE(p);
	h = HEIGHT(p);
	d = DEPTH(p);
	w = WIDTH(p);

	p = cddddr(p);

	switch ((int) k) {

	case EMIT_SPACE:
		break;

	case EMIT_CHAR:
		emit_push(DRAW_CHAR);
		emit_push(x);
		emit_push(y);
		emit_push(VAL1(p)); // font number
		emit_push(VAL2(p)); // char number
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			emit_draw(x, y, car(p));
			x += WIDTH(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		x += VAL1(p);
		y += VAL2(p);
		p = caddr(p);
		emit_draw(x, y, p);
		break;

	case EMIT_SUBEXPR:
		emit_delims(x, y, h, d, w, DELIM_STROKE, ROMAN_FONT);
		x += get_char_width(ROMAN_FONT, '(');
		emit_draw(x, y, car(p));
		break;

	case EMIT_SMALL_SUBEXPR:
		emit_delims(x, y, h, d, w, SMALL_DELIM_STROKE, SMALL_ROMAN_FONT);
		x += get_char_width(SMALL_ROMAN_FONT, '(');
		emit_draw(x, y, car(p));
		break;

	case EMIT_FRACTION:
	case EMIT_SMALL_FRACTION:

		// horizontal line

		dy = VAL1(p);

		if (k == EMIT_FRACTION)
			stroke_width = FRAC_STROKE;
		else
			stroke_width = SMALL_FRAC_STROKE;

		emit_push(DRAW_STROKE);
		emit_push(x);
		emit_push(y - dy);
		emit_push(x + w);
		emit_push(y - dy);
		emit_push(stroke_width);

		// numerator

		p = cdr(p);
		dx = (w - WIDTH(car(p))) / 2.0;
		dy = HEIGHT(car(p)) - h;
		emit_draw(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - WIDTH(car(p))) / 2.0;
		dy = d - DEPTH(car(p));
		emit_draw(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		emit_delims(x, y, h, d, w, ROMAN_FONT, LARGE_DELIM_STROKE);
		x += get_char_width(ROMAN_FONT, '(');
		emit_table(x, y - h, p);
		break;
	}
}
