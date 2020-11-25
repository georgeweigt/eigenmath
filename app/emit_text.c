#include "app.h"

void
emit_text(char *buf, int len, int color)
{
	int i;
	double w;
	struct display *p;

	p = malloc(sizeof (struct display) + len);

	if (p == NULL)
		malloc_kaput();

	p->len = len;

	memcpy(p->buf, buf, len);

	p->type = 0;
	p->color = color;

	p->h = 0; // overlap with shipout padding

	w = 0.0;

	for (i = 0; i < len; i++)
		w += get_char_width(DEFAULT_FONT, buf[i]);

	p->w = ceil(w);

	shipout(p);
}
