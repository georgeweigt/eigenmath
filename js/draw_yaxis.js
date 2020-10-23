function
draw_yaxis()
{
	var x = DRAW_SIZE * xmin / (xmin - xmax);
	draw_line(x, 0, x, DRAW_SIZE, 0.5);
}
