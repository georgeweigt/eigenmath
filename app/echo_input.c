#include "app.h"

void
echo_input(char *s)
{
	emit_text(s, (int) strlen(s), BLUE);
}
