function
draw_yaxis()
{
	var x = DRAW_WIDTH * xmin / (xmin - xmax);
	if (x > 0 && x < DRAW_WIDTH)
		draw_line(x, 0, x, DRAW_HEIGHT, 0.5);
}
