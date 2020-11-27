#include "app.h"

void
clear_display(void)
{
	struct display *p;

	if (running)
		return;

	while (display_list) {
		p = display_list;
		display_list = display_list->next;
		free(p);
	}

	fence = NULL;

	total_height = 0.0;
	total_width = 0.0;
}
