function
eval_draw(p1)
{
	var F, T;

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

	draw_buf = [];

	draw_pass1(F, T);
	draw_pass2(F, T);

	emit_graph();

	restore_symbol();

	push_symbol(NIL); // return value

	drawing = 0;
}

function
setup_trange()
{
	var p1, p2, p3;

	tmin = -Math.PI;
	tmax = Math.PI;

	p1 = lookup("trange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 1 || p1.dim[0] != 2)
		return;

	p2 = p1.elem[0];
	p3 = p1.elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	tmin = pop_double();

	push(p3);
	tmax = pop_double();
}

function
setup_xrange()
{
	var p1, p2, p3;

	xmin = -10.0;
	xmax = 10.0;

	p1 = lookup("xrange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 1 || p1.dim[0] != 2)
		return;

	p2 = p1.elem[0];
	p3 = p1.elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	xmin = pop_double();

	push(p3);
	xmax = pop_double();
}

function
setup_yrange()
{
	var p1, p2, p3;

	ymin = -10.0;
	ymax = 10.0;

	p1 = lookup("yrange");
	push(p1);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (!istensor(p1) || p1.dim.length != 1 || p1.dim[0] != 2)
		return;

	p2 = p1.elem[0];
	p3 = p1.elem[1];

	if (!isnum(p2) || !isnum(p3))
		return;

	push(p2);
	ymin = pop_double();

	push(p3);
	ymax = pop_double();
}

function
setup_final(F, T)
{
	var p1;

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

function
draw_pass1(F, T)
{
	var i, t;
	for (i = 0; i <= DRAW_WIDTH; i++) {
		t = tmin + (tmax - tmin) * i / DRAW_WIDTH;
		sample(F, T, t);
	}
}

function
draw_pass2(F, T)
{
	var dt, dx, dy, i, j, m, n, t, t1, t2, x1, x2, y1, y2;

	n = draw_buf.length - 1;

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

		m = Math.sqrt(dx * dx + dy * dy);

		m = Math.floor(m);

		for (j = 1; j < m; j++) {
			t = t1 + dt * j / m;
			sample(F, T, t);
		}
	}
}

function
sample(F, T, t)
{
	var x, y, p1, X, Y;

	push_double(t);
	p1 = pop();
	set_symbol(T, p1, symbol(NIL));

	push(F);
	eval_nonstop();
	floatfunc();
	p1 = pop();

	if (istensor(p1)) {
		X = p1.elem[0];
		Y = p1.elem[1];
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

	if (!isFinite(x) || !isFinite(y))
		return;

	x = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	y = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	draw_buf.push({t:t, x:x, y:y});
}

function
inrange(x, y)
{
	return x > -0.5 && x < DRAW_WIDTH + 0.5 && y > -0.5 && y < DRAW_HEIGHT + 0.5;
}
