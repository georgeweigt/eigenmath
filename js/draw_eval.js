function
draw_eval(F, X, x)
{
	var dx, dy, y, p1;

	push_double(x);
	p1 = pop();
	set_binding(X, p1);

	push(F);
	eval_nonstop();
	floatf();
	p1 = pop();

	if (!isnum(p1))
		return;

	push(p1);
	y = pop_double();

	dx = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	dy = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	draw_array.push({x:x, y:y, dx:dx, dy:dy});
}
