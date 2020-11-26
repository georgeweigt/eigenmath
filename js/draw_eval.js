function
draw_eval(F, X, x)
{
	var dx, dy, p, y;

	draw_eval_nib(F, X, x);
	p = pop();
	if (!isnum(p))
		return;
	push(p);
	y = pop_double();

	dx = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	dy = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	dy = DRAW_HEIGHT - dy; // flip

	draw_array.push({x:x, y:y, dx:dx, dy:dy});
}
