function
draw_yaxis()
{
	var x = DRAW_WIDTH * xmin / (xmin - xmax);
	draw_line(x, 0, x, DRAW_HEIGHT, 0.5);
}
