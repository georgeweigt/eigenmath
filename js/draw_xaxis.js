function
draw_xaxis()
{
	var y = DRAW_HEIGHT * (1 + ymin / (ymax - ymin));
	draw_line(0, y, DRAW_WIDTH, y, 0.5);
}
