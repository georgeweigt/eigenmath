#include "app.h"

int malloc_kaput_flag;

static int total_h; // sum of the heights of all the items in the display queue
static int total_w; // the maximum width including right and left margins

static struct display *first, *last, *mark;

void
malloc_kaput(void)
{
	malloc_kaput_flag = 1;
	longjmp(stop_return, 1);
}

void
echo_input(char *s)
{
	emit_text(s, (int) strlen(s), BLUE);
}

void
printbuf(char *s, int color)
{
	int len;
	char *t;

	for (;;) {

		t = strchr(s, '\n');

		if (t == NULL)
			break;

		len = (int) (t - s);

		emit_text(s, len, color);

		s = t + 1;
	}

	if (*s) {
		len = (int) strlen(s);
		emit_text(s, len, color);
	}
}

void
shipout(struct display *p)
{
	p->h += 2 * VPAD;
	p->w += 2 * HPAD;

	total_h += p->h;

	if (p->w > total_w)
		total_w = p->w;

	p->tot_h = total_h;
	p->tot_w = total_w;

	p->next = NULL;

	if (first == NULL) {
		last = p;
		first = p;
	} else {
		last->next = p;
		last = p;
	}
}

static int cleared;

void
clear_display(void)
{
	struct display *t;
	while (first) {
		t = first;
		first = first->next;
		free(t);
	}
	last = NULL;
	mark = NULL;
	total_w = 0;
	total_h = 0;
	cleared = 1;
}

int
check_display(void)
{
	if (mark == last && cleared == 0)
		return 0;
	else
		return 1;
}

void
get_view(int *h, int *w)
{
	mark = last;
	cleared = 0;
	if (mark) {
		*h = mark->tot_h;
		*w = mark->tot_w;
	} else {
		*h = 0;
		*w = 0;
	}
}

void
draw_display(int y1, int y2)
{
	int y;
	struct display *p;
	y = 0;
	p = first;
	while (p) {
		draw_block(p, y, y1, y2);
		y += p->h;
		if (p == mark)
			break;
		p = p->next;
	}
}

void
draw_block(struct display *p, int y, int y1, int y2)
{
	// clip to view rect

	if (y + p->h < y1 || y > y2)
		return;

	switch (p->color) {
	case BLACK:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 0.0, 1);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);
		break;
	case BLUE:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		break;
	case RED:
		CGContextSetRGBFillColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		break;
	}

	switch (p->type) {

	case 0:
		draw_text(HPAD, y, p->buf, p->len);
		break;

	case 2:
		draw_formula(HPAD, y + VPAD, p->tab);
		break;
	}
}

void
eval_exit(void)
{
	push_symbol(NIL);
}
