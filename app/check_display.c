#include "app.h"

// app calls check_display() then draw_display()

int
check_display(void)
{
	if (fence == last && cleared == 0)
		return 0; // nothing changed

	cleared = 0;

	fence = last;

	if (fence) {
		document_height = fence->total_height;
		document_width = fence->total_width;
	} else {
		document_height = 0.0;
		document_width = 0.0;
	}

	return 1;
}
