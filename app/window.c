#include "app.h"

int malloc_kaput_flag;

static int total_h; // sum of the heights of all the items in the display queue
static int total_w; // the maximum width including right and left margins

static struct display *first, *last, *mark;

static int len;
static char buf[1000];

static void printcharf(int, int);

void
malloc_kaput(void)
{
	malloc_kaput_flag = 1;
	longjmp(stop_return, 1);
}

void
echo_input(char *s)
{
	printbuf(s, BLUE);
	printbuf("\n", BLUE);
}

void
printbuf(char *s, int color)
{
	while (*s)
		printchar(*s++, color);
}

void
printchar(int c, int color)
{
	printcharf(c, color);
	if (len >= 100)
		printcharf('\n', color);
}

static void
printcharf(int c, int color)
{
	struct display *d;

	if (c == '\n') {
		d = (struct display *) malloc(sizeof (struct display) + len + 1);
		if (d == NULL)
			malloc_kaput();
		d->len = len;
		buf[len] = 0;
		strcpy((char *) d->buf, buf);
		d->type = 0;
		d->attr = color;
		get_height_width(&d->h, &d->w, DEFAULT_FONT, (char *) d->buf);
		shipout(d);
		len = 0;
		return;
	}

	if (len < (int) sizeof buf)
		buf[len++] = c;
}

void
shipout(struct display *p)
{
	int w;

	total_h += p->h + 2 * VPAD;

	w = p->w + 2 * HPAD;

	if (w > total_w)
		total_w = w;

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

extern void drawstring(int, int, char *, int);
extern void drawpixel(int, int, int, int);
extern void draw_left_bracket(int, int, int, int);
extern void draw_right_bracket(int, int, int, int);
extern void draw_line(int, int, int, int);

static void draw(struct display *, int, int);

static int yy;
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
	struct display *p;
	if (first == NULL)
		return;
	yy = 0;
	p = first;
	draw(p, y1, y2);
	while (p != mark) {
		p = p->next;
		draw(p, y1, y2);
	}
}

#define N(x) (((int) buf[k + x] << 24) | ((int) buf[k + x + 1] << 16) | ((int) buf[k + x + 2]) << 8 | ((int) buf[k + x + 3]))

static void
draw(struct display *p, int yy1, int yy2)
{
	int w, xx;

	w = p->w;

	xx = HPAD;
	yy += VPAD;

	// clip to view rect

	if (yy + p->h < yy1 || yy > yy2) {
		yy += p->h + VPAD;
		return;
	}

	switch (p->type) {

	case 0:
		draw_text(DEFAULT_FONT, xx, yy, p->buf, p->len, p->attr);
		break;

	case 2:
		draw_formula(xx, yy, p->tab);
		break;
	}

	yy += p->h + VPAD;
}

void
eval_exit(void)
{
	push_symbol(NIL);
}
