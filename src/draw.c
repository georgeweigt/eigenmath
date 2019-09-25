#include "defs.h"

extern int text_width(int, char *);

extern struct text_metric text_metric[11];

#define SMALL_FONT 1
#define DEFAULT_FONT 2
#define DRAW_LINE 23
#define DRAW_POINT 24
#define DRAW_BOX 25

#define DIM 300

#define F p3
#define T p4
#define X p5
#define Y p6
#define XT p7
#define YT p8

static double tmin, tmax;
static double xmin, xmax;
static double ymin, ymax;

#define YMAX 10000

static struct {
	int x, y;
	double t;
} draw_buf[YMAX];

static int draw_count;

void
eval_draw(void)
{
	push(cadr(p1)); // 1st arg

	T = caddr(p1); // 2nd arg

	if (T == symbol(NIL)) {
		guess();
		T = pop();
	}

	F = pop();

	push_binding(T); // save binding of 2nd arg

	draw_main();

	pop_binding(T); // restore binding of 2nd arg

	push_symbol(NIL); // draw returns nil
}

void
draw_main(void)
{
	if (draw_flag) {
		draw_flag = 0; // so "stop" really stops
		stop("draw calls draw");
	}

	draw_flag++;

	setup_trange();

	setup_xrange();

	setup_yrange();

	check_for_parametric_draw();

	create_point_set();

	emit_graph();

	draw_flag--;
}

/*	xrange sets the horizontal scale

	yrange sets the vertical scale

	Normally, the function F is evaluated from xrange[1] to xrange[2].

	However, if F returns a vector then parametric drawing is used. In this
	case F is evaluated from trange[1] to trange[2].
*/

void
check_for_parametric_draw(void)
{
	eval_f(tmin);
	p1 = pop();
	if (!istensor(p1)) {
		tmin = xmin;
		tmax = xmax;
	}
}

#define N 101 // fix for scruffy line, for example, draw(-x)

void
create_point_set(void)
{
	int i, n;
	double t;

	draw_count = 0;

	for (i = 0; i <= N; i++) {
		t = tmin + ((double) i / (double) N) * (tmax - tmin);
		new_point(t);
	}

	n = draw_count;

	for (i = 0; i < n - 1; i++)
		fill(i, i + 1, 0);
}

void
new_point(double t)
{
	double x, y;

	if (draw_count >= YMAX)
		return;

	get_xy(t);

	if (!isnum(XT) || !isnum(YT))
		return;

	push(XT);
	x = pop_double();

	push(YT);
	y = pop_double();

	x = DIM * (x - xmin) / (xmax - xmin);
	y = DIM * (y - ymin) / (ymax - ymin);

	x = round(x);
	y = round(y);

	draw_buf[draw_count].x = x;
	draw_buf[draw_count].y = y;
	draw_buf[draw_count].t = t;

	draw_count++;
}

// Evaluate F(t) and return in XT and YT.

void
get_xy(double t)
{
	eval_f(t);

	p1 = pop();

	if (istensor(p1)) {
		if (p1->u.tensor->nelem >= 2) {
			XT = p1->u.tensor->elem[0];
			YT = p1->u.tensor->elem[1];
		} else {
			XT = symbol(NIL);
			YT = symbol(NIL);
		}
		return;
	}

	push_double(t);
	XT = pop();
	YT = p1;
}

// Evaluate F(t) without stopping due to an error such as divide by zero.

void
eval_f(double t)
{
	// These must be volatile or it crashes. (Compiler error?)
	// Read it backwards, save_tos is a volatile int, etc.

	int volatile save_tos;
	int volatile save_tof;

	save();
	save_tos = tos;
	save_tof = tof;

	draw_flag++;

	if (setjmp(draw_stop_return)) {
		tos = save_tos;
		push_symbol(NIL);
		tof = save_tof;
		restore();
		draw_flag--;
		return;
	}

	push_double(t);
	p1 = pop();
	set_binding(T, p1);

	push(F);
	eval();
	float_expr();

	restore();
	draw_flag--;
}

static int
invisible(int i)
{
	if (draw_buf[i].x < 0 || draw_buf[i].x > DIM)
		return 1;
	if (draw_buf[i].y < 0 || draw_buf[i].y > DIM)
		return 1;
	return 0;
}

#define M 3 // use 3 to avoid scruffy lines, for example, draw(-x)

// divide segment P(i) to P(j) into M segments
//
// P(i)    P(m)   P(n-1)   P(j)
//   o-------o-------o-------o

