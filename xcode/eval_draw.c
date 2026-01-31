#include "app.h"

void
eval_draw(struct atom *p1)
{
	struct atom *F, *T;

	if (drawing) {
		push_symbol(NIL); // not reentrant
		return;
	}

	drawing = 1;

	F = cadr(p1);
	T = caddr(p1);

	if (!isusersymbol(T))
		T = symbol(X_LOWER);

	save_symbol(T);

	setup_trange();
	setup_xrange();
	setup_yrange();

	setup_final(F, T);

	draw_count = 0;

	draw_pass1(F, T);
	draw_pass2(F, T);

	emit_graph();

	restore_symbol();

	push_symbol(NIL); // return value

	drawing = 0;
}

void
setup_trange(void)
{
	struct atom *p1, *p2, *p3;

	tmin = -M_PI;
	tmax = M_PI;

	p1 = lookup("trange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	p2 = p1->u.tensor->elem[0];
	p3 = p1->u.tensor->elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	tmin = pop_double();

	push(p3);
	tmax = pop_double();
}

void
setup_xrange(void)
{
	struct atom *p1, *p2, *p3;

	xmin = -10.0;
	xmax = 10.0;

	p1 = lookup("xrange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	p2 = p1->u.tensor->elem[0];
	p3 = p1->u.tensor->elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	xmin = pop_double();

	push(p3);
	xmax = pop_double();
}

void
setup_yrange(void)
{
	struct atom *p1, *p2, *p3;

	ymin = -10.0;
	ymax = 10.0;

	p1 = lookup("yrange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 1 || p1->u.tensor->nelem != 2)
		return;

	p2 = p1->u.tensor->elem[0];
	p3 = p1->u.tensor->elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	ymin = pop_double();

	push(p3);
	ymax = pop_double();
}

void
setup_final(struct atom *F, struct atom *T)
{
	struct atom *p1;

	push_double(tmin);
	p1 = pop();
	set_symbol(T, p1, symbol(NIL));

	push(F);
	eval_nonstop();
	p1 = pop();

	if (!istensor(p1)) {
		tmin = xmin;
		tmax = xmax;
	}
}

void
draw_pass1(struct atom *F, struct atom *T)
{
	int i;
	double t;
	for (i = 0; i <= DRAW_WIDTH; i++) {
		t = tmin + (tmax - tmin) * i / DRAW_WIDTH;
		sample(F, T, t);
	}
}

void
draw_pass2(struct atom *F, struct atom *T)
{
	int i, j, m, n;
	double dt, dx, dy, t, t1, t2, x1, x2, y1, y2;

	n = draw_count - 1;

	for (i = 0; i < n; i++) {

		t1 = draw_buf[i].t;
		x1 = draw_buf[i].x;
		y1 = draw_buf[i].y;

		t2 = draw_buf[i + 1].t;
		x2 = draw_buf[i + 1].x;
		y2 = draw_buf[i + 1].y;

		if (!inrange(x1, y1) && !inrange(x2, y2))
			continue;

		dt = t2 - t1;
		dx = x2 - x1;
		dy = y2 - y1;

		m = floor(sqrt(dx * dx + dy * dy));

		for (j = 1; j < m; j++) {
			t = t1 + dt * j / m;
			sample(F, T, t);
		}
	}
}

void
sample(struct atom *F, struct atom *T, double t)
{
	double x, y;
	struct atom *p1, *X, *Y;

	push_double(t);
	p1 = pop();
	set_symbol(T, p1, symbol(NIL));

	push(F);
	eval_nonstop();
	floatfunc();
//	real();
	p1 = pop();

	if (istensor(p1)) {
		X = p1->u.tensor->elem[0];
		Y = p1->u.tensor->elem[1];
	} else {
		push_double(t);
		X = pop();
		Y = p1;
	}

	if (!isnum(X) || !isnum(Y))
		return;

	push(X);
	x = pop_double();

	push(Y);
	y = pop_double();

	if (!isfinite(x) || !isfinite(y))
		return;

	x = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	y = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	if (draw_count == draw_max) {
		draw_max += 1000;
		draw_buf = realloc(draw_buf, draw_max * sizeof (struct draw_buf_t));
		if (draw_buf == NULL)
			exit(1);
	}

	draw_buf[draw_count].x = x;
	draw_buf[draw_count].y = y;
	draw_buf[draw_count].t = t;

	draw_count++;
}

int
inrange(double x, double y)
{
	return x > -0.5 && x < DRAW_WIDTH + 0.5 && y > -0.5 && y < DRAW_HEIGHT + 0.5;
}
