#include "app.h"

// 12.0625 + 3.9375 = 16.0 for TEXT_FONT

void
emit_text(char *buf, int len, int color)
{
	int i;
	double d, h, w;
	struct display *p;

	p = malloc(sizeof (struct display) + len);

	if (p == NULL)
		malloc_kaput();

	p->len = len;

	memcpy(p->buf, buf, len);

	h = get_ascent(TEXT_FONT) + 1.0;
	d = get_descent(TEXT_FONT) + 3.0;

	w = 0.0;

	for (i = 0; i < len; i++)
		w += get_char_width(TEXT_FONT, buf[i]);

	p->type = 0;
	p->color = color;

	p->height = round(h + d);
	p->width = HPAD + w + HPAD;

	p->dx = HPAD;
	p->dy = round(h);

	shipout(p);
}
