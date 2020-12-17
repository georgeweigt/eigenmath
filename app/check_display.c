#include "app.h"

int
check_display(void)
{
	if (fence == get_display_ptr())
		return 0; // nothing changed
	else
		return 1;
}
