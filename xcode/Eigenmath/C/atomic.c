#include "app.h"
#include <stdatomic.h>

// atomic ops needed for memory barrier

// memory barrier needed because cpu reorders memory writes

_Atomic(struct display *) display_ptr;

struct display *
get_display_ptr(void)
{
	return atomic_load(&display_ptr);
}

void
set_display_ptr(struct display *p)
{
	atomic_store(&display_ptr, p);
}
