#include "app.h"

void
emit_infix_operator(int c)
{
	emit_thick_space();
	emit_roman_char(c);
	emit_thick_space();
}
