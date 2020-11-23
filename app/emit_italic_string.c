#include "app.h"

void
emit_italic_string(char *s)
{
	while (*s)
		emit_italic_char(*s++);
}
