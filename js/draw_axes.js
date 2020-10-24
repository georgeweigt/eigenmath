function
draw_axes()
{
	var dx, dy, x, y;

	x = 0;
	y = 0;

	dx = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	dy = DRAW_HEIGHT - DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	if (dx > 0 && dx < DRAW_WIDTH)
		draw_line(dx, 0, dx, DRAW_HEIGHT, 0.5); // vertical axis

	if (dy > 0 && dy < DRAW_HEIGHT)
		draw_line(0, dy, DRAW_WIDTH, dy, 0.5); // horizontal axis
}
