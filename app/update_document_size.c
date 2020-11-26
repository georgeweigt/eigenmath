#include "app.h"

void
update_document_size(void)
{
	cleared = 0;

	barrier = last;

	if (barrier) {
		document_height = barrier->total_height;
		document_width = barrier->total_width;
	} else {
		document_height = 0.0;
		document_width = 0.0;
	}
}
