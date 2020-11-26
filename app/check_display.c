#include "app.h"

int
check_display(void)
{
	if (fence == last && cleared == 0)
		return 0; // nothing changed
	else
		return 1;
}
