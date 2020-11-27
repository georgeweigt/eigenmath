#include "app.h"

int
check_display(void)
{
	if (fence == last)
		return 0; // nothing changed
	else
		return 1;
}
