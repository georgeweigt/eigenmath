#include "app.h"

int
check_display(void)
{
	if (fence == display_list)
		return 0; // nothing changed
	else
		return 1;
}
