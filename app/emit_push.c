#include "app.h"

void
emit_push(double d)
{
	if (emit_index == emit_count)
		stop("internal error: emit_push");
	emit_display->mem[emit_index++] = d;
}
