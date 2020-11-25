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

extern void draw_point(int, int, int, int);
extern void draw_box(int, int, int, int);

#define N(x) (((int) buf[k + x] << 24) | ((int) buf[k + x + 1] << 16) | ((int) buf[k + x + 2]) << 8 | ((int) buf[k + x + 3]))

static void
draw(struct display *p, int yy1, int yy2)
{
	int cmd, k, len, w, xx, x, x1, x2, y, y1, y2;
	uint8_t *buf;

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

	case 1:
		buf = (uint8_t *) p->buf;
		k = 0;
		while (1) {
			cmd = buf[k];
			if (cmd == 0)
				break;
			switch (cmd) {
			case 1:
				x = 256 * buf[k + 1] + buf[k + 2];
				y = 256 * buf[k + 3] + buf[k + 4];
				if (x > 32767)
					x -= 65536;
				if (y > 32767)
					y -= 65536;
				len = buf[k + 5];
				draw_text(SMALL_FONT, xx + x, yy + y, buf + k + 6, len, 0); // small font
				k = k + 6 + len;
				break;
			case 2: // default font
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				x = N(1);
				y = N(5);
				len = N(9);
				draw_text(cmd, xx + x, yy + y, buf + k + 13, len, 0);
				k = k + 13 + len;
				break;
			case DRAW_LINE:
				x1 = 256 * buf[k + 1] + buf[k + 2];
				y1 = 256 * buf[k + 3] + buf[k + 4];
				x2 = 256 * buf[k + 5] + buf[k + 6];
				y2 = 256 * buf[k + 7] + buf[k + 8];
				draw_line(xx + x1, yy + y1, xx + x2, yy + y2);
				k += 9;
				break;
			case DRAW_POINT:
				x = 256 * buf[k + 1] + buf[k + 2];
				y = 256 * buf[k + 3] + buf[k + 4];
				draw_point(xx, x, yy, y);
				k += 5;
				break;
			case DRAW_BOX:
				x1 = 256 * buf[k + 1] + buf[k + 2];
				y1 = 256 * buf[k + 3] + buf[k + 4];
				x2 = 256 * buf[k + 5] + buf[k + 6];
				y2 = 256 * buf[k + 7] + buf[k + 8];
				draw_box(xx + x1, yy + y1, xx + x2, yy + y2);
				k += 9;
				break;
			default:
				buf[4] = 0; // error, force stop
				k = 4;
				break;
			}
		}

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