void
fill(int i, int j, int level)
{
	int dx, dy, k, m, n;
	double d, t;

	if (level > 5)
		return;

	if (invisible(i) && invisible(j))
		return;

	dx = abs(draw_buf[i].x - draw_buf[j].x);
	dy = abs(draw_buf[i].y - draw_buf[j].y);

	if (dx + dy < 2)
		return;

	d = draw_buf[j].t - draw_buf[i].t;

	m = draw_count;

	for (k = 1; k < M; k++) {
		t = draw_buf[i].t + (double) k / M * d;
		new_point(t);
	}

	n = draw_count;

	if (m == n)
		return; // no new plot points

	fill(i, m, level + 1); // fill first segment

	for (k = m; k < n - 1; k++)
		fill(k, k + 1, level + 1); // fill middle segments

	fill(n - 1, j, level + 1); // fill last segment
}

//	Normalize x to [0,1]
//
//	Example: xmin = -10, xmax = 10, xmax - xmin = 20
//
//	x		x - xmin	(x - xmin) / (xmax - xmin)
//
//	-10		0		0.00
//
//	-5		5		0.25
//
//	0		10		0.50
//
//	5		15		0.75
//
//	10		20		1.00

void
setup_trange(void)
{
	save();
	setup_trange_f();
	restore();
}

