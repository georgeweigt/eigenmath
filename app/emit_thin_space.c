#include "app.h"

void
emit_thin_space(void)
{
	double w;

	if (emit_level == 0)
		w = 0.25 * get_char_width(ROMAN_FONT, 'n');
	else
		w = 0.25 * get_char_width(SMALL_ROMAN_FONT, 'n');

	push_double(EMIT_SPACE);
	push_double(0.0);
	push_double(0.0);
	push_double(w);

	list(4);
}
