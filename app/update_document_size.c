#include "app.h"

void
update_document_size(void)
{
	mark = last;
	cleared = 0;
	if (mark) {
		document_height = mark->tot_h;
		document_width = mark->tot_w;
	} else {
		document_height = 0.0;
		document_width = 0.0;
	}
}
