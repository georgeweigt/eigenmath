#include "app.h"

int
check_display(void)
{
	if (barrier == last && cleared == 0)
		return 0; // nothing changed
	else
		return 1;
}
