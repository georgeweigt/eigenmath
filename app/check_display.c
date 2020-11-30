#include "app.h"

int
check_display(void)
{
	if (fence == atomic_load(&display_list))
		return 0; // nothing changed
	else
		return 1;
}
