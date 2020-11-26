#include "app.h"

#define F p3
#define T p4
#define X p5
#define Y p6
#define XT p7
#define YT p8

double tmin, tmax;
double xmin, xmax;
double ymin, ymax;

#define YMAX 2000

struct {
	double x, y, t;
} draw_buf[YMAX];

int draw_count;

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

	save_binding(T); // save binding of 2nd arg

	draw_main();

	restore_binding(T); // restore binding of 2nd arg

	push_symbol(NIL); // draw returns nil
}

void
draw_main(void)
{
	if (drawing)
		return;

	drawing++;

	setup_trange();

	setup_xrange();

	setup_yrange();

	check_for_parametric_draw();

	create_point_set();

	emit_graph();

	drawing--;
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

#define N (GDIM + 1) // +1 fixes scruffy line, for example, draw(-x)

void
create_point_set(void)
{
	int i, n;
	double t;

	draw_count = 0;

	for (i = 1; i < N; i++) {
		t = tmin + (double) i / N * (tmax - tmin);
		new_point(t);
	}

	n = draw_count;

	for (i = 0; i < n - 1; i++)
		fill(i, i + 1);
}

void
new_point(double t)
{
	double x, y;

	if (draw_count == YMAX)
		return;

	get_xy(t);

	if (!isnum(XT) || !isnum(YT))
		return;

	push(XT);
	x = pop_double();

	push(YT);
	y = pop_double();

	x = GDIM * (x - xmin) / (xmax - xmin);
	y = GDIM * (y - ymin) / (ymax - ymin);

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
	int volatile save_expanding;

	drawing++;
	save();

	save_tos = tos;
	save_tof = tof;
	save_expanding = expanding;

	if (setjmp(draw_stop_return)) {
		tos = save_tos;
		tof = save_tof;
		expanding = save_expanding;
		push_symbol(NIL); // result
		restore();
		drawing--;
		return;
	}

	push_double(t);
	p1 = pop();
	set_binding(T, p1);

	push(F);
	eval();
	sfloat();

	restore();
	drawing--;
}

int
invisible(int i)
{
	if (draw_buf[i].x < 0 || draw_buf[i].x > GDIM)
		return 1;
	if (draw_buf[i].y < 0 || draw_buf[i].y > GDIM)
		return 1;
	return 0;
}

void
fill(int i, int j)
{
	int k, n;
	double dx, dy, dt, t;

	if (invisible(i) && invisible(j))
		return;

	dx = draw_buf[i].x - draw_buf[j].x;
	dy = draw_buf[i].y - draw_buf[j].y;

	n = sqrt(dx * dx + dy * dy);

	dt = draw_buf[j].t - draw_buf[i].t;

	for (k = 1; k < n; k++) {
		t = draw_buf[i].t + (double) k / n * dt;
		new_point(t);
	}
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

	p1 = lookup("trange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	sfloat();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	sfloat();
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

	p1 = lookup("xrange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	sfloat();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	sfloat();
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

	p1 = lookup("yrange");

	if (!issymbol(p1))
		return;

	p1 = get_binding(p1);

	// must be two element vector

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	push(p1->u.tensor->elem[0]);
	eval();
	sfloat();
	p2 = pop();

	push(p1->u.tensor->elem[1]);
	eval();
	sfloat();
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

int xzero, yzero;

void
emit_graph(void)
{
	int i;
	double h, x, y;

	emit_level = 1; // small font
	emit_index = 0;
	emit_count = 3 * draw_count + 37; // 37 = 1 for DRAW_END and 6 for each stroke (6 strokes)

	// emit_list advances emit_count, leaves result on stack

	push_double(xmin);
	p1 = pop();
	emit_list(p1);

	push_double(xmax);
	p1 = pop();
	emit_list(p1);

	push_double(ymin);
	p1 = pop();
	emit_list(p1);

	push_double(ymax);
	p1 = pop();
	emit_list(p1);

	emit_display = malloc(sizeof (struct display) + emit_count * sizeof (float));

	if (emit_display == NULL)
		malloc_kaput();

	h = emit_labels(); // uses the results on stack from emit_list

	get_xzero();
	get_yzero();

	emit_box();

	emit_xaxis();
	emit_yaxis();

	for (i = 0; i < draw_count; i++) {
		x = draw_buf[i].x;
		y = GDIM - draw_buf[i].y; // flip the y coordinate
		if (x < 0 || x > GDIM)
			continue;
		if (y < 0 || y > GDIM)
			continue;
		x += DRAW_LEFT_PAD;
		emit_push(DRAW_POINT);
		emit_push(x);
		emit_push(y);
	}

	emit_push(DRAW_END);

	emit_display->type = 1;
	emit_display->color = BLACK;

	emit_display->height = VPAD + GDIM + DRAW_LABEL_PAD + round(h) + VPAD;
	emit_display->width = DRAW_LEFT_PAD + GDIM + DRAW_RIGHT_PAD;

	emit_display->dx = 0.0;
	emit_display->dy = VPAD;

	shipout(emit_display);
}

void
get_xzero(void)
{
	xzero = GDIM * (0.0 - xmin) / (xmax - xmin);
}

void
get_yzero(void)
{
	double y;
	y = GDIM * (0.0 - ymin) / (ymax - ymin);
	yzero = GDIM - y; // flip the y coordinate
}

void
emit_box(void)
{
	double x1, x2, y1, y2;

	x1 = DRAW_LEFT_PAD;
	y1 = 0.0;

	x2 = DRAW_LEFT_PAD + GDIM;
	y2 = GDIM;

	// left

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x1);
	emit_push(y2);
	emit_push(1.0);

	// right

	emit_push(DRAW_STROKE);
	emit_push(x2);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(1.0);

	// top

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y1);
	emit_push(1.0);

	// bottom

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y2);
	emit_push(x2);
	emit_push(y2);
	emit_push(1.0);
}

void
emit_xaxis(void)
{
	double x1, x2, y1, y2;

	if (yzero < 0 || yzero > GDIM)
		return;

	x1 = DRAW_LEFT_PAD;
	y1 = yzero;

	x2 = DRAW_LEFT_PAD + GDIM;
	y2 = yzero;

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(1.0);
}

void
emit_yaxis(void)
{
	double x1, x2, y1, y2;

	if (xzero < 0 || xzero > GDIM)
		return;

	x1 = DRAW_LEFT_PAD + xzero;
	y1 = 0.0;

	x2 = DRAW_LEFT_PAD + xzero;
	y2 = GDIM;

	emit_push(DRAW_STROKE);
	emit_push(x1);
	emit_push(y1);
	emit_push(x2);
	emit_push(y2);
	emit_push(1.0);
}
