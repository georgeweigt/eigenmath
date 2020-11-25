#include "app.h"

void
get_view(double *h, double *w)
{
	mark = last;
	cleared = 0;
	if (mark) {
		*h = mark->tot_h;
		*w = mark->tot_w;
	} else {
		*h = 0.0;
		*w = 0.0;
	}
}
