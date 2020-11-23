#include "app.h"

void
emit_italic_char(int char_num)
{
	double d, font_num, h, w;

	if (emit_level == 0)
		font_num = ITALIC_FONT;
	else
		font_num = SMALL_ITALIC_FONT;

	h = get_char_height(font_num);
	d = get_char_depth(font_num);
	w = get_char_width(font_num, char_num);

	push_double(EMIT_CHAR);
	push_double(h);
	push_double(d);
	push_double(w);
	push_double(font_num);
	push_double(char_num);

	list(6);

	emit_count += 5; // alloc 5 floats for drawing char
}
