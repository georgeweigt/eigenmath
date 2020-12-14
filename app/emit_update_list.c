#include "app.h"

void
emit_update_list(int t)
{
	save();
	emit_update_list_nib(t);
	restore();
}

void
emit_update_list_nib(int t)
{
	int i, n;
	double d, h, w;

	n = tos - t;

	if (n < 2)
		return;

	h = 0.0;
	d = 0.0;
	w = 0.0;

	for (i = t; i < tos; i++) {
		p1 = stack[i];
		h = fmax(h, HEIGHT(p1));
		d = fmax(d, DEPTH(p1));
		w += WIDTH(p1);
	}

	list(n);
	p1 = pop();

	push_double(EMIT_LIST);
	push_double(h);
	push_double(d);
	push_double(w);
	push(p1);

	list(5);
}
