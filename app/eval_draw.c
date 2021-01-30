#include "app.h"

#define F p3
#define T p4
#define X p5
#define Y p6
#define XT p7
#define YT p8

void
eval_draw(void)
{
	F = cadr(p1); // 1st arg

	T = caddr(p1); // 2nd arg

	if (T == symbol(NIL))
		T = symbol(SYMBOL_X);

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

void
check_for_parametric_draw(void)
{
	eval_f(tmin);
	p1 = pop();

	if (istensor(p1))
		return;

	tmin = xmin;
	tmax = xmax;
}

#define N (DRAW_WIDTH + 1)

void
create_point_set(void)
{
	int i, n;
	double t;

	draw_count = 0;

	for (i = 0; i <= N; i++) {
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

	if (draw_count == DRAW_MAX)
		return;

	get_xy(t);

	if (!isnum(XT) || !isnum(YT))
		return;

	push(XT);
	x = pop_double();

	push(YT);
	y = pop_double();

	x = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	y = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

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

	push(get_binding(F)); // on eval error, binding may be lost

	save_tos = tos;
	save_tof = tof;
	save_expanding = expanding;

	if (setjmp(draw_stop_return)) {
		tos = save_tos;
		tof = save_tof;
		expanding = save_expanding;
		set_binding(F, pop()); // restore binding
		push_symbol(NIL); // return value
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

	p1 = pop();
	pop(); // pop saved binding
	push(p1);

	restore();
	drawing--;
}

int
invisible(int i)
{
	if (draw_buf[i].x < 0 || draw_buf[i].x > DRAW_WIDTH)
		return 1;
	if (draw_buf[i].y < 0 || draw_buf[i].y > DRAW_HEIGHT)
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

void
setup_trange(void)
{
	save();
	setup_trange_nib();
	restore();
}

void
setup_trange_nib(void)
{
	// default range is (-pi, pi)

	tmin = -M_PI;
	tmax = M_PI;

	p1 = get_binding(lookup("trange"));

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
	setup_xrange_nib();
	restore();
}

void
setup_xrange_nib(void)
{
	// default range is (-10,10)

	xmin = -10.0;
	xmax = 10.0;

	p1 = get_binding(lookup("xrange"));

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

void
setup_yrange(void)
{
	save();
	setup_yrange_nib();
	restore();
}

void
setup_yrange_nib(void)
{
	// default range is (-10,10)

	ymin = -10.0;
	ymax = 10.0;

	p1 = get_binding(lookup("yrange"));

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
