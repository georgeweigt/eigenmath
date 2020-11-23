#include "app.h"

void
emit_update_list(int t)
{
	int i;
	double h, d, w;

	if (tos - t == 1)
		return;

	save();

	h = 0.0;
	d = 0.0;
	w = 0.0;

	for (i = t; i < tos; i++) {
		p1 = stack[i];
		h = fmax(h, HEIGHT(p1));
		d = fmax(d, DEPTH(p1));
		w += WIDTH(p1);
	}

	list(tos - t);
	p1 = pop();

	push_double(EMIT_LIST);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);

	restore();
}
