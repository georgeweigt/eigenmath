#include "app.h"

int
check_display(void)
{
	if (mark == last && cleared == 0)
		return 0;
	else
		return 1;
}
