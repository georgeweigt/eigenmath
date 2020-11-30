#include "app.h"

// called before draw_display()

void
prep_display(void)
{
	fence = atomic_load(&display_list);

	if (fence) {
		document_height = fence->total_height;
		document_width = fence->total_width;
	} else {
		document_height = 0.0;
		document_width = 0.0;
	}
}
