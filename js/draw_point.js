function
draw_point(F, X, x)
{
	var p, y, t;

	t = x;

	x = DRAW_SIZE * (x - xmin) / (xmax - xmin);

	if (x < 0 || x > DRAW_SIZE)
		return;

	draw_evalf(F, X, t);

	p = pop();

	if (!isdouble(p))
		return;

	y = DRAW_SIZE * (1 - (p.d - ymin) / (ymax - ymin));

	if (y >= 0 && y <= DRAW_SIZE) {
		draw_array.push(t);
		draw_line(x, y, x, y, 2);
	}
}
