#include "app.h"

// called from a timer interrupt so don't change anything,
// parent process might be in the middle of draw_display()

int
check_display(void)
{
	if (fence == last && cleared == 0)
		return 0; // nothing changed
	else
		return 1;
}
