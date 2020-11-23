#include "app.h"

void
emit_roman_string(char *s)
{
	while (*s)
		emit_roman_char(*s++);
}
