function
sample(F, T, t)
{
	var x, y, p1, X, Y;

	push_double(t);
	p1 = pop();
	set_binding(T, p1);

	push(F);
	eval_nonstop();
	floatf();
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

	x = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	y = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	draw_array.push({t:t, x:x, y:y});
}
