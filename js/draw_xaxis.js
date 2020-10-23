function
draw_xaxis()
{
	var y = DRAW_SIZE * (1 + ymin / (ymax - ymin));
	draw_line(0, y, DRAW_SIZE, y, 0.5);
}
