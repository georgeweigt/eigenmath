#include "app.h"

void
emit_draw(double x, double y, struct atom *p)
{
	int k;
	double d, dx, dy, h, stroke_width, w;

	k = (int) OPCODE(p);
	h = HEIGHT(p);
	d = DEPTH(p);
	w = WIDTH(p);

	p = cddddr(p);

	switch (k) {

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
		emit_delims(x, y, h, d, w, MEDIUM_STROKE, ROMAN_FONT);
		x += get_char_width(ROMAN_FONT, '(');
		emit_draw(x, y, car(p));
		break;

	case EMIT_SMALL_SUBEXPR:
		emit_delims(x, y, h, d, w, THIN_STROKE, SMALL_ROMAN_FONT);
		x += get_char_width(SMALL_ROMAN_FONT, '(');
		emit_draw(x, y, car(p));
		break;

	case EMIT_FRACTION:
	case EMIT_SMALL_FRACTION:

		// horizontal line

		if (k == EMIT_FRACTION) {
			dy = get_char_height(ROMAN_FONT) / 2.0;
			stroke_width = MEDIUM_STROKE;
		} else {
			dy = get_char_height(SMALL_ROMAN_FONT) / 2.0;
			stroke_width = THIN_STROKE;
		}

		emit_push(DRAW_STROKE);
		emit_push(x);
		emit_push(y - dy);
		emit_push(x + w);
		emit_push(y - dy);
		emit_push(stroke_width);

		// numerator

		dx = (w - WIDTH(car(p))) / 2.0;
		dy = -h + HEIGHT(car(p));
		emit_draw(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - WIDTH(car(p))) / 2.0;
		dy = d - DEPTH(car(p));
		emit_draw(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		emit_delims(x, y, h, d, w, THICK_STROKE, ROMAN_FONT);
		x += get_char_width(ROMAN_FONT, '(');
		emit_table(x, y - h, p);
		break;
	}
}