void
setup_trange_f(void)
{
	// default range is (-pi, pi)

	tmin = -M_PI;

	tmax = M_PI;

	p1 = usr_symbol("trange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	float_expr();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	float_expr();
	p3 = pop();

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	tmin = pop_double();

	push(p3);
	tmax = pop_double();

	if (tmin == tmax)
		stop("draw: trange is zero");
}

void
setup_xrange(void)
{
	save();
	setup_xrange_f();
	restore();
}

void
setup_xrange_f(void)
{
	// default range is (-10,10)

	xmin = -10.0;

	xmax = 10.0;

	p1 = usr_symbol("xrange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	float_expr();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	float_expr();
	p3 = pop();

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	xmin = pop_double();

	push(p3);
	xmax = pop_double();

	if (xmin == xmax)
		stop("draw: xrange is zero");
}

//	Example: yrange=(-10,10)
//
//	y	d	v (vertical pixel coordinate)
//
//	10	0.00	0
//
//	5	0.25	100
//
//	0	0.50	200
//
//	-5	0.75	300
//
//	-10	1.00	400
//
//	We have
//
//		d = (10 - y) / 20
//
//	          = (B - y) / (B - A)
//
//	where yrange=(A,B)
//
//	To convert d to v, multiply by N where N = 400.

void
setup_yrange(void)
{
	save();
	setup_yrange_f();
	restore();
}

void
setup_yrange_f(void)
{
	// default range is (-10,10)

	ymin = -10.0;

	ymax = 10.0;

	p1 = usr_symbol("yrange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	float_expr();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	float_expr();
	p3 = pop();

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	ymin = pop_double();

	push(p3);
	ymax = pop_double();

	if (ymin == ymax)
		stop("draw: yrange is zero");
}

#define XOFF (72 + 36) // moves the entire plot 1.5 inches to the right
#define YOFF 9 // 0.125 inch above and below the plot

#define SHIM 9

static int k;
static uint8_t *buf;

static void emit_box(void);
static void emit_xaxis(void);
static void emit_yaxis(void);
static void emit_xscale(void);
static void emit_yscale(void);
static void emit_xzero(void);
static void emit_yzero(void);
static void get_xzero(void);
static void get_yzero(void);

static int xzero, yzero;

void
emit_graph(void)
{
	int i, len, x, y;
	struct display *d;

	get_xzero();
	get_yzero();

	len = 1000 + 5 * draw_count;

	d = (struct display *) malloc(sizeof (struct display) + len);

	if (d == NULL)
		malloc_kaput();

	d->len = len;
	d->type = 1;
	d->attr = 0;
	d->w = XOFF + DIM + XOFF;
	d->h = YOFF + DIM + SHIM + text_metric[SMALL_FONT].ascent + text_metric[SMALL_FONT].descent + YOFF;

	buf = d->buf;

	k = 0;

	emit_box();

	emit_xaxis();
	emit_yaxis();

	emit_xscale();
	emit_yscale();

	emit_xzero();
	emit_yzero();

	for (i = 0; i < draw_count; i++) {
		x = draw_buf[i].x;
		y = DIM - draw_buf[i].y; // flip the y coordinate
		if (x < 0 || x > DIM)
			continue;
		if (y < 0 || y > DIM)
			continue;
		x += XOFF;
		y += YOFF;
		buf[k++] = DRAW_POINT;
		buf[k++] = (uint8_t) (x >> 8);
		buf[k++] = (uint8_t) x;
		buf[k++] = (uint8_t) (y >> 8);
		buf[k++] = (uint8_t) y;
	}

	buf[k++] = 0;

	shipout(d);
}

static void
get_xzero(void)
{
	double x;
	x = DIM * (0.0 - xmin) / (xmax - xmin);
	x = round(x);
	if (x < -10000.0)
		x = -10000.0;
	if (x > 10000.0)
		x = 10000.0;
	xzero = x;
}

static void
get_yzero(void)
{
	double y;
	y = DIM * (0.0 - ymin) / (ymax - ymin);
	y = round(y);
	if (y < -10000.0)
		y = -10000.0;
	if (y > 10000.0)
		y = 10000.0;
	yzero = DIM - y; // flip the y coordinate
}

static void
emit_box(void)
{
	int x, y;

	buf[k++] = DRAW_BOX;

	x = XOFF;
	y = YOFF;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;

	x = XOFF + DIM;
	y = YOFF + DIM;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;
}

static void
emit_xaxis(void)
{
	int x, y;

	if (yzero < 0 || yzero > DIM)
		return;

	buf[k++] = DRAW_LINE;

	x = XOFF;
	y = YOFF + yzero;

	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;

	x = XOFF + DIM;
	y = YOFF + yzero;

	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;
}

static void
emit_yaxis(void)
{
	int x, y;

	if (xzero < 0 || xzero > DIM)
		return;

	buf[k++] = DRAW_LINE;

	x = XOFF + xzero;
	y = YOFF;

	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;

	x = XOFF + xzero;
	y = YOFF + DIM;

	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;
}

static void emit_xscale_f(int, char *);

static void
emit_xscale(void)
{
	sprintf(tbuf, "%g", xmin);
	emit_xscale_f(0, tbuf);
	sprintf(tbuf, "%g", xmax);
	emit_xscale_f(DIM, tbuf);
}

static void
emit_xscale_f(int xx, char *s)
{
	int d, i, len, w, x, y;

	// want to center the number w/o sign

	w = text_width(SMALL_FONT, s);

	if (*s == '-')
		d = w - text_width(SMALL_FONT, s + 1);
	else
		d = 0;

	x = XOFF + xx - (w - d) / 2 - d;
	y = YOFF + DIM + SHIM;

	buf[k++] = SMALL_FONT;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;

	len = (int) strlen(s);

	buf[k++] = (uint8_t) len;

	for (i = 0; i < len; i++)
		buf[k++] = (uint8_t) s[i];
}

static void emit_yscale_f(int, char *);

static void
emit_yscale(void)
{
	sprintf(tbuf, "%g", ymax);
	emit_yscale_f(0, tbuf);
	sprintf(tbuf, "%g", ymin);
	emit_yscale_f(DIM, tbuf);
}

static void
emit_yscale_f(int yy, char *s)
{
	int i, len, w, x, y;

	w = text_width(SMALL_FONT, s);

	x = XOFF - SHIM - w;
	y = YOFF + yy - text_metric[SMALL_FONT].ascent / 2;

	buf[k++] = SMALL_FONT;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;

	len = (int) strlen(s);

	buf[k++] = (uint8_t) len;

	for (i = 0; i < len; i++)
		buf[k++] = (uint8_t) s[i];
}

// emit the '0' axis label

// make sure it doesn't hit the other labels

static void
emit_xzero(void)
{
	int x, y;

	if (xzero < DIM / 4 || xzero > 3 * DIM / 4)
		return;

	x = XOFF + xzero - text_width(SMALL_FONT, "0") / 2;
	y = YOFF + DIM + SHIM;

	buf[k++] = SMALL_FONT;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;
	buf[k++] = 1;
	buf[k++] = '0';
}

// emit the '0' axis label

// make sure it doesn't hit the other labels

static void
emit_yzero(void)
{
	int x, y;

	if (yzero < DIM / 4 || yzero > 3 * DIM / 4)
		return;

	x = XOFF - SHIM - text_width(SMALL_FONT, "0");
	y = YOFF + yzero - text_metric[SMALL_FONT].ascent / 2;

	buf[k++] = SMALL_FONT;
	buf[k++] = (uint8_t) (x >> 8);
	buf[k++] = (uint8_t) x;
	buf[k++] = (uint8_t) (y >> 8);
	buf[k++] = (uint8_t) y;
	buf[k++] = 1;
	buf[k++] = '0';
}
