function
draw_point(F, X, x, pass1)
{
	var dx, dy, p, y;

	draw_evalf(F, X, x);
	p = pop();
	if (!isnum(p))
		return;
	push(p);
	y = pop_double();

	dx = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	dy = DRAW_HEIGHT - DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	if (dy > 0 && dy < DRAW_HEIGHT)
		draw_line(dx - 1, dy, dx + 1, dy, 2);

	if (pass1)
		draw_array.push({x:x, y:y, dx:dx, dy:dy});
}
