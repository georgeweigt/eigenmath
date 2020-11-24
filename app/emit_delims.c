#include "app.h"

void
emit_delims(double x, double y, double h, double d, double w, double stroke_width, int font_num)
{
	double cd, ch, cw;

	ch = get_char_height(font_num);
	cd = get_char_depth(font_num, '(');
	cw = get_char_width(font_num, '(');

	if (h > ch || d > cd) {
		emit_left_delim(x, y, h, d, cw, stroke_width);
		emit_right_delim(x + w - cw, y, h, d, cw, stroke_width);
		return;
	}

	emit_push(DRAW_CHAR);
	emit_push(x);
	emit_push(y);
	emit_push(font_num);
	emit_push('(');

	emit_push(DRAW_CHAR);
	emit_push(x + w - cw);
	emit_push(y);
	emit_push(font_num);
	emit_push(')');
}